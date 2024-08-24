#include "FAT.h"
#include <fstream>

std::string DirectoryEntry::GetName()
{
	std::string nameString = this->name;
	if (nameString.size() != 0)
	{
		while (nameString.back() == ' ')
		{
			nameString.pop_back();
		}
	}
	return nameString;
}

std::string DirectoryEntry::GetExtension()
{
	std::string extensionString = this->extension;
	if (extensionString.size() == 0)
	{
		while (extensionString.back() == ' ')
		{
			extensionString.pop_back();
		}
	}
	return extensionString;
}

DirectoryEntry ReadDirectoryEntry(std::fstream* disk)
{
	unsigned char bytes[8];
	DirectoryEntry newDirectoryEntry;

	disk->read((char*)bytes, 8);
	newDirectoryEntry.name.append((char*)bytes, 8);

	disk->read((char*)bytes, 3);
	newDirectoryEntry.extension.append((char*)bytes, 3);

	disk->read((char*)bytes, 1);
	newDirectoryEntry.fileAttributes = bytes[0];

	disk->read((char*)bytes, 1);
	newDirectoryEntry.userAttributes = bytes[0];

	disk->read((char*)bytes, 1);
	newDirectoryEntry.firstCharacterOfDeletedFile = bytes[0];

	disk->read((char*)bytes, 2);
	newDirectoryEntry.passwordHash = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 2);
	newDirectoryEntry.recordSize = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 2);
	newDirectoryEntry.ownerID = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 2);
	newDirectoryEntry.fileAccessRightsBitmap = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 2);
	newDirectoryEntry.lastModifiedTime = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 2);
	newDirectoryEntry.lastModifiedDate = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 2);
	newDirectoryEntry.clusterStart = ((bytes[1] << 8) + bytes[0]);

	disk->read((char*)bytes, 4);
	newDirectoryEntry.fileSize = ((bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0]);

	return newDirectoryEntry;
}

unsigned int GetFreeCluster(std::fstream* disk, unsigned int firstAddressOfFAT)
{
	unsigned int result = 2;
	unsigned char bytes[2];
	disk->clear(std::ios_base::goodbit);
	unsigned int oldAddress = disk->tellg();
	while (result != 511)
	{
		disk->seekg(firstAddressOfFAT + result * 3 / 2);
		disk->read((char*)bytes, 2);
		if (result % 2 == 0)
		{
			unsigned int value = (((bytes[1] << 8) + bytes[0]) & 0xFFF);
			if (value == 0)
				break;
		}
		else
		{
			unsigned int value = (((bytes[1] << 8) + bytes[0]) >> 4);
			if (value == 0)
				break;
		}
		result += 1;
	}
	disk->seekg(oldAddress);
	return result;
}

void WriteClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster, unsigned int value)
{
	unsigned char bytes[2];
	disk->clear(std::ios_base::goodbit);
	unsigned int oldAddress = disk->tellg();
	disk->seekg(firstAddressOfFAT + cluster * 3 / 2);
	disk->read((char*)bytes, 2);
	disk->seekg(firstAddressOfFAT + cluster * 3 / 2);
	if (cluster % 2 == 0)
	{
		bytes[1] = value >> 8 | (bytes[1] & 0xF0);
		bytes[0] = value;
	}
	else
	{
		bytes[1] = value >> 4;
		bytes[0] = value << 4 | (bytes[0] & 0x0F);
	}
	disk->write((char*)bytes, 2);
	disk->seekg(oldAddress);
}

unsigned int GetClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster)
{
	unsigned char bytes[2];
	unsigned int result;
	disk->clear(std::ios_base::goodbit);
	unsigned int oldAddress = disk->tellg();
	disk->seekg(firstAddressOfFAT + cluster * 3 / 2);
	disk->read((char*)bytes, 2);
	if (cluster % 2 == 0)
		result = ((bytes[1] << 8) + bytes[0]) & 0xFFF;
	else
		result = ((bytes[1] << 8) + bytes[0]) << 4;
	disk->seekg(oldAddress);
	return result;
}

void WriteDirectoryTable(std::fstream* disk, DirectoryEntry directoryEntry)
{
	unsigned char bytes[4];
	directoryEntry.name.resize(8, 0x20);
	directoryEntry.extension.resize(3, 0x20);
	disk->write(directoryEntry.name.c_str(), 8);
	disk->write(directoryEntry.extension.c_str(), 3);
	bytes[0] = directoryEntry.fileAttributes;
	disk->write((char*)bytes, 1);
	bytes[0] = directoryEntry.userAttributes;
	disk->write((char*)bytes, 1);
	bytes[0] = directoryEntry.firstCharacterOfDeletedFile;
	disk->write((char*)bytes, 1);
	bytes[0] = directoryEntry.passwordHash;
	bytes[1] = directoryEntry.passwordHash >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.recordSize;
	bytes[1] = directoryEntry.recordSize >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.ownerID;
	bytes[1] = directoryEntry.ownerID >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.fileAccessRightsBitmap;
	bytes[1] = directoryEntry.fileAccessRightsBitmap >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.lastModifiedTime;
	bytes[1] = directoryEntry.lastModifiedTime >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.lastModifiedDate;
	bytes[1] = directoryEntry.lastModifiedDate >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.clusterStart;
	bytes[1] = directoryEntry.clusterStart >> 8;
	disk->write((char*)bytes, 2);
	bytes[0] = directoryEntry.fileSize;
	bytes[1] = directoryEntry.fileSize >> 8;
	bytes[2] = directoryEntry.fileSize >> 16;
	bytes[3] = directoryEntry.fileSize >> 24;
	disk->write((char*)bytes, 4);
}

DirectoryEntry FindDirectoryEntry(std::fstream* disk, unsigned int currentDirectory, std::string name)
{
	DirectoryEntry newDirectoryEntry;
	disk->seekg(currentDirectory);
	while (true)
	{
		DirectoryEntry directoryEntry = ReadDirectoryEntry(disk);

		if (directoryEntry.name[0] == 0)
			break;
		else if (directoryEntry.name[0] == 0xE5)
			continue;

		std::string nameString = directoryEntry.name;
		while (nameString.back() == ' ')
		{
			nameString.pop_back();
			if (nameString.size() == 0)
				break;
		}
		std::string extensionString = directoryEntry.extension;
		while (extensionString.back() == ' ')
		{
			extensionString.pop_back();
			if (extensionString.size() == 0)
				break;
		}

		if (nameString == name)
		{
			newDirectoryEntry = directoryEntry;
			break;
		}
	}
	return newDirectoryEntry;
}
