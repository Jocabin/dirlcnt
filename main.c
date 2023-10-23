#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define ArraySize(arr) sizeof(arr) / sizeof(arr[0])
#define true 1
#define false 0

char *FileExtensions[10] = {0};
int TotalLinesCount = 0;
const char *BasePath = NULL;

const int IsFileIgnored(char *FilePath, char *GIFilePath);
void ReadDirectory(char *DirPath, char *GIFilePath, int *FileCount);
void ReadFileLineCount(char *FilePath, int *FileCount);

int main(int argc, char **argv)
{
	if (strcmp(argv[1], "help") == 0)
	{
		printf("HOW TO USE DIRLCNT\n\n");
		printf("ARG_1 => Working Path. If no arguments povided, it will use the current directory\n");
		printf("ARG_2 => Extensions list to filter. Comma separated values. Maximum of 10 values. Put \"all\" to keep count all files.\n");
		printf("ARG_3 => .gitignore file path. If no one provided, no files will be ignored\n\n");
		return EXIT_SUCCESS;
	}

	char *WDPath = "./";
	int FileCount = 0;

	if (argv[1] != NULL)
		WDPath = argv[1];

	WDPath = realpath(WDPath, NULL);
	BasePath = realpath(WDPath, NULL);

	if (argv[2] != NULL)
	{
		char *Ext;
		int i = 0;

		while ((Ext = strsep(&argv[2], ",")) != NULL)
		{
			if (FileExtensions[i] == NULL)
				FileExtensions[i] = Ext;
			++i;
		}

		fprintf(stdout, "Extensions allowed : ");
		for (int idx = 0; idx < ArraySize(FileExtensions); ++idx)
			if (FileExtensions[idx] != NULL)
				fprintf(stdout, "[%s] ", FileExtensions[idx]);
		fprintf(stdout, "\n\n");
	}

	char *GitIgnoreFilePath = realpath(argv[3], NULL);
	ReadDirectory(WDPath, GitIgnoreFilePath, &FileCount);

	fprintf(stdout, "\nTotal Lines count : %d\n", TotalLinesCount);
	fprintf(stdout, "Files readed : %d\n", FileCount);

	return EXIT_SUCCESS;
}

void ReadDirectory(char *DirPath, char *GIFilePath, int *FileCount)
{

	DIR *Directory;
	const struct dirent *DirEntity;

	if (chdir(DirPath))
	{
		perror("Can't move to directory\n");
		exit(1);
	}

	Directory = opendir(DirPath);

	if (Directory == NULL)
	{
		perror("Directory %s is NULL\n");
		exit(1);
	}

	while ((DirEntity = readdir(Directory)) != NULL)
	{
		const char *CurrentEntry = DirEntity->d_name;

		if (strcmp(CurrentEntry, ".") == 0 || strcmp(CurrentEntry, "..") == 0 || strcmp(CurrentEntry, ".git") == 0)
			continue;

		char *CurrentFilePath = realpath(CurrentEntry, NULL);

		if (IsFileIgnored(CurrentFilePath, GIFilePath) == true)
			continue;

		switch (DirEntity->d_type)
		{
		case DT_REG:
		{
			const char *dot = strrchr(CurrentFilePath, '.');

			if (!dot || dot == NULL || dot == CurrentFilePath)
				continue;

			const char *Ext = dot + 1;

			if (strcmp(FileExtensions[0], "all") == 0)
				ReadFileLineCount(CurrentFilePath, FileCount);
			else
				for (int i = 0; i < ArraySize(FileExtensions); ++i)
					if (FileExtensions[i] != NULL && strcmp(FileExtensions[i], Ext) == 0)
						ReadFileLineCount(CurrentFilePath, FileCount);
			break;
		}
		case DT_DIR:
		{
			if (GIFilePath != NULL || IsFileIgnored(realpath(DirEntity->d_name, NULL), GIFilePath) == false)
				ReadDirectory(realpath(DirEntity->d_name, NULL), GIFilePath, FileCount);
			break;
		}
		default:
			break;
		}
	}
	chdir("..");
	closedir(Directory);
}

void ReadFileLineCount(char *FilePath, int *FileCount)
{
	FILE *CurrentFile = fopen(FilePath, "r");

	if (CurrentFile == NULL)
	{
		perror("Can't read file\n");
		exit(1);
	}

	fprintf(stdout, "Reading %s => ", FilePath);

	int LineCount = 1;
	char CurrentChar;

	char *CurrentLine = NULL;
	size_t LineLength = 0;
	ssize_t Read;

	while ((Read = getline(&CurrentLine, &LineLength, CurrentFile)) != -1)
		LineCount += 1;

	if (CurrentLine != NULL)
		free(CurrentLine);

	fprintf(stdout, "%d lines\n", LineCount);

	TotalLinesCount += LineCount;
	*FileCount += 1;
	fclose(CurrentFile);
}

const int IsFileIgnored(char *FilePath, char *GIFilePath)
{
	FILE *GIFile = fopen(GIFilePath, "r");

	if (GIFile == NULL)
		return false;

	char *CurrentGILine = NULL;
	size_t LineLength = 0;
	ssize_t Read;

	while ((Read = getline(&CurrentGILine, &LineLength, GIFile)) != -1)
	{
		if (CurrentGILine != NULL)
		{
			char *PathToIgnore = strdup(BasePath);

			// Remove LF char
			CurrentGILine[Read - 1] = '\0';

			// Add '/' to concat for a valid path
			if (CurrentGILine[0] != '/')
				PathToIgnore = strcat(PathToIgnore, "/");
			PathToIgnore = strcat(PathToIgnore, CurrentGILine);

			if (strcmp(PathToIgnore, FilePath) == 0)
			{
				fclose(GIFile);
				return true;
			}
		}
	}

	if (CurrentGILine != NULL)
		free(CurrentGILine);

	fclose(GIFile);
	return false;
}
