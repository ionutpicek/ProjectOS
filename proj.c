#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s directory\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct dirent *str;
    DIR *director = opendir(argv[1]);
    if (director == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    char path[1024];

    while ((str = readdir(director)) != NULL) {
        if (strcmp(str->d_name, ".") == 0 || strcmp(str->d_name, "..") == 0)
            continue; // Skip current and parent directory entries

        snprintf(path, sizeof(path), "%s/%s", argv[1], str->d_name);
        printf("Path: %s\n", path);
    }

    closedir(director);
    exit(EXIT_SUCCESS);
}
