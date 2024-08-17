# DiskManager

A program that allows to read/write raw image files of a diskette.  
Currently supports only FAT12 file system.

How to use
----------
&emsp;`Usage: DiskManager.exe <disk> [options]`

&emsp;disk - a relative or absolute path of a file to read/edit.  
  
Options:  
  
&emsp;`-c:number` &mdash; create a new diskette image.  
&emsp; &emsp;&emsp;&emsp;&emsp;&emsp;**NOTE!** It can erase all data in specified file. So it can recreate file.  
&emsp; &emsp;&emsp;&emsp;&emsp;&emsp;Accepts only 160kb and 360kb diskette sizes.  
  
&emsp;`-s file` &nbsp;&nbsp; &mdash; a relative or absolute path of a script to run program commands.  
&emsp; &emsp;&emsp;&emsp;&emsp;&emsp;It is useful if you want to automate diskette creation.  
  
You can get this information by just running the application without parameters.

Compiling the program
---------------------
You need a compatible C++17 and higher compiler.  
List of well known C++ compilers: 
|Supported|Name|
|---------|----|
|&check;|[msvc++](https://visualstudio.microsoft.com/)|
|&check;|[g++](https://gcc.gnu.org/)|
|&cross;|[clang](https://clang.llvm.org/)|
|&check;|[mingw-w64](https://www.mingw-w64.org/)|

You also need a [**CMake**](https://cmake.org/) to build and compile a project.

Clone repository first, and then use CMake to build project.

    git clone https://github.com/iulianInternal/DiskManager.git
    cmake .
    
Then compile project with installed compatible C++17 compiler:

    cmake --build .

Tested on Windows and on Linux (Debian).  
If you find bugs in untested OS, open an issue.

License
-------
This program is licenced under MIT licence. See in file `LICENCE`.
