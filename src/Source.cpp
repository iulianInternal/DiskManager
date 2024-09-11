#include "FAT.h"
#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>

//Some OS has unique set of byte to correspond as a folder path.
#ifdef __linux__
#define FILESYSTEM_FOLDER_PATH_SYMBOL '/'
#elif defined(_WIN32) || defined(_WIN64)
#define FILESYSTEM_FOLDER_PATH_SYMBOL '\\'
#endif

std::vector<std::string> getArgs(const char* args)
{
	std::vector<std::string> arguments;
	std::string string;
	int argc = 0;
	bool flag1 = false;
	bool flag2 = false;
	while (true)
	{
		if (*args == 0)
			break;
		if (*args == ' ' && flag2 == false)
		{
			flag1 = false;
			++args;
			continue;
		}
		if (*args == '"' && flag1 == false)
		{
			if (flag2 == false)
				argc += 1;
			flag2 = !flag2;
			++args;
			continue;
		}
		if ((*args > 0x21 && *args < 0x7E) && flag1 == false && flag2 == false)
		{
			argc += 1;
			flag1 = true;
		}
		if (arguments.size() != argc)
			arguments.push_back(string);
		arguments[argc - 1].append(1, *args);
		++args;
	}
	return arguments;
}

int main(int argc, char* argv[])
{
	std::cout << "Disk manager 0.1.1\r\n(c) iulianInternal 2024" << std::endl;
	std::cout << std::endl;

	if (argc < 2)
	{
#ifdef __linux__
		std::cout << "Usage: ./DiskManager <disk> [options]" << std::endl << std::endl;
#elif defined(_WIN32) || defined(_WIN64)
		std::cout << "Usage: DiskManager.exe <disk> [options]" << std::endl << std::endl;
#endif
		std::cout << "Available options:" << std::endl << std::endl;
		std::cout << "    -c:number - create a new diskette image." << std::endl;
		std::cout << "                NOTE! It can erase all data in specified file.So it can recreate file." << std::endl;
		std::cout << "                Accepts only 160kb and 360kb diskette sizes." << std::endl << std::endl;
		std::cout << "    -s file   - a relative or absolute path of a script to run program commands." << std::endl;
		std::cout << "                Useful if you want to automate diskette creation." << std::endl;
		return 0;
	}

	std::ifstream script;

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] != 0)
			{
				switch (argv[i][1])
				{
				//Create a disk with predefined size
				case 'c':
				{
					std::string create = argv[i] + sizeof(char) * 2;
					if (create == ":160")
					{
						std::string bootSector = { (char)0xEB, (char)0x17, (char)0x90,
													'M','S','D','O','S','2','.','0', //OEMName
													(char)0x00,(char)0x02, //Bytes per logical sector
													(char)0x01, //Logical sectors per cluster
													(char)0x01,(char)0x00, //Reserved sectors
													(char)0x02, //Number of FAT
													(char)0x40,(char)0x00, //Max number of FAT
													(char)0x40,(char)0x01, //Total logical sectors
													(char)0xFE, //Media descriptor
													(char)0x01,(char)0x00, //Logical sectors per FAT
													(char)0x00, //End of BIOS Parameter Block
							//ASM to show that this disk is not bootable.
													(char)0xB3,(char)0x30,				//MOV BL,30
							//printInfo:
													(char)0x80,(char)0xC3,(char)0x01,	//ADD BL,01
													(char)0x8A,(char)0x07,				//MOV AL,[BX]
													(char)0x3C,(char)0x00,				//CMP AL,00
													(char)0x74,(char)0xFE,				//JMP $
													(char)0xB4,(char)0x0E,				//MOV AH 0x0E
													(char)0xCD,(char)0x10,				//INT 10
													(char)0x80,(char)0xC2,(char)0x01,	//ADD DL,01
													(char)0xB4,(char)0x02,				//MOV AH 0x02
													(char)0xCD,(char)0x10,				//INT 10
													(char)0xEB,(char)0xEA,				//JMP printInfo
													'T','h','i','s',' ','d','i','s','k',' ','i','s',' ','n','o','t',' ','b','o','o','t','a','b','l','e','.','\r','\n',
													'D','i','s','k',' ','i','s',' ','g','e','n','e','r','a','t','e','d',' ','b','y',' ','D','i','s','k','M','a','n','a','g','e','r','.',
						};
						bootSector.resize(512);
						bootSector[510] = 0x55;
						bootSector[511] = 0xAA;
						std::ofstream newDisk;
						newDisk.open(argv[1], std::ios_base::out | std::ios_base::binary);
						newDisk.write(bootSector.c_str(), 512);
						char symbol = 0xFE;
						newDisk.write(&symbol, 1);
						symbol = 0xFF;
						newDisk.write(&symbol, 1);
						newDisk.write(&symbol, 1);
						symbol = 0;
						for (int write = 515; write < 163840; write++)
						{
							newDisk.write(&symbol, 1);
						}
						newDisk.close();
					}
					else if (create == ":360")
					{
						std::string bootSector = { (char)0xEB, (char)0x17, (char)0x90,
													'M','S','D','O','S','2','.','0', //OEMName
													(char)0x00,(char)0x02, //Bytes per logical sector
													(char)0x02, //Logical sectors per cluster
													(char)0x01,(char)0x00, //Reserved sectors
													(char)0x02, //Number of FAT
													(char)0x70,(char)0x00, //Max number of FAT
													(char)0xD0,(char)0x02, //Total logical sectors
													(char)0xFD, //Media descriptor
													(char)0x02,(char)0x00, //Logical sectors per FAT
													(char)0x00, //End of BIOS Parameter Block
							//ASM to show that this disk is not bootable.
													(char)0xB3,(char)0x30,				//MOV BL,30
							//printInfo:
													(char)0x80,(char)0xC3,(char)0x01,	//ADD BL,01
													(char)0x8A,(char)0x07,				//MOV AL,[BX]
													(char)0x3C,(char)0x00,				//CMP AL,00
													(char)0x74,(char)0xFE,				//JMP $
													(char)0xB4,(char)0x0E,				//MOV AH 0x0E
													(char)0xCD,(char)0x10,				//INT 10
													(char)0x80,(char)0xC2,(char)0x01,	//ADD DL,01
													(char)0xB4,(char)0x02,				//MOV AH 0x02
													(char)0xCD,(char)0x10,				//INT 10
													(char)0xEB,(char)0xEA,				//JMP printInfo
													'T','h','i','s',' ','d','i','s','k',' ','i','s',' ','n','o','t',' ','b','o','o','t','a','b','l','e','.','\r','\n',
													'D','i','s','k',' ','i','s',' ','g','e','n','e','r','a','t','e','d',' ','b','y',' ','D','i','s','k','M','a','n','a','g','e','r','.',
						};
						bootSector.resize(512);
						bootSector[510] = 0x55;
						bootSector[511] = 0xAA;
						std::ofstream newDisk;
						newDisk.open(argv[1], std::ios_base::out | std::ios_base::binary);
						newDisk.write(bootSector.c_str(), 512);
						char symbol = 0xFD;
						newDisk.write(&symbol, 1);
						symbol = 0xFF;
						newDisk.write(&symbol, 1);
						newDisk.write(&symbol, 1);
						symbol = 0;
						for (int write = 515; write < 368640; write++)
						{
							newDisk.write(&symbol, 1);
						}
						newDisk.close();
					}
					else
					{
						std::cout << "Invalid option." << std::endl;
					}
					break;
				}
				case 's':
				{
					std::string fileName = argv[i] + sizeof(char) * 3;
					script.open(fileName, std::ios_base::in);
					if (!script.is_open())
						std::cout << "Invalid script file." << std::endl;
					break;
				}
				default:
				{
					std::cout << "Unknown option." << std::endl;
					break;
				}
				}
			}
			else
			{
				std::cout << "Unknown option." << std::endl;
			}
		}
		else
		{
			//might be smth else here, maybe.
		}
	}

	std::fstream disk;
	disk.open(argv[1], std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	if (!disk.is_open())
	{
		std::cout << "Disk was not found." << std::endl;
		return 0;
	}
	unsigned int maxFileSize = std::filesystem::file_size(argv[1]);

	std::string diskName = argv[1];
	diskName.erase(0, diskName.rfind("\\")+1);

	unsigned char bytes[4];

	unsigned int offset = 0;

	unsigned char OEMName[9];
	unsigned short bytesPerLogicalSector;
	unsigned char logicalSectorPerCluster;
	unsigned short reservedLogicalSectors;
	unsigned char numberOfFAT;
	unsigned short maxNumberOfFAT;
	unsigned short totalLogicalSector;
	unsigned char mediaDescriptor;
	unsigned short logicalSectorPerFAT;

	disk.read((char*)bytes, 3);

	//For some reason, on some systems structure of boot sector is different. So we skip first sector.
	if (!((bytes[0] == 0xEB && bytes[2] == 0x90) || (bytes[0] == 0xE9) || (bytes[0] == 0x90 && bytes[1] == 0xEB)))
	{
		disk.seekg(0x200+0x003);
		offset = 0x200;
	}

	disk.read((char*)OEMName, 8);
	OEMName[8] = 0;

	disk.read((char*)bytes, 2);
	bytesPerLogicalSector = ((bytes[1] << 8) + bytes[0]);

	disk.read((char*)bytes, 1);
	logicalSectorPerCluster = bytes[0];

	disk.read((char*)bytes, 2);
	reservedLogicalSectors = ((bytes[1] << 8) + bytes[0]);

	disk.read((char*)bytes, 1);
	numberOfFAT = bytes[0];

	disk.read((char*)bytes, 2);
	maxNumberOfFAT = ((bytes[1] << 8) + bytes[0]);

	disk.read((char*)bytes, 2);
	totalLogicalSector = ((bytes[1] << 8) + bytes[0]);

	disk.read((char*)bytes, 1);
	mediaDescriptor = bytes[0];

	disk.read((char*)bytes, 2);
	logicalSectorPerFAT = ((bytes[1] << 8) + bytes[0]);

	std::cout << "Disk info:" << std::endl;
	std::cout << "Name file: " << diskName << std::endl;
	std::cout << "OEMName: " << OEMName << std::endl;
	std::cout << "Bytes per logical sector: " << std::to_string(bytesPerLogicalSector) << std::endl;
	std::cout << "Logical sectors per cluster: " << std::to_string(logicalSectorPerCluster) << std::endl;
	std::cout << "Reserved logical sectors: " << std::to_string(reservedLogicalSectors) << std::endl;
	std::cout << "Number of FAT: " << std::to_string(numberOfFAT) << std::endl;
	std::cout << "Maximum number of FAT: " << std::to_string(maxNumberOfFAT) << std::endl;
	std::cout << "Total logical sector: " << std::to_string(totalLogicalSector) << std::endl;
	std::cout << "Media descriptor: " << std::to_string(mediaDescriptor) << std::endl;
	std::cout << "Logical sectors per FAT: " << std::to_string(logicalSectorPerFAT) << std::endl;

	unsigned int firstAddressOfFAT = reservedLogicalSectors * bytesPerLogicalSector;
	unsigned int rootDirectory = firstAddressOfFAT + numberOfFAT * logicalSectorPerFAT * bytesPerLogicalSector + offset;
	unsigned int currentDirectory = rootDirectory;
	unsigned int addressRegion = rootDirectory + maxNumberOfFAT * 32;
	std::string currentDirectoryString = "root:\\\\";
	std::cout << "First address of FAT: " << std::to_string(firstAddressOfFAT) << std::endl;
	std::cout << "Root directory location: " << std::to_string(currentDirectory) << std::endl;
	std::cout << "Address region location: " << std::to_string(addressRegion) << std::endl;
	std::cout << std::endl;

	disk.seekg(currentDirectory);

	while (true)
	{
		std::string command;
		if (script.is_open())
		{
			std::getline(script, command);
			if (script.eof())
			{
				script.close();
				std::getline(std::cin, command);
			}
			else
				std::cout << command;
		}
		else
			std::getline(std::cin, command);

		disk.clear(std::ios_base::goodbit);

		std::vector<std::string> arguments = getArgs(command.c_str());

		if (arguments.size() == 0)
		{
			std::cout << "Invalid command. Use \"help\" for command list." << std::endl;
			std::cout << std::endl;
			continue;
		}
		disk.seekg(currentDirectory);
		if (arguments[0] == "dir")
		{
			std::cout << "Directory of " << currentDirectoryString << std::endl;

			if (arguments.size() == 2)
			{
				for (int i = 0; i < arguments[1].length(); i++)
				{
					arguments[1][i] = std::toupper(arguments[1][i]);
				}

				bool found = true;
				size_t firstPos = arguments[1].find('\\');
				DirectoryEntry directoryEntry;

				while (firstPos != std::string::npos || firstPos == arguments[1].length() - 1)
				{
					directoryEntry = FindDirectoryEntry(&disk, (unsigned int)disk.tellg(), arguments[1].substr(0, firstPos));
					if (directoryEntry.name[0] == 0)
					{
						std::cout << "Directory " << arguments[1].substr(0, firstPos) << " does not exist." << std::endl;
						std::cout << std::endl;
						found = false;
						break;
					}
					if ((directoryEntry.fileAttributes & 0x10) != 0x10)
					{
						std::cout << arguments[1] << " is not a directory" << std::endl;
						found = false;
						break;
					}
					unsigned int addressDirectory = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
					disk.seekg(directoryEntry.clusterStart != 0 ? addressDirectory : rootDirectory);

					arguments[1].erase(0, firstPos + 1);
					firstPos = arguments[1].find('\\');
				}
				if (found == false)
					continue;

				directoryEntry = FindDirectoryEntry(&disk, (unsigned int)disk.tellg(), arguments[1].substr(0, firstPos));
				if (directoryEntry.name[0] == 0)
				{
					std::cout << "Directory " << arguments[1].substr(0, firstPos) << " does not exist." << std::endl;
					std::cout << std::endl;
					continue;
				}
				if ((directoryEntry.fileAttributes & 0x10) != 0x10)
				{
					std::cout << arguments[1] << " is not a directory" << std::endl;
					continue;
				}
				unsigned int addressDirectory = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
				disk.seekg(directoryEntry.clusterStart != 0 ? addressDirectory : rootDirectory);
			}

			while (true)
			{
				DirectoryEntry directoryEntry = ReadDirectoryEntry(&disk);

				if (directoryEntry.name[0] == 0)
					break;
				else if (directoryEntry.name[0] == 0xE5)
					continue;

				unsigned char hour, minute, second;
				std::string lastModifiedTimeString;
				hour = directoryEntry.lastModifiedTime >> 11;
				minute = (directoryEntry.lastModifiedTime % 2048) >> 5;
				second = directoryEntry.lastModifiedTime % 32;
				if (hour < 10)
					lastModifiedTimeString.append("0");
				lastModifiedTimeString.append(std::to_string(hour)).append(":");
				if (minute < 10)
					lastModifiedTimeString.append("0");
				lastModifiedTimeString.append(std::to_string(minute)).append(":");
				if (second * 2 < 10)
					lastModifiedTimeString.append("0");
				lastModifiedTimeString.append(std::to_string(second * 2));

				unsigned short year, month, day = 0;
				std::string lastModifiedDateString;
				year = directoryEntry.lastModifiedDate >> 9;
				month = (directoryEntry.lastModifiedDate % 512) >> 5;
				day = directoryEntry.lastModifiedDate % 32;
				if (day < 10)
					lastModifiedDateString.append("0");
				lastModifiedDateString.append(std::to_string(day)).append(".");
				if (month < 10)
					lastModifiedDateString.append("0");
				lastModifiedDateString.append(std::to_string(month)).append(".");
				lastModifiedDateString.append(std::to_string(year + 1980));

				std::string fileSizeString = std::to_string(directoryEntry.fileSize);
				fileSizeString.resize(5, ' ');

				std::cout << directoryEntry.name << " " << directoryEntry.extension << " ";

				if ((directoryEntry.fileAttributes & 0x10) == 0x10)
					std::cout << "<DIR>";
				else
					std::cout << fileSizeString;

				std::cout << " " << lastModifiedDateString << " " << lastModifiedTimeString << " ";

				std::cout << std::endl;
			}
		}
		else if (arguments[0] == "cd")
		{
			for (int i = 0; i < arguments[1].length(); i++)
			{
				arguments[1][i] = std::toupper(arguments[1][i]);
			}
			std::string newPath = currentDirectoryString;

			bool found = true;
			size_t firstPos = arguments[1].find('\\');
			DirectoryEntry directoryEntry;

			while (firstPos != std::string::npos || firstPos == arguments[1].length()-1)
			{
				directoryEntry = FindDirectoryEntry(&disk, (unsigned int)disk.tellg(), arguments[1].substr(0, firstPos));
				if (directoryEntry.name[0] == 0)
				{
					std::cout << "Directory " << arguments[1].substr(0, firstPos) << " does not exist." << std::endl;
					std::cout << std::endl;
					found = false;
					break;
				}
				if ((directoryEntry.fileAttributes & 0x10) != 0x10)
				{
					std::cout << arguments[1] << " is not a directory" << std::endl;
					found = false;
					break;
				}
				newPath.append(arguments[1].substr(0, firstPos)).append("\\");
				if (directoryEntry.name[0] == '.' && directoryEntry.name[1] == 0x20)
					newPath.erase(newPath.rfind("\\", newPath.length() - 2) + 1);
				else if (directoryEntry.name[0] == '.' && directoryEntry.name[1] == '.')
				{
					newPath.erase(newPath.rfind("\\", newPath.length() - 2) + 1);
					newPath.erase(newPath.rfind("\\", newPath.length() - 2) + 1);
				}
				unsigned int addressDirectory = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
				disk.seekg(directoryEntry.clusterStart != 0 ? addressDirectory : rootDirectory);

				arguments[1].erase(0, firstPos + 1);
				firstPos = arguments[1].find('\\');
			}
			if (found == false)
				continue;

			directoryEntry = FindDirectoryEntry(&disk, (unsigned int)disk.tellg(), arguments[1]);
			if (directoryEntry.name[0] == 0)
			{
				std::cout << "Invalid directory" << std::endl;
				continue;
			}
			if ((directoryEntry.fileAttributes & 0x10) != 0x10)
			{
				std::cout << arguments[1] << " is not a directory" << std::endl;
				continue;
			}

			unsigned int addressDirectory = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
			currentDirectory = directoryEntry.clusterStart != 0 ? addressDirectory : rootDirectory;
			
			newPath.append(arguments[1]).append("\\");
			if (directoryEntry.name[0] == '.' && directoryEntry.name[1] == 0x20)
				newPath.erase(newPath.rfind("\\", newPath.length() - 2) + 1);
			else if (directoryEntry.name[0] == '.' && directoryEntry.name[1] == '.')
			{
				newPath.erase(newPath.rfind("\\", newPath.length() - 2) + 1);
				newPath.erase(newPath.rfind("\\", newPath.length() - 2) + 1);
			}
			currentDirectoryString = newPath;
			std::cout << "Changed directory to " << currentDirectoryString << std::endl;

		}
		else if (arguments[0] == "copy")
		{
			if (arguments.size() == 3)
			{
				//If copying from img to disk
				if (arguments[1].substr(0, 7) == "root://")
				{
					arguments[1].erase(0, 7);
					for (int i = 0; i < arguments[1].length(); i++)
					{
						arguments[1][i] = std::toupper(arguments[1][i]);
					}

					//Create new folders (if they don't exist)
					bool success = true;
					size_t firstPos = arguments[2].find(FILESYSTEM_FOLDER_PATH_SYMBOL);

					while (firstPos != std::string::npos || firstPos == arguments[2].length() - 1)
					{
						std::string folder = arguments[2].substr(0, firstPos);
						if (!std::filesystem::exists(folder))
						{
							success = false;
							std::cout << "Cannot create folder \"" << folder << "\"" << std::endl;
							break;
						}

						arguments[2].erase(0, firstPos + 1);
						firstPos = arguments[2].find('\\');
					}
					if (success == false)
						continue;

					std::string namePath = "";
					std::string directoryPath;
					std::vector<unsigned int> exitStack;

					while (true)
					{
						DirectoryEntry directoryEntry = ReadDirectoryEntry(&disk);

						if (directoryEntry.name[0] == 0)
						{
							if (exitStack.size() == 0)
								break;
							else
							{
								disk.seekg(exitStack[exitStack.size() - 1]);
								exitStack.pop_back();
								namePath.erase(namePath.rfind("\\"));
								continue;
							}
						}
						else if ((unsigned char)directoryEntry.name[0] == 0xE5)
							continue;

						std::string nameString = directoryEntry.GetName();
						std::string extensionString = directoryEntry.GetExtension();

						if (nameString == arguments[1] || exitStack.size() != 0)
						{
							directoryPath = arguments[2];

							if ((directoryEntry.fileAttributes & 0x10) == 0x10)
							{
								namePath.append(1, FILESYSTEM_FOLDER_PATH_SYMBOL).append(nameString);
								directoryPath.append(namePath);
								bool create = std::filesystem::create_directory(directoryPath);
								std::cout << namePath << std::endl;
								exitStack.push_back(disk.tellg());

								unsigned int addressDirectory = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;

								disk.seekg(addressDirectory);

								directoryEntry = ReadDirectoryEntry(&disk);
								directoryEntry = ReadDirectoryEntry(&disk);
							}
							else
							{
								std::string nameFileString = namePath;
								nameFileString.append(1, FILESYSTEM_FOLDER_PATH_SYMBOL).append(nameString).append(".").append(extensionString);

								std::string nameFile = directoryPath;
								nameFile.append(nameFileString);

								std::ofstream file;
								file.open(nameFile, std::ios_base::out | std::ios_base::binary);
								if (file.is_open())
								{
									unsigned int oldAddress = disk.tellg();
									unsigned int addressData = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;

									unsigned int clusterIndex = directoryEntry.clusterStart;
									unsigned int data = 0;
									while (data < directoryEntry.fileSize)
									{
										disk.seekg(addressData);
										for (unsigned int i = 0; i < logicalSectorPerCluster * bytesPerLogicalSector; i++)
										{
											char byte;
											disk.read(&byte, 1);
											file.write(&byte, 1);
											++data;
											if (data >= directoryEntry.fileSize - 1)
												break;
										}
										if (data >= directoryEntry.fileSize - 1)
											break;

										clusterIndex = GetClusterChain(&disk, firstAddressOfFAT + offset, clusterIndex);
										addressData = addressRegion + (clusterIndex - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
										if (clusterIndex >= 0xFF8)
											break;
									}
									file.close();
									disk.clear(std::ios_base::goodbit);
									disk.seekg(oldAddress);
								}
								file.close();
								std::cout << nameFileString << std::endl;
							}
						}
					}
				}
				//If copying from disk to img
				if (arguments[2].substr(0, 7) == "root://")
				{
					arguments[2].erase(0, 7);
					if (!std::filesystem::is_directory(arguments[1]))
					{
						std::string fileNameWithExtension = arguments[1];
						fileNameWithExtension.erase(0, fileNameWithExtension.rfind(FILESYSTEM_FOLDER_PATH_SYMBOL) + 1);
						std::string fileName = fileNameWithExtension;
						if (fileNameWithExtension.rfind(".") != std::string::npos)
							fileName.erase(fileNameWithExtension.rfind("."));
						std::string fileExtension = fileNameWithExtension;
						if (fileNameWithExtension.rfind(".") != std::string::npos)
							fileExtension.erase(0, fileNameWithExtension.rfind(".") + 1);
						else
							fileExtension = "   ";
						fileName.resize(8, 0x20);
						fileExtension.resize(3, 0x20);
						for (int i = 0; i < 8; i++)
						{
							fileName[i] = fileName.length() > i ? std::toupper(fileName[i]) : 0x20;
						}
						for (int i = 0; i < 3; i++)
						{
							fileExtension[i] = fileExtension.length() > i ? std::toupper(fileExtension[i]) : 0x20;
						}
						std::ifstream file;
						file.open(arguments[1], std::ios_base::in | std::ios_base::binary);
						if (file.is_open())
						{
							unsigned int addressData = 0;
							bool foundFolder = true;
							std::string folders = arguments[2];
							while (true)
							{
								if (folders.find("\\") == std::string::npos)
								{
									DirectoryEntry directoryEntry = ReadDirectoryEntry(&disk);
									if ((unsigned char)directoryEntry.name[0] == 0xE5 || (unsigned char)directoryEntry.name[0] == 0x00)
									{
										disk.seekg((unsigned int)disk.tellg() - 32);
										break;
									}
								}
								else
								{
									std::string folderName = folders.substr(0, folders.find("\\"));
									folderName.resize(8, 0x20);
									for (int i = 0; i < 8; i++)
									{
										folderName[i] = folderName.length() > i ? std::toupper(folderName[i]) : 0x20;
									}
									DirectoryEntry directoryEntry = ReadDirectoryEntry(&disk);
									if ((char*)directoryEntry.name == folderName)
									{
										addressData = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
										disk.seekg(addressData);
										folders.erase(0, folders.find("\\") + 1);
									}
									else if ((unsigned char)directoryEntry.name[0] == 0xE5 || (unsigned char)directoryEntry.name[0] == 0x00)
									{
										disk.seekg((unsigned int)disk.tellg() - 32);
										foundFolder = false;
										break;
									}
								}
							}
							if (foundFolder == false)
								goto skip;
							unsigned int oldAddress = disk.tellg();
							unsigned int clusterStart = GetFreeCluster(&disk, firstAddressOfFAT + offset);
							addressData = addressRegion + (clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
							if (addressData+std::filesystem::file_size(arguments[1]) >= maxFileSize)
							{
								std::cout << "Ran out of space!" << std::endl;
								goto skip;
							}
							disk.seekg(addressData);
							unsigned int clusterIndex = clusterStart;
							unsigned char symbol;
							int fileSize = 0;
							while (true)
							{
								for (int i = 0;i < bytesPerLogicalSector * logicalSectorPerCluster;i++)
								{
									if (fileSize > std::filesystem::file_size(argv[1]))
										break;
									file.read((char*)&symbol, 1);
									if (file.eof())
										break;
									disk.write((char*)&symbol, 1);
									fileSize += 1;
								}
								if (fileSize > std::filesystem::file_size(argv[1]))
									break;
								if (file.eof())
								{
									WriteClusterChain(&disk, firstAddressOfFAT + offset, clusterIndex, 0xFFF);
									break;
								}
								WriteClusterChain(&disk, firstAddressOfFAT + offset, clusterIndex, 0x001);
								unsigned int freeClusterIndex = GetFreeCluster(&disk, firstAddressOfFAT + offset);
								WriteClusterChain(&disk, firstAddressOfFAT + offset, clusterIndex, freeClusterIndex);
								clusterIndex = freeClusterIndex;
								addressData = addressRegion + (clusterIndex - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
								disk.seekg(addressData);
							}
							disk.seekg(oldAddress);
							DirectoryEntry newFile = { fileName, fileExtension, (unsigned char)0x20, (unsigned char)0x00, (unsigned char)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)clusterStart, (unsigned int)fileSize };
							WriteDirectoryTable(&disk, newFile);
						}
						else
						{
							std::cout << "File was not found." << std::endl;
						}
					skip:
						file.close();
					}
					else
					{
						std::filesystem::recursive_directory_iterator files(arguments[1]);
						for (std::filesystem::recursive_directory_iterator i(arguments[1]); std::filesystem::begin(i) != std::filesystem::end(files); i++)
						{

						}
					}
				}
			}
		}
		else if (arguments[0] == "del")
		{
			if (arguments.size() == 2)
			{
				for (int i = 0; i < arguments[1].length(); i++)
				{
					arguments[1][i] = std::toupper(arguments[1][i]);
				}

				DirectoryEntry file = FindDirectoryEntry(&disk, currentDirectory, arguments[1]);
				if (file.name[0] == 0)
				{
					std::cout << "File was not found." << std::endl;
					continue;
				}
				if ((file.fileAttributes & 0x10) == 0x10)
				{
					std::cout << arguments[1] << " is a directory." << std::endl;
					continue;
				}
				unsigned int fileAddress = (unsigned int)disk.tellg() - 32;
				disk.seekg(fileAddress);
				file.name[0] = 0xE5;
				WriteDirectoryTable(&disk, file);
			}
		}
		else if (arguments[0] == "md")
		{
			if (arguments.size() == 2)
			{
				bool canCreate = true;
				unsigned int freeSpace = 0;

				for (int i = 0; i < arguments[1].length(); i++)
				{
					arguments[1][i] = std::toupper(arguments[1][i]);
				}

				bool found = true;
				size_t firstPos = arguments[1].find('\\');

				while (firstPos != std::string::npos)
				{
					DirectoryEntry directoryEntry = FindDirectoryEntry(&disk, currentDirectory, arguments[1].substr(0, firstPos));
					if (directoryEntry.name[0] == 0)
					{
						std::cout << "Directory " << arguments[1].substr(0, firstPos) << " does not exist." << std::endl;
						std::cout << std::endl;
						found = false;
						break;
					}
					else
					{
						unsigned int addressDirectory = addressRegion + (directoryEntry.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
						disk.seekg(directoryEntry.clusterStart != 0 ? addressDirectory : rootDirectory);
					}
					arguments[1].erase(0, firstPos+1);
					firstPos = arguments[1].find('\\');
				}
				if (found == false)
					continue;

				unsigned int previousCluster = (unsigned int)disk.tellg() != rootDirectory ? ((unsigned int)disk.tellg() - addressRegion) / bytesPerLogicalSector / logicalSectorPerCluster + 2 : 0;

				while (true)
				{
					DirectoryEntry directoryEntry = ReadDirectoryEntry(&disk);

					if ((unsigned char)directoryEntry.name[0] == 0)
					{
						if (freeSpace == 0)
							freeSpace = (unsigned int)disk.tellg() - 32;
						break;
					}
					else if ((unsigned char)directoryEntry.name[0] == 0xE5)
					{
						if (freeSpace == 0)
							freeSpace = (unsigned int)disk.tellg() - 32;
						continue;
					}

					std::string nameString = directoryEntry.GetName();
					std::string extensionString = directoryEntry.GetExtension();

					if (nameString == arguments[1])
					{
						if ((directoryEntry.fileAttributes & 0x10) == 0x10)
							std::cout << "Directory already exists." << std::endl;
						else
							std::cout << "Cannot create directory, choose different name." << std::endl;
						canCreate = false;
						break;
					}
				}
				if (canCreate == true && freeSpace != 0)
				{
					unsigned int freeCluster = GetFreeCluster(&disk, firstAddressOfFAT);
					disk.seekg(freeSpace);
					DirectoryEntry newDirectory = { arguments[1], "   ", (unsigned char)0x10, (unsigned char)0x00, (unsigned char)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)freeCluster, 0x00};
					WriteDirectoryTable(&disk, newDirectory);

					WriteClusterChain(&disk, firstAddressOfFAT + offset, freeCluster, 0xFFF);

					unsigned int addressData = addressRegion + (freeCluster - 2) * logicalSectorPerCluster * bytesPerLogicalSector;

					disk.seekg(addressData);

					DirectoryEntry newCurrentDirectory = { ".", "   ", (unsigned char)0x10, (unsigned char)0x00, (unsigned char)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)freeCluster, (unsigned int)0x00 };
					DirectoryEntry newParentDirectory = { "..", "   ", (unsigned char)0x10, (unsigned char)0x00, (unsigned char)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)0x00, (unsigned short)previousCluster, (unsigned int)0x00 };
					WriteDirectoryTable(&disk, newCurrentDirectory);
					WriteDirectoryTable(&disk, newParentDirectory);
				}
				else if (freeSpace == 0)
				{
					std::cout << "Out of space!" << std::endl;
				}
			}
		}
		else if (arguments[0] == "rd")
		{
			if (arguments.size() == 2)
			{
				for (int i = 0; i < arguments[1].length(); i++)
				{
					arguments[1][i] = std::toupper(arguments[1][i]);
				}

				DirectoryEntry folder = FindDirectoryEntry(&disk, currentDirectory, arguments[1]);
				if (folder.name[0] == 0)
				{
					std::cout << "Folder was not found." << std::endl;
					continue;
				}
				unsigned int oldAddress = (unsigned int)disk.tellg()-32;
				unsigned int addressData = addressRegion + (folder.clusterStart - 2) * logicalSectorPerCluster * bytesPerLogicalSector;
				disk.seekg(addressData);
				DirectoryEntry file;
				file = ReadDirectoryEntry(&disk);
				file = ReadDirectoryEntry(&disk);
				unsigned int amountOfFiles = 0;
				while (file.name[0] != 0)
				{
					file = ReadDirectoryEntry(&disk);
					if (file.name[0] != 0 && file.name[0] != 0xE5)
						amountOfFiles += 1;
				}
				if (amountOfFiles != 0)
				{
					std::cout << "Directory is not empty." << std::endl;
					continue;
				}

				folder.name[0] = 0xE5;
				disk.seekg(oldAddress);
				WriteDirectoryTable(&disk, folder);
			}
		}
		else if (arguments[0] == "cl")
		{
			if (arguments.size() > 1)
			{
				std::string labelName = arguments[1];
				labelName.resize(8, 0x20);
				DirectoryEntry newFile = { labelName, "   ", 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
				WriteDirectoryTable(&disk, newFile);
			}
			else
			{
				std::cout << "Usage: cl <name>" << std::endl;
			}
		}
		else if (arguments[0] == "exit")
		{
			break;
		}
		else if (arguments[0] == "help")
		{
			std::cout << "Available commands:" << std::endl;
			std::cout << "help - shows this information" << std::endl;
			std::cout << "dir - show current directory" << std::endl;
			std::cout << "cd - change current directory" << std::endl;
			std::cout << "copy <from> <to> - copies file or directory to specified directory" << std::endl;
			std::cout << "                   To use the virtual disk drive, always specify root:// (see dir)" << std::endl;
			std::cout << "del <name> - delete file" << std::endl;
			std::cout << "md <name> - make directory" << std::endl;
			std::cout << "rd <name> - remove directory" << std::endl;
			std::cout << "cl <name> - creates label for a disk" << std::endl;
			std::cout << "exit - close the program" << std::endl;
		}
		else
		{
			std::cout << "Invalid command. Use \"help\" for command list." << std::endl;
		}
		std::cout << std::endl;
	}

	script.close();
	disk.close();

	return 0;
}
