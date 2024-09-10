#include "FAT.h"
#include <fstream>

DirectoryEntry::DirectoryEntry()
{
	memcpy(this->name, "        ", 9);
	memcpy(this->extension, "   ", 4);
	this->fileAttributes = 0;
	this->userAttributes = 0;
	this->firstCharacterOfDeletedFile = 0;
	this->passwordHash = 0;
	this->recordSize = 0;
	this->ownerID = 0;
	this->fileAccessRightsBitmap = 0;
	this->lastModifiedTime = 0;
	this->lastModifiedDate = 0;
	this->clusterStart = 0;
	this->fileSize = 0;
}

DirectoryEntry::DirectoryEntry(std::string name, std::string extension, unsigned char fileAttributes, unsigned char userAttributes, unsigned char firstCharacterOfDeletedFile, unsigned short passwordHash, unsigned short recordSize, unsigned short ownerID, unsigned short fileAccessRightsBitmap, unsigned short lastModifiedTime, unsigned short lastModifiedDate, unsigned short clusterStart, unsigned int fileSize)
{
	memcpy(this->name, name.c_str(), 8);
	memcpy(this->extension, extension.c_str(), 3);
	this->fileAttributes = fileAttributes;
	this->userAttributes = userAttributes;
	this->firstCharacterOfDeletedFile = firstCharacterOfDeletedFile;
	this->passwordHash = passwordHash;
	this->recordSize = recordSize;
	this->ownerID = ownerID;
	this->fileAccessRightsBitmap = fileAccessRightsBitmap;
	this->lastModifiedTime = lastModifiedTime;
	this->lastModifiedDate = lastModifiedDate;
	this->clusterStart = clusterStart;
	this->fileSize = fileSize;
}

std::string DirectoryEntry::GetName()
{
	std::string nameString = (char*)this->name;
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
	std::string extensionString = (char*)this->extension;
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

	disk->read((char*)newDirectoryEntry.name, 8);

	disk->read((char*)newDirectoryEntry.extension, 3);

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
	disk->write((char*)directoryEntry.name, 8);
	disk->write((char*)directoryEntry.extension, 3);
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

		std::string nameString = (char*)directoryEntry.name;
		while (nameString.back() == ' ')
		{
			nameString.pop_back();
			if (nameString.size() == 0)
				break;
		}
		std::string extensionString = (char*)directoryEntry.extension;
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
