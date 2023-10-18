#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define ArraySize(arr) sizeof(arr) / sizeof(arr[0])

int main(int argc, char *argv[])
{
	char *DirPath = "./";
	char *FileExtensions[10] = {0};

	// get dir name by cmd arg, if not arg defined, use current dir
	if (argv[1] != NULL)
	{
		DirPath = argv[1];
	}

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
	}

	// get all files in dir
	DIR *Directory;
	struct dirent *DirEntity;
	Directory = opendir(DirPath);

	if (Directory == NULL)
		exit(1);

	while ((DirEntity = readdir(Directory)) != NULL)
	{
		printf("%s\n", DirEntity->d_name);
		printf("%llu\n", DirEntity->d_ino);
		printf("%d\n", DirEntity->d_reclen);
		printf("%d\n\n\n", DirEntity->d_type);
	}
	closedir(Directory);

	// read single file lines count
	// output to the screen
	return 0;
}