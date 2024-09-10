#pragma once
#include <string>

struct DirectoryEntry
{
	DirectoryEntry();
	DirectoryEntry(std::string name, std::string extension, unsigned char fileAttributes, unsigned char userAttributes, unsigned char firstCharacterOfDeletedFile, unsigned short passwordHash, unsigned short recordSize, unsigned short ownerID, unsigned short fileAccessRightsBitmap, unsigned short lastModifiedTime, unsigned short lastModifiedDate, unsigned short clusterStart, unsigned int fileSize);

	unsigned char name[9];
	unsigned char extension[4];
	unsigned char fileAttributes;
	unsigned char userAttributes;
	unsigned char firstCharacterOfDeletedFile;
	unsigned short passwordHash;
	unsigned short recordSize;
	unsigned short ownerID;
	unsigned short fileAccessRightsBitmap;
	unsigned short lastModifiedTime;
	unsigned short lastModifiedDate;
	unsigned short clusterStart;
	unsigned int fileSize;

	std::string GetName();
	std::string GetExtension();
};

unsigned int GetFreeCluster(std::fstream* disk, unsigned int firstAddressOfFAT);
void WriteClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster, unsigned int value);
unsigned int GetClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster);

DirectoryEntry ReadDirectoryEntry(std::fstream* disk);
void WriteDirectoryTable(std::fstream* disk, DirectoryEntry directoryEntry);
DirectoryEntry FindDirectoryEntry(std::fstream* disk, unsigned int currentDirectory, std::string name);
