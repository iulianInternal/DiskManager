# DiskManager

A program that allows access to read/write raw image files of diskette.  
Currently supports only FAT12 file system.

Compiling the program
---------------------
You need a compatible C++17 and higher compiler.  
List of C++ compilers: 
|Supported|Name|
|---------|----|
|&check;|[msvc++](https://visualstudio.microsoft.com/)|
|&cross;|[g++](https://gcc.gnu.org/)|
|&cross;|[clang](https://clang.llvm.org/)|
|&cross;|[mingw-w64](https://www.mingw-w64.org/)|

You also need a [**CMake**](https://cmake.org/) to build and compile a project.

Clone repository first, and then use CMake to build project.

    git clone https://github.com/iulianInternal/DiskManager.git
    cmake .
    
Then compile project with installed compatible C++17 compiler:

    cmake --build .

License
-------
This program is licenced under MIT licence. See in file LICENCE.
