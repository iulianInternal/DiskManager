#pragma once
#include <string>

struct DirectoryEntry
{
	std::string name = "";
	std::string extension = "";
	unsigned __int8 fileAttributes = 0;
	unsigned __int8 userAttributes = 0;
	unsigned __int8 firstCharacterOfDeletedFile = 0;
	unsigned __int16 passwordHash = 0;
	unsigned __int16 recordSize = 0;
	unsigned __int16 ownerID = 0;
	unsigned __int16 fileAccessRightsBitmap = 0;
	unsigned __int16 lastModifiedTime = 0;
	unsigned __int16 lastModifiedDate = 0;
	unsigned __int16 clusterStart = 0;
	unsigned __int32 fileSize = 0;
};

unsigned int GetFreeCluster(std::fstream* disk, unsigned int firstAddressOfFAT);
void WriteClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster, unsigned int value);
unsigned int GetClusterChain(std::fstream* disk, unsigned int firstAddressOfFAT, unsigned int cluster);

DirectoryEntry ReadDirectoryEntry(std::fstream* disk);
void WriteDirectoryTable(std::fstream* disk, DirectoryEntry directoryEntry);
