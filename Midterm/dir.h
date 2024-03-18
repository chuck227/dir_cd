#pragma once
#define TRUE 1
#define FALSE 0
#define MAXPATH 2048
#define USERMAX 2048

/*
	Need to include: 
					No flags # Done
					/a: Prints based on attributes, if none given print all # Testing Needed
					/s: List all occruences of provided name within cur and sub directories # Done
					/q: Display file ownership information # Done

	Also need to handle basic CLI argument parsing 
*/

#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <aclapi.h>

// Printing Stuff
void defaultPrintFile(LPWIN32_FIND_DATAW fileInfo, WCHAR* baseDir);
void printFilesByAttributes(TCHAR* directory, DWORD attrs, void (*printingMethod)(LPWIN32_FIND_DATAW, WCHAR*));
// void printFiles(TCHAR* directory, void (*printer)(LPWIN32_FIND_DATAW));
// void printAllFiles(TCHAR* directory);
void printFileOwnership(LPWIN32_FIND_DATAW fileInfo, WCHAR* baseDir);
void searchPrinting(TCHAR* startingDir, const WCHAR* filename, DWORD attrs, void (*printingMethod)(LPWIN32_FIND_DATAW, WCHAR*));

// CLI Stuff
DWORD parseAttributeArg(WCHAR* attributes);

// Helper Functions
DWORD regexCompareFilenames(WCHAR* filename, const WCHAR* regex);