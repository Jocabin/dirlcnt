#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define ArraySize(arr) sizeof(arr) / sizeof(arr[0])
#define ArrayEnd(arr) arr[ArraySize(arr) - 1]

int main(int argc, char *argv[])
{
	char *DirPath = "./";
	char *FileExtensions[20] = {0};
	int TotalLinesCount = 0;

	// get dir name by cmd arg, if not arg defined, use current dir
	if (argv[1] != NULL)
		DirPath = argv[1];

	fprintf(stdout, "Reading directory => %s\n", DirPath);

	// get all files extensions user want to count
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

	// get all files in dir
	DIR *Directory;
	struct dirent *DirEntity;
	Directory = opendir(DirPath);

	if (Directory == NULL)
	{
		fprintf(stderr, "Directory is NULL\n");
		exit(1);
	}

	while ((DirEntity = readdir(Directory)) != NULL)
	{
		if (DirEntity->d_type == DT_REG)
		{
			char *CurrentFilePath = realpath(DirEntity->d_name, NULL);
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

					// read single file lines count
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
			// TODO sub dirs
			fprintf(stdout, "%s directory not implemented\n", DirEntity->d_name);
		}
	}
	closedir(Directory);

	// output to the screen
	fprintf(stdout, "\nTotal Lines count : %d\n", TotalLinesCount);

	return 0;
}