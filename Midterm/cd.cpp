#include "cd.h"

int main() {
	TCHAR curDir[MAXPATH];
	GetCurrentDirectory(MAXPATH, curDir);
	printf("Current Directory %ls\n\n", curDir);
	WCHAR* cli = GetCommandLine();
	while (NULL != *cli && *cli != L' ') cli++; // Remove filename
	while (*cli == ' ') cli++; // Remove any extra leading spaces
	if(!SetCurrentDirectoryW(cli)) { // Check if the path was valid
		printf("Invalid path given exiting...\n");
		return -1;
	}
	GetCurrentDirectory(MAXPATH, curDir);
	printf("New Directory %ls\n", curDir);
}
