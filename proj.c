//S1
//Description: The proposed project combines functionalities for monitoring a directory to manage differences between two captures (snapshots) of it. 
//The user will be able to observe and intervene in the changes in the monitored directory.
//Directory Monitoring:
//The user can specify the directory to be monitored as an argument in the command line, and the program will track changes occurring in it and its subdirectories, 
//parsing recursively each entry from the directory.
//With each run of the program, the snapshot of the directory will be updated, storing the metadata of each entry. 

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char *argv[]){

    if(argc != 2){
        printf("Illegal number of arguments");
    }

    struct dirent *str;

    DIR *director = opendir(argv[1]);

    while((str = readdir(director)) != NULL){
        printf("%s\n",str -> d_name);

        char path[50];
        
        sprintf(argv[1],"/%s",str -> d_name);
        
        printf("Path %s",path);
        printf("Director - \n");
    }

    closedir(director);
    exit(EXIT_SUCCESS);
}