#include "dir.h"

int main() {
	TCHAR dirToPrint[MAXPATH] = { 0 }, newDir[MAXPATH] = {0};
	WCHAR* cli = GetCommandLine();

	DWORD singleSearch = 1, i = 0, args = FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN; // MAXDWORD will match all in a filter

	WCHAR searchFor[MAXPATH] = { 0 };

	void (*printingMethod)(LPWIN32_FIND_DATAW, WCHAR*) = &defaultPrintFile; // Set printing file to default printing method
	DWORD(*comp)(LPWIN32_FIND_DATAW, DWORD) = &doesNotHaveAttribute; // Set default attribute checking to does not have so we can ignore system and hidden objects

	//Find first attribute
	while (NULL != *cli && *cli != L' ') cli++;

	while (NULL != *cli) { // Go till we have no more attributes
		while (*cli == ' ') cli++; // Remove the current spaces
		if (NULL == *cli) break; // Ensure trailing spaces don't get us in trouble

		if (*cli == L'/') { // If the next item starts with a / treat it as a switch
			cli++;
			switch (*cli) {
			case L'a':
				cli++;
				if (parseAttributeArg(cli, &args)) {
					comp = &hasAttribute;
				}
				break;
			case L's':
				singleSearch = 0; // Tell control to go through sub directories
				cli++;
				cli++;
				i = 0;
				while (NULL != *cli  && *cli != L' ' && *cli != '\\' ) { searchFor[i] = *cli; cli++; i++; }
				break;
			case L'q':
				printingMethod = &printFileOwnership;
				break;
			}
		}
		else { // Otherwise it is a directory to check
			i = 0;
			while (*cli != ' ' && NULL != *cli) { newDir[i] = *cli; cli++; i++; }
			SetCurrentDirectoryW(newDir);
		}
		while (NULL != *cli && *cli != L' ') cli++; // Get to next point
	}
	
	GetCurrentDirectory(MAXPATH, dirToPrint); // Set the starting directory

	if(singleSearch) printFilesByAttributes(dirToPrint, args, printingMethod, comp);
	else searchPrinting(dirToPrint, searchFor, args, printingMethod, comp);
}

void searchPrinting (TCHAR* startingDir, const WCHAR* filename, DWORD attrs, void (*printingMethod)(LPWIN32_FIND_DATAW, WCHAR*), DWORD(*comparison)(LPWIN32_FIND_DATAW, DWORD attr)) {
	if (wcslen(filename) == 0) filename = L"*";
	size_t lenOfStart = wcslen(startingDir) + 2, current = 0, foundMatching = 0;
	TCHAR* baseDir = (TCHAR*)malloc(sizeof(TCHAR) * lenOfStart);
	HANDLE searcher;
	TCHAR nextDir[MAXPATH];
	LPWIN32_FIND_DATAW data;
	if (NULL == baseDir) return;

	wcscpy_s(baseDir, lenOfStart, startingDir);
	wcscat_s(startingDir, MAXPATH, L"\\*");
	data = (LPWIN32_FIND_DATAW)malloc(sizeof(WIN32_FIND_DATAW));
	if (NULL == data) return;
		
	searcher = FindFirstFileW(startingDir, data);

	if (comparison(data, attrs) && regexCompareFilenames(data->cFileName, filename)) {
		if (!foundMatching) {
			foundMatching = 1;
			printf("\n");
			printf("Directory of %ls\n\n", baseDir); // Need to make sure we only print if something is found
		}
		printingMethod(data, baseDir);
	}
	while (FindNextFile(searcher, data)) {
		if (comparison(data, attrs) && regexCompareFilenames(data->cFileName, filename)) {
			if (!foundMatching) {
				foundMatching = 1;
				printf("\n");
				printf("Directory of %ls\n\n", baseDir); // Need to make sure we only print if something is found
			}
			printingMethod(data, baseDir);
		}
	}

	wcscat_s(baseDir, lenOfStart, L"\\");
	searcher = FindFirstFileW(startingDir, data);
	FindNextFile(searcher, data);
	while (FindNextFile(searcher, data)) {
		if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { 
			wcscpy_s(nextDir, MAXPATH, baseDir);
			wcscat_s(nextDir, MAXPATH, data->cFileName);
			searchPrinting(nextDir, filename, attrs, printingMethod, comparison); 
		}
	}
	free(data);
}

