#include "cd.h"

int main() {
	TCHAR curDir[MAXPATH];
	GetCurrentDirectory(MAXPATH, curDir);
	printf("Current Directory %ls\n\n", curDir);
	WCHAR* cli = GetCommandLine();
	while (NULL != *cli && *cli != L' ') cli++;
	while (*cli == ' ') cli++; // Remove any extra leading spaces
	if(!SetCurrentDirectoryW(cli)) {
		printf("Invalid path given exitting...\n");
		return -1;
	}
	GetCurrentDirectory(MAXPATH, curDir);
	printf("New Directory %ls\n", curDir);
}
