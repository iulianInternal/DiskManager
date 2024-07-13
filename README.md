# DiskManager

A program that allows access to read/write raw image files of diskette.  
Currently supports only FAT12 file system.

Compiling the program
---------------------
For Windows you need compatible C++17 and higher compiler.  
List of C++ compilers: 
|Supported|Name|
|---------|----|
|&check;|[msvc++](https://visualstudio.microsoft.com/)|
|not tested|[g++](https://gcc.gnu.org/)|
|not tested|[clang](https://clang.llvm.org/)|
|&cross;|[mingw-w64](https://www.mingw-w64.org/)|

You also need a [**CMake**](https://cmake.org/) to build a project.

Clone repository first, and then use CMake to build project.

    git clone https://github.com/iulianInternal/DiskManager.git
    cmake .
    
Then compile project with installed compatible C++17 compiler.  
Or you can use CMake to build it for you:

    cmake --build .

License
-------
This program is licenced under MIT licence. See in file LICENCE.