DWORD regexCompareFilenames(WCHAR* filename, const WCHAR* regex) {
	if (NULL == *regex && NULL != *filename) return 0;
	else if (NULL == *regex && NULL == *filename) return 1;

	while (NULL != *filename) {
		if (*regex != '*' && *regex != '?') { 
			if (*filename != *regex) return 0; // If the regex char is not wild then return false if the filename does not match
			regex++;
		}
		else if (*regex == '*') { // If the char is * then attempt various consumption methods before continuing (potentially without consuming regex)
			if (*(regex + 1) == *filename && regexCompareFilenames(filename + 1, regex + 2)) return 1; // Attempt to consume current wild and next character if it matches next filename
			else if ((*(regex + 1) == '?' || *(regex + 1) == '*') && regexCompareFilenames(filename + 1, regex + 2)) return 1; // Attempt to consume current wild and next
			else if (*(regex + 1) == '*' && regexCompareFilenames(filename + 1, regex + 1)) return 1; // Consume current * but not the next
		}
		else { // If char is ? then consume it and continue
			regex++;
		}
		filename++;
		if (NULL == *regex && NULL != *filename) return 0; // If regex is empty but filename is not then False
	}
	while (*regex != NULL) { // Ensure remaining chars are * since they match the empty string
		if (*regex != '*') return 0; // ? must match at least one char, but * can match the empty string
		regex++;
	}
	return 1;
}

DWORD parseAttributeArg(WCHAR* attributes, DWORD* returnAttrs) {
	DWORD attrs = 0, val = 1;
	int i = 0;
	while(attributes[i] != L' ' && NULL != attributes[i]) {
		switch (attributes[i]) {
		case L'd':
			attrs = attrs | FILE_ATTRIBUTE_DIRECTORY;
			break;
		case L'h':
			attrs = attrs | FILE_ATTRIBUTE_HIDDEN;
			break;
		case L's':
			attrs = attrs | FILE_ATTRIBUTE_SYSTEM;
			break;
		case L'r':
			attrs = attrs | FILE_ATTRIBUTE_READONLY;
			break;
		case L'l':
			attrs = attrs | FILE_ATTRIBUTE_REPARSE_POINT;
			break;
		case L'a':
			attrs = attrs | FILE_ATTRIBUTE_ARCHIVE;
			break;
		case L'i':
			attrs = attrs | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
			break;
		case L'-': // If - is given then we want anything without the supplied attributes
			val = 0;
		}
		i++;
	}
	if (!attrs) attrs = MAXDWORD;
	*returnAttrs = attrs;
	return val;
}

DWORD hasAttribute(LPWIN32_FIND_DATAW data, DWORD attrs) {
	return data->dwFileAttributes & attrs;
}

DWORD doesNotHaveAttribute(LPWIN32_FIND_DATAW data, DWORD attrs) {
	DWORD dataAttr = data->dwFileAttributes;
	while (dataAttr && attrs) {
		if (dataAttr % 2 == 1 && attrs % 2 == 1) return 0;
		dataAttr = dataAttr >> 1;
		attrs = attrs >> 1;
	}
	return 1;
}

void defaultPrintFile(LPWIN32_FIND_DATAW fileInfo, WCHAR* baseDir) {
	LPSYSTEMTIME displayable = (LPSYSTEMTIME)malloc(sizeof(SYSTEMTIME));
	if (NULL == displayable) return;
	FileTimeToSystemTime(&(fileInfo->ftLastWriteTime), displayable);

	int isDir = fileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

	DWORD64 bytes = (fileInfo->nFileSizeHigh * MAXDWORD + 1) + fileInfo->nFileSizeLow;

	
	const char *aOrP;
	if (displayable->wHour >= 12) aOrP = "PM";
	else aOrP = "AM";

	WORD hour = displayable->wHour;
	if (hour > 12) hour -= 12;

	printf(
		"%02d/%02d/%04d %02d:%02d %s\t",
		displayable->wMonth,
		displayable->wDay,
		displayable->wYear,
		hour,
		displayable->wMinute,
		aOrP
	);

	if (isDir) {
		printf("<DIR>\t");
	}
	else {
		printf("     \t"); // 5 spaces for the 5 missing chars of <DIR>
	}

	if (!isDir) printf("%10llu ", bytes);
	else printf("           ");
	printf("%ls\n", fileInfo->cFileName);

	free(displayable);
}

