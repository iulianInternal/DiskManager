#pragma once
#include <string>

struct DirectoryEntry
{
	std::string name = "";
	std::string extension = "";
	unsigned char fileAttributes = 0;
	unsigned char userAttributes = 0;
	unsigned char firstCharacterOfDeletedFile = 0;
	unsigned short passwordHash = 0;
	unsigned short recordSize = 0;
	unsigned short ownerID = 0;
	unsigned short fileAccessRightsBitmap = 0;
	unsigned short lastModifiedTime = 0;
	unsigned short lastModifiedDate = 0;
	unsigned short clusterStart = 0;
	unsigned int fileSize = 0;
};

unsigned int GetFreeCluster(std::fstream* disk, unsigned int firstAddressOfFAT);
void WriteClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster, unsigned int value);
unsigned int GetClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster);

DirectoryEntry ReadDirectoryEntry(std::fstream* disk);
void WriteDirectoryTable(std::fstream* disk, DirectoryEntry directoryEntry);
DirectoryEntry FindDirectoryEntry(std::fstream* disk, unsigned int currentDirectory, std::string name);
