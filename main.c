#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "base_layer.h"

string **file_extensions_arr = null;
string **ignored_paths_arr = null;
unsigned int total_line_count = 0;
unsigned int file_count = 0;

bool is_file_ignored(string *file_path) {
    if (ignored_paths_arr == null)
        return false;

    for (size_t i = 0; i < array_size(ignored_paths_arr); ++i) {
        if (str_cmp(ignored_paths_arr[i], file_path) == 0) {
            return true;
        }
    }
    return false;
}

void read_file_line_count(string *file_path) {
    FILE *current_file_p = fopen(file_path->str, "r");

    if (current_file_p == null) {
        perror("Can't read file\n");
        exit(1);
    }

    unsigned int line_count = 0;

    string *current_line = null;
    usize line_len = 0;
    ssize read;

    while ((read = getline(&current_line->str, &line_len, current_file_p)) != -1)
        line_count += 1;

    total_line_count += line_count;
    file_count += 1;

    fclose(current_file_p);
    str_free(current_line);
}

void read_directory(string *dir_path) {
    DIR *directory;
    const struct dirent *dir_entity;

    if (chdir(dir_path->str)) {
        perror("Can't move to directory\n");
        exit(1);
    }

    directory = opendir(dir_path->str);

    if (directory == null) {
        perror("Directory %s is NULL\n");
        exit(1);
    }

    string *current_file_path = str_create("");

    while ((dir_entity = readdir(directory)) != null) {
        string *current_entry = str_create((char *)dir_entity->d_name);

        if (strcmp(current_entry->str, ".") == 0 ||
            strcmp(current_entry->str, "..") == 0 ||
            strcmp(current_entry->str, ".git") == 0) {
            continue;
        }

        str_set(current_file_path, realpath(current_entry->str, null));
        str_free(current_entry);

        if (is_file_ignored(current_file_path) == true) {
            continue;
        }

        switch (dir_entity->d_type) {
            case DT_REG: {
                string *dot = str_create(strrchr(current_file_path->str, '.'));

                if (dot == null || dot == current_file_path) {
                    continue;
                }

                string *file_ext = dot + 1;
                str_free(dot);

                if (strcmp(file_extensions_arr[0]->str, "all") == 0) {
                    read_file_line_count(current_file_path);
                } else {
                    for (usize i = 0; i < array_size(file_extensions_arr); ++i)
                        if (file_extensions_arr[i] != null &&
                            str_cmp(file_extensions_arr[i], file_ext) == true) {
                            str_free(file_ext);
                            read_file_line_count(current_file_path);
                        }
                }
                break;
            }

            case DT_DIR: {
                if (is_file_ignored(current_file_path) == false)
                    read_directory(current_file_path);

                break;
            }

            default:
                break;
        }

        str_free(current_file_path);
    }

    chdir("..");
    closedir(directory);
}

/* MAIN ENTRY POINT */
int main(int argc, char **argv) {
    string *working_dir_path = str_create("./");
    file_extensions_arr = malloc(sizeof(string *));

    if (file_extensions_arr == null) {
        fprintf(stderr, "Error: Can't allocate memory for extensions list\n");
        exit(1);
    }

    file_extensions_arr[0] = str_create("all");

    // Help command or custom path
    if (argc >= 2) {
        if (strcmp(argv[1], "help") == 0) {
            printf("HOW TO USE DIRLCNT\n\n");

            printf(
                "ARG_1 => Working Path. If no arguments povided, it will use "
                "the "
                "current directory\n");

            printf(
                "ARG_2 => Extensions list to filter. Comma separated values. "
                "Maximum of 10 values. Put \"all\" to keep count all files.\n");

            printf(
                "ARG_3 => .gitignore file path. If no one provided, no files "
                "will "
                "be ignored\n\n");

            return EXIT_SUCCESS;
        }

        str_set(working_dir_path, argv[1]);
    }

    str_set(working_dir_path, realpath(working_dir_path->str, null));

    // Read extenions list
    if (argc >= 3) {
        string *extensions_list = str_create(argv[2]);
        string *ext = str_create("");
        usize i = 0;

        while ((ext->str = strsep(&extensions_list->str, ",")) != null) {
            file_extensions_arr =
                realloc(file_extensions_arr,
                        sizeof(string *) * (array_size(file_extensions_arr) + 1));

            if (file_extensions_arr == null) {
                fprintf(stderr,
                        "Error: Can't allocate memory for extensions list\n");
                exit(1);
            }

            file_extensions_arr[i] = ext;
            i += 1;
        }

        str_free(extensions_list);
        str_free(ext);
    }

    // Read gitignore
    if (argc >= 4) {
        string *base_path = str_create(realpath(working_dir_path->str, null));
        string *gitignore_file_path = str_create(realpath(argv[3], null));
        FILE *gitignore_file_p = fopen(gitignore_file_path->str, "r");

        if (gitignore_file_p == null) {
            return false;
        }

        string *current_gitignore_line = str_create("");
        usize line_len = 0;
        // usize index = 0;
        ssize read;

        while ((read = getline(&current_gitignore_line->str, &line_len, gitignore_file_p)) != -1) {
            // Remove LF char
            str_setc(current_gitignore_line, '\0', read - 1);
            printf("%s", current_gitignore_line->str);
            // string *to_prepend = str_getc(current_gitignore_line, 0) == '/' ? "." : "./";

            /* usize to_prepend_len = str_len(to_prepend);
            usize current_gitignore_line_len = str_len(current_gitignore_line);

            string *path_to_ignore =
                (char *)malloc(to_prepend_len + current_gitignore_line_len + 1);

            if (path_to_ignore == null) {
                perror("Error");
                exit(1);
            }

            memcpy(path_to_ignore, to_prepend, to_prepend_len);
            memcpy(path_to_ignore + to_prepend_len, current_gitignore_line,
                   current_gitignore_line_len);

            char *FinalPathToIgnore = realpath(path_to_ignore, null);
            free(path_to_ignore); */

            /* if (FinalPathToIgnore != null) {
                char **IgnoredPathsRealoc = (char **)realloc(
                    ignored_paths_arr,
                    sizeof(char *) * (array_size(ignored_paths_arr) + 1));

                if (IgnoredPathsRealoc == null) {
                    perror("Error");
                    exit(1);
                }
                printf("final size ip: %zu\n", sizeof(ignored_paths_arr));

                ignored_paths_arr = IgnoredPathsRealoc;
                ignored_paths_arr[index] = FinalPathToIgnore;
                index += 1;
            }
            free(FinalPathToIgnore); */
        }
        fclose(gitignore_file_p);
        str_free(current_gitignore_line);
        str_free(gitignore_file_path);
        str_free(base_path);
    }

    fprintf(stdout, "Extensions allowed : ");
    for (usize idx = 0; idx < array_size(file_extensions_arr); ++idx) {
        fprintf(stdout, "[%s] ", file_extensions_arr[idx]->str);
    }
    fprintf(stdout, "\n\n");

    for (usize idx = 0; idx < array_size(ignored_paths_arr); ++idx) {
        fprintf(stdout, "[%s] ", ignored_paths_arr[idx]->str);
    }
    fprintf(stdout, "\n\n");

    read_directory(working_dir_path);

    str_free(working_dir_path);
    free(ignored_paths_arr);
    free(file_extensions_arr);

    fprintf(stdout, "\nTotal Lines count : %u\n", total_line_count);
    fprintf(stdout, "Files readed : %u\n", file_count);

    return 0;
}
