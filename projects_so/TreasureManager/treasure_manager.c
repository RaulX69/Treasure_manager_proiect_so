#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include "treasure_manager.h"

#define TREASURE_FILE "treasure.dat"

treasure create_treasure()
{
    treasure new_treasure;
    printf("Create Treasure\nTreasure ID: ");
    fgets(new_treasure.id, sizeof(new_treasure.id), stdin);
    new_treasure.id[strcspn(new_treasure.id, "\n")] = 0;
    printf("User Name: ");
    fgets(new_treasure.user_name, sizeof(new_treasure.user_name), stdin);
    new_treasure.user_name[strcspn(new_treasure.user_name, "\n")] = 0;
    printf("Latitude: ");
    scanf("%f", &new_treasure.latitude);
    printf("Longitude: ");
    scanf("%f", &new_treasure.longitude);
    getchar();
    printf("Clue: ");
    fgets(new_treasure.clue, sizeof(new_treasure.clue), stdin);
    new_treasure.clue[strcspn(new_treasure.clue, "\n")] = 0;
    printf("Value: ");
    scanf("%d", &new_treasure.value);
    return new_treasure;
}

void log_op(const char* hunt_id, const char* message)
{
    char log_path[150];
    snprintf(log_path, sizeof(log_path), "%s/logged_hunt", hunt_id);
    int fd;
    if((fd = open(log_path, O_WRONLY|O_CREAT|O_APPEND, 0644)) == -1)
    {
        perror("error when opening file\n");
        exit(-1);
    }
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", tm_info);
    dprintf(fd, "%s %s\n", timestamp, message);
    close(fd);
}

void add(const char* hunt_id, treasure* new_treasure)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat st;
    int dir_exists = (stat(dir_path, &st) == 0 && S_ISDIR(st.st_mode));
    if(!dir_exists)
    {
        mkdir(dir_path, 0777);
        char log_path[150];
        snprintf(log_path, sizeof(log_path), "%s/logged_hunt", hunt_id);
        char link_name [200];
        snprintf(link_name, sizeof(link_name), "logged_hunt-%s", hunt_id);
        symlink(log_path, link_name);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_WRONLY|O_CREAT|O_APPEND, 0666)) == -1)
    {
        perror("error when opening file\n");
        exit(-1);
    }
    if(write(fd, new_treasure, sizeof(treasure)) != sizeof(treasure))
    {
        perror("error when writing in file\n");
        close(fd);
        exit(-1);
    }
    close(fd);
    char message[100];
    snprintf(message, sizeof(message), "add: treasure_id: %s\n", new_treasure->id);
    log_op(hunt_id, message);
}

void list(const char* hunt_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat dir_stat;
    stat(dir_path, &dir_stat);
    if(!S_ISDIR(dir_stat.st_mode))
    {
        perror("error when accessing director\n");
        exit(-1);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_RDONLY)) == -1)
    {
        perror("error when opening file\n");
        exit(-1);
    }
    struct stat file_stat;
    if(stat(file_path, &file_stat) == -1)
    {
        perror("error file\n");
        exit(-1);
    }
    printf("Hunt Name: %s\n", hunt_id);
    printf("Total File Siez: %ld bytes\n", file_stat.st_size);
    printf("Last Modification Time: %s\n", ctime(&file_stat.st_mtime));
    treasure treasure;
    while(read(fd, &treasure, sizeof(treasure)) == sizeof(treasure))
    {
        printf("Treasure ID: %s\nUser Name: %s\nCoordinates: (%4f, %4f)\nClue: %s\nValue: %d\n", treasure.id, treasure.user_name, treasure.latitude, treasure.longitude, treasure.clue, treasure.value);
    }
    close(fd);
    log_op(hunt_id, "list\n");
}

void view(const char* hunt_id, const char* treasure_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat dir_stat;
    stat(dir_path, &dir_stat);
    if(!S_ISDIR(dir_stat.st_mode))
    {
        //perror("error when accessing director\n");
        printf("Hunt not found\n");
        exit(-1);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_RDONLY)) == -1)
    {
        perror("error when opening file\n");
        exit(-1);
    }
    treasure treasure;
    int found = 0;
    while(read(fd, &treasure, sizeof(treasure)) == sizeof(treasure))
    {
        if(strcmp(treasure.id, treasure_id) == 0)
        {
            found = 1;
            printf("Treasure ID: %s\nUser Name: %s\nCoordinates: (%4f, %4f)\nClue: %s\nValue: %d\n", treasure.id, treasure.user_name, treasure.latitude, treasure.longitude, treasure.clue, treasure.value);
        }
    }
    if(found == 0)
    {
        printf("Treasure not found\n");
    }
    close(fd);
    char message[100];
    snprintf(message, sizeof(message), "view: treasure_id: %s\n", treasure_id);
    log_op(hunt_id, message);
}

void remove_treasure(const char* hunt_id, const char* treasure_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat dir_stat;
    stat(dir_path, &dir_stat);
    if(!S_ISDIR(dir_stat.st_mode))
    {
        perror("error when accessing director\n");
        exit(-1);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_RDONLY)) == -1)
    {
        perror("error when opening file\n");
        exit(-1);
    }
    char temp_path[150];
    snprintf(temp_path, sizeof(temp_path), "%s/temp.dat", dir_path);
    int fd2;
    if((fd2 = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC,  0666)) == -1)
    {
        perror("error when opening file 2\n");
        exit(-1);
    }
    treasure treasure;
    int found = 0;
    while(read(fd, &treasure, sizeof(treasure)) == sizeof(treasure))
    {
        if(strcmp(treasure.id, treasure_id) == 0)
        {
            found = 1;
        }
        else
        {
            write(fd2, &treasure, sizeof(treasure));
        }
    }
    close(fd);
    close(fd2);
    if(found == 1)
    {
        remove(file_path);
        rename(temp_path, file_path);
    }
    else
    {
        remove(temp_path);
        printf("Treasure not found\n");
    }
    char message[100];
    snprintf(message, sizeof(message), "remove_treasure: treasure_id: %s\n", treasure_id);
    log_op(hunt_id, message);
}

void remove_hunt(const char* hunt_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    DIR *dir = opendir(dir_path);
    if(!dir)
    {
        perror("error when opening director\n");
        exit(-1);
    }
    struct dirent *entry;
    char file_path[400];
    while((entry = readdir(dir)) != NULL)
    {
        if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);
            remove(file_path);
        }
    }
    closedir(dir);
    if(rmdir(dir_path) != 0)
    {
        perror("error when deleting director\n");
        exit(-1);
    }
}