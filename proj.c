#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_DIRS 100
#define MAX_PATH_LEN 1024

void perform_syntactic_analysis(const char *file_path, const char *isolated_space_dir) {

    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    int line_count = 0;
    int word_count = 0;
    int char_count = 0;
    char buffer[1024];
    int contains_keyword = 0;
    int contains_non_ascii = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line_count++;

        char_count += strlen(buffer);
        char *word = strtok(buffer, " \t\n");
        while (word != NULL) {
            word_count++;
            word = strtok(NULL, " \t\n");
        }

        // check for keywords
        if (strstr(buffer, "corrupted") || strstr(buffer, "dangerous") || strstr(buffer, "risk") ||
            strstr(buffer, "attack") || strstr(buffer, "malware") || strstr(buffer, "malicious")) {
            contains_keyword = 1;
            break;
        }

        // check for non-ASCII characters
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] < 0 || buffer[i] > 127) {
                contains_non_ascii = 1;
                break;
            }
        }
    }

    fclose(file);

    printf("Syntactic analysis for file: %s\n", file_path);
    printf("Line count: %d\n", line_count);
    printf("Word count: %d\n", word_count);
    printf("Character count: %d\n", char_count);

    if (contains_keyword) {
        printf("File contains keywords associated with malicious content.\n");
    }
    if (contains_non_ascii) {
        printf("File contains non-ASCII characters.\n");
    }

    if (contains_keyword || contains_non_ascii) {
        printf("Moving file to a safe location.\n");
        char dest_path[MAX_PATH_LEN];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", isolated_space_dir, strrchr(file_path, '/') + 1);

        if (rename(file_path, dest_path) != 0) {
            perror("rename");
            exit(EXIT_FAILURE);
        }
    }
}

void update_snapshots(const char *output_dir, const char *input_dir, const char *isolated_space_dir) {
    DIR *dir = opendir(input_dir);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    char snapshot_path[MAX_PATH_LEN];
    snprintf(snapshot_path, sizeof(snapshot_path), "%s/snapshot.txt", output_dir);
    FILE *snapshot_file = fopen(snapshot_path, "w");
    if (snapshot_file == NULL) {
        perror("fopen");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    fprintf(snapshot_file, "Snapshot for directory: %s\n", input_dir);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char entry_path[MAX_PATH_LEN];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", input_dir, entry->d_name);

        // get metadata for the entry
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
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc < 5 || strcmp(argv[1], "-o") != 0) {
        printf("Usage: %s -o output_dir isolated_space_dir directory1 [directory2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char output_dir[MAX_PATH_LEN];
    strcpy(output_dir, argv[2]);
    char isolated_space_dir[MAX_PATH_LEN];
    strcpy(isolated_space_dir, argv[3]);

    int num_dirs = argc - 4;
    if (num_dirs > MAX_DIRS) {
        printf("Too many directories specified.\n");
        exit(EXIT_FAILURE);
    }

    // create a child process for each directory
    for (int i = 0; i < num_dirs; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            update_snapshots(output_dir, argv[i + 4], isolated_space_dir);
        }
    }

    // parent process: wait for all child processes to complete
    int status;
    pid_t wpid;
    while ((wpid = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("Child process %d exited with status %d\n", (int)wpid, WEXITSTATUS(status));
        } else {
            printf("Child process %d exited abnormally\n", (int)wpid);
        }
    }

    return EXIT_SUCCESS;
}
