#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_DIRS 100
#define MAX_PATH_LEN 1024

void update_snapshots(const char *output_dir, const char *input_dirs[], int num_dirs) {
    for (int i = 0; i < num_dirs; i++) {
        const char *input_dir = input_dirs[i];
        DIR *dir = opendir(input_dir);
        if (dir == NULL) {
            perror("opendir");
            continue;
        }

        char snapshot_path[MAX_PATH_LEN];
        snprintf(snapshot_path, sizeof(snapshot_path), "%s/snapshot_%d.txt", output_dir, i + 1);
        FILE *snapshot_file = fopen(snapshot_path, "w");
        if (snapshot_file == NULL) {
            perror("fopen");
            closedir(dir);
            continue;
        }

        fprintf(snapshot_file, "Snapshot for directory: %s\n", input_dir);

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            char entry_path[MAX_PATH_LEN];
            snprintf(entry_path, sizeof(entry_path), "%s/%s", input_dir, entry->d_name);

            // Get metadata for the entry
            struct stat entry_stat;
            if (stat(entry_path, &entry_stat) == -1) {
                perror("stat");
                continue;
            }

            fprintf(snapshot_file, "Name: %s, Size: %ld bytes, Modified: %ld\n", entry->d_name,
                    (long)entry_stat.st_size, (long)entry_stat.st_mtime);
        }

        fclose(snapshot_file);
        closedir(dir);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "-o") != 0) {
        printf("Usage: %s -o output_dir directory1 [directory2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char output_dir[MAX_PATH_LEN];
    strcpy(output_dir, argv[2]);

    const char *input_dirs[MAX_DIRS];
    int num_dirs = argc - 3;
    if (num_dirs > MAX_DIRS) {
        printf("Too many directories specified.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_dirs; i++) {
        input_dirs[i] = argv[i + 3];
    }

    update_snapshots(output_dir, input_dirs, num_dirs);

    return EXIT_SUCCESS;
}