void printFileOwnership(LPWIN32_FIND_DATAW fileInfo, WCHAR* baseDir) { 
	PSID username;
	char name[USERMAX], domain[USERMAX];
	WCHAR fullPath[MAXPATH] = { 0 };
	DWORD size = USERMAX, size2 = USERMAX;
	LPFILETIME localtime = (LPFILETIME)malloc(sizeof(FILETIME));
	LPSYSTEMTIME displayable = (LPSYSTEMTIME)malloc(sizeof(SYSTEMTIME));
	SID_NAME_USE ignored;

	if (NULL == displayable) return;
	FileTimeToLocalFileTime(&(fileInfo->ftLastWriteTime), localtime);
	FileTimeToSystemTime(localtime, displayable);

	int isDir = fileInfo->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
	
	DWORD64 bytes = (fileInfo->nFileSizeHigh * MAXDWORD + 1) + fileInfo->nFileSizeLow;
	if (NULL != baseDir) wcscpy_s(fullPath, MAXPATH, baseDir);
	wcscat_s(fullPath, MAXPATH, fileInfo->cFileName);

	GetNamedSecurityInfo(fullPath, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &username, NULL, NULL, NULL, NULL);
	LookupAccountSidA(NULL, username, name, &size, domain, &size2, &ignored);

	const char* aOrP;
	if (displayable->wHour >= 12) aOrP = "PM";
	else aOrP = "AM";

	WORD hour = displayable->wHour;
	if (hour > 12) hour -= 12;

	printf(
		"%02d/%02d/%04d %02d:%02d %s\t",
		displayable->wMonth,
		displayable->wDay,
		displayable->wYear,
		hour,
		displayable->wMinute,
		aOrP
	);

	if (isDir) {
		printf("<DIR>\t");
	}
	else {
		printf("     \t"); // 5 spaces for the 5 missing chars of <DIR>
	}

	if (!isDir) printf("%10llu ", bytes);
	else printf("           ");
	printf("%s\\%s ", domain, name);
	printf("%ls\n", fileInfo->cFileName);

	free(displayable);
}

void printFilesByAttributes(TCHAR* directory, DWORD attrs, void (*printingMethod)(LPWIN32_FIND_DATAW, WCHAR*), DWORD (*comparison)(LPWIN32_FIND_DATAW, DWORD attr)) {
	/*
		NOTE: dir /a is an and function meaning we if ask for /a:hd (hidden and directory) we print all directories which are also hidden
		not all directories and all hidden objects
	*/
	size_t lenOfStart = wcslen(directory) + 2;
	TCHAR* baseDir = (TCHAR*)malloc(sizeof(TCHAR) * lenOfStart);
	if (NULL == baseDir) return;

	wcscpy_s(baseDir, lenOfStart, directory);
	wcscat_s(baseDir, lenOfStart, L"\\");
	printf("Directory of %ls\n\n", directory);
	wcscat_s(directory, MAXPATH, L"\\*");
	LPWIN32_FIND_DATAW data = (LPWIN32_FIND_DATAW)malloc(sizeof(WIN32_FIND_DATAW));
	if (NULL == data) return;

	HANDLE searcher = FindFirstFileW(directory, data);
	if (comparison(data, attrs)) printingMethod(data, baseDir);
	while (FindNextFile(searcher, data)) {
		if (comparison(data, attrs)) printingMethod(data, baseDir);
	}

	free(data);
}

/*
void printAllFiles(TCHAR* directory) {
	LPWIN32_FIND_DATAW data = (LPWIN32_FIND_DATAW)malloc(sizeof(WIN32_FIND_DATAW));
	HANDLE searcher = FindFirstFileW(directory, data);

	defaultPrintFile(data);
	while (FindNextFile(searcher, data)) {
		defaultPrintFile(data);
	}

	free(data);
}

void printFiles(TCHAR* directory, void (*printer)(LPWIN32_FIND_DATAW)) {
	LPWIN32_FIND_DATAW data = (LPWIN32_FIND_DATAW) malloc(sizeof(WIN32_FIND_DATAW));
	HANDLE searcher = FindFirstFileW(directory, data);

	if(!(data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) & !(data->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) printer(data);
	while (FindNextFile(searcher, data)) {
		if (!(data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) & !(data->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) printer(data);
	}

	free(data);
}
*/