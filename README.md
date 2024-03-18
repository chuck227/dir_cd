# How to Compile
## Microsoft Visual Studio
1. Open Midterm.sln in Microsoft Visual Studio.
2. Hit CTRL-b or CTRL-B to build the code

## Manually 
1. Navigate to the folder ./Midterm
2. Using g++ run g++ cd.cpp -o cd.exe 
3. Using g++ run g++ dir.cpp -o dir.exe

# Use
## cd
CD is a fairly simple command which will:
1. Print the current directory
2. Change the current directory to the user supplied directory
3. Print the new current directory

The syntax for this command is cd.exe [New Path]

## dir
dir is used to print the contents of a directory it has the following syntax:
dir.exe [directory to print] [/s <filename>] [/a{adshrli-}] [/q]

dir will show the date and time last modified, and the name of filesystem objects. For files, the size of the file will also be printed.

|Switch|Description|
|---|---|
|/s|Prints recursive through all subdirectories. If a filename is supplied then only files who match the filename will be printed. \* and ? can be used as wildcard values in filenames.|
|/a|Prints files based on their attributes. If no attributes are supplied then all files will be printed. Otherwise, the following letters can be used:
- a: archive
- d: directory
- s: system file
- h: hidden
- r: readonly
- l: Reparse Point
- i: content not indexed
If \- is used provided then files without the given attributes will be printed|
|/q|Prints the name of the file owner before printing the 