# LLZZ_FILECONVERTER
Stupid fast file converter for all platforms, but mainly intended for IBM mainframes.
Converts given LLZZ file into a format that can be read by the Hessling editor and can convert it back to LLZZ. It can also convert regular fixed length files into Hesslings file format.

# Installation
Go to release and chose the version you want to use, note that the latest one is safer and better than the one before
compile it with your c-compiler of choice, but note that compile it with pthread option, like this: gcc -pthread main.c
After compiling enjoy it!

# Usage
Form versions upper than 1.2 the program should be called by a script, because it takes the path for input output from enviroment variables of the shellscript.
