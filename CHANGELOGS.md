# Changelogs

This project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

-----------------------------------------------------------------------------------------------------
## DiskManager Unreleased - N/A Commit: N/A

### New features:
- Added command "rd".
- Added constructors to DirectoryEntry.
- Added support to copy folder from disk to img.
- When you specify path to copy to that does not exist, it will create it, if it is possible.
- Added functions: copyFileToIMG; createDirectory.

### Changes:
- DirectoryEntry::name and DirectoryEntry::extension are unsigned char now, previously they were std::string.

### Bug fixes:
- Command "dir" will not show deleted folders.

-----------------------------------------------------------------------------------------------------
## DiskManager 0.1.1 - 2024-09-06 Commit: 3cce681a238b66544acf255d07e15a47cf68544f
Some bug fixes.

### New features:
- Command "dir" is now supporting first argument.
- You can specify path in commands "md", "cd" and "dir".
- Added in class "DirectoryEntry" new functions `GetName()` and `GetExtension()` to retrieve name/extension in normalized way.

### Bug fixes:
- Command "md" is now not limited to 8 symbols and will not be able to include \ in name.
- When using command "md", directory ".." now links to the previous directory and not to the root.
- Command "cd" accepts lowercase now.
- When using command "cd", it outputs a normal name (by viewing directory with command "dir").
- Commands are now commands (i.e. you cannot write "directory" to view directory anymore.)
- Specifying not existing image file will not crash the program.

-----------------------------------------------------------------------------------------------------
## DiskManager 0.1.0 - 2024-08-17 Commit: 7690226da651b047aeaf53ad8bdb2d45172c86f7
This is first initial release. Some bugs may be in here.

### Available features:
- Read any .img diskette (only FAT12 is supported).
- Create disks (currently 160kb and 360kb are supported and only FAT12 is supported).
- Automate disk creation with script.
- Execute commands (in DOS style).

### Available commands:
- help
- dir
- cd
- copy
- md
- cl
- exit
