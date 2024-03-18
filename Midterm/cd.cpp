#pragma once
#include "cd.h"

int main() {
	TCHAR curDir[MAXPATH];
	GetCurrentDirectory(MAXPATH, curDir);
	printf("%ls\n", curDir);
	WCHAR* cli = GetCommandLine();
	while (NULL != *cli && *cli != L' ') cli++;
	while (*cli == ' ') cli++; // Remove any extra leading spaces
	SetCurrentDirectoryW(cli);
	GetCurrentDirectory(MAXPATH, curDir);
	printf("%ls\n", curDir);
}
