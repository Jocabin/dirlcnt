#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#define ArraySize(arr) sizeof(arr) / sizeof(arr[0])
#define ArrayEnd(arr) arr[ArraySize(arr) - 1]

char *FileExtensions[20] = {0};
int TotalLinesCount = 0;

void ReadDirectory(char *DirPath)
{
	DIR *Directory;
	struct dirent *DirEntity;

	if (chdir(realpath(DirPath, NULL)))
	{
		fprintf(stderr, "Can't chande to %s directory\n", DirPath);
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
		char *CurrentFilePath = realpath(DirEntity->d_name, NULL);

		if (DirEntity->d_type == DT_REG)
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

					fclose(CurrentFile);
				}
			}
		}
		else if (DirEntity->d_type == DT_DIR)
		{
			if (strcmp(DirEntity->d_name, ".") == 0 || strcmp(DirEntity->d_name, "..") == 0)
				continue;
			// TODO prendre en compte gitignore
			ReadDirectory(realpath(DirEntity->d_name, NULL));
		}
	}
	chdir("..");
	closedir(Directory);
}

int main(int argc, char *argv[])
{
	char *DirPath = "./";

	if (argv[1] != NULL)
		DirPath = argv[1];

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
		{
			if (FileExtensions[idx] != NULL)
				fprintf(stdout, "[%s] ", FileExtensions[idx]);
		}
		fprintf(stdout, "\n\n");
	}

	ReadDirectory(DirPath);

	fprintf(stdout, "\nTotal Lines count : %d\n", TotalLinesCount);
	return 0;
}