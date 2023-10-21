#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define ArraySize(arr) sizeof(arr) / sizeof(arr[0])
#define true 1
#define false 0

// TODO change this to dynamic
char *FileExtensions[3] = {0};
int TotalLinesCount = 0;
const char *BasePath = NULL;

int IsFileIgnored(char *FilePath, char *GIFilePath);
void ReadDirectory(char *DirPath, char *GIFilePath, int *FileCount);

int main(int argc, char **argv)
{
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
	return false;
}

void ReadDirectory(char *DirPath, char *GIFilePath, int *FileCount)
{

	DIR *Directory;
	struct dirent *DirEntity;

	if (chdir(DirPath))
	{
		fprintf(stderr, "Can't move to %s directory\n", DirPath);
		exit(1);
	}

	Directory = opendir(DirPath);

	if (Directory == NULL)
	{
		fprintf(stderr, "Directory %s is NULL\n", DirPath);
		exit(1);
	}

	while ((DirEntity = readdir(Directory)) != NULL)
	{
		const char *CurrentEntry = DirEntity->d_name;

		if (strcmp(CurrentEntry, ".") == 0 || strcmp(CurrentEntry, "..") == 0)
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

			for (int i = 0; i < ArraySize(FileExtensions); ++i)
			{
				if (FileExtensions[i] != NULL && strcmp(FileExtensions[i], Ext) == 0)
				{
					FILE *CurrentFile = fopen(CurrentFilePath, "r");

					if (CurrentFile == NULL)
					{
						fprintf(stderr, "CurrentFile is NULL; File path is : %s\n", CurrentFilePath);
						exit(1);
					}

					fprintf(stdout, "Reading %s => ", CurrentFilePath);

					int LineCount = 1;
					char CurrentChar;

					for (CurrentChar = getc(CurrentFile); CurrentChar != EOF; CurrentChar = getc(CurrentFile))
						if (CurrentChar == '\n')
							LineCount += 1;

					fprintf(stdout, "%d lines\n", LineCount);

					TotalLinesCount += LineCount;
					*FileCount += 1;
					fclose(CurrentFile);
				}
			}
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

int IsFileIgnored(char *FilePath, char *GIFilePath)
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