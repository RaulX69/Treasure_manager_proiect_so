#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define TREASURE_FILE "treasure.dat"

typedef struct{
    char id[21];
    char user_name[21];
    float latitude;
    float longitude;
    char clue[51];
    int value;
}treasure;

treasure create_treasure()
{
    treasure new_treasure;
    printf("Create Treasure\nTreasure ID: ");
    scanf(" %20[^\n]", new_treasure.id);
    printf("User Name: ");
    scanf(" %20[^\n]", new_treasure.user_name);
    //fgets(new_treasure.user_name, sizeof(new_treasure.user_name), stdin);
    printf("Latitude: ");
    scanf("%f", &new_treasure.latitude);
    printf("Longitude: ");
    scanf("%f", &new_treasure.longitude);
    printf("Clue: ");
    scanf(" %50[^\n]", new_treasure.clue);
    //fgets(new_treasure.clue, sizeof(new_treasure.clue), stdin);
    printf("Value: ");
    scanf("%d", &new_treasure.value);
    return new_treasure;
}

void add(char* hunt_id, treasure* new_treasure)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    mkdir(dir_path, 0777);
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_WRONLY|O_CREAT|O_APPEND, 0666)) == -1)
    {
        perror("eroare la deschidere fisier\n");
        exit(-1);
    }
    if(write(fd, new_treasure, sizeof(treasure)) != sizeof(treasure))
    {
        perror("eroare la scriere in fisier\n");
        close(fd);
        exit(-1);
    }
    close(fd);
}

void list(char* hunt_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat dir_stat;
    stat(dir_path, &dir_stat);
    if(!S_ISDIR(dir_stat.st_mode))
    {
        perror("eroare accesare director\n");
        exit(-1);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_RDONLY)) == -1)
    {
        perror("eroare la deschidere fisier\n");
        exit(-1);
    }
    struct stat file_stat;
    if(stat(file_path, &file_stat) == -1)
    {
        perror("eroare fisier\n");
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
}

void view(char* hunt_id, char* treasure_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat dir_stat;
    stat(dir_path, &dir_stat);
    if(!S_ISDIR(dir_stat.st_mode))
    {
        perror("eroare accesare director\n");
        exit(-1);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_RDONLY)) == -1)
    {
        perror("eroare la deschidere fisier\n");
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
}

void remove_treasure(char* hunt_id, char* treasure_id)
{
    char dir_path[100];
    snprintf(dir_path, sizeof(dir_path), "%s", hunt_id);
    struct stat dir_stat;
    stat(dir_path, &dir_stat);
    if(!S_ISDIR(dir_stat.st_mode))
    {
        perror("eroare accesare director\n");
        exit(-1);
    }
    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, TREASURE_FILE);
    int fd;
    if((fd = open(file_path, O_RDONLY)) == -1)
    {
        perror("eroare la deschidere fisier\n");
        exit(-1);
    }
    char temp_path[150];
    snprintf(temp_path, sizeof(temp_path), "%s/temp.dat", dir_path);
    int fd2;
    if((fd2 = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC,  0666)) == -1)
    {
        perror("eroare la deschidere fisier 2\n");
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
}
int main(int argc, char** argv)
{
    if(argc > 1)
    {
        if(strcmp(argv[1], "--add") == 0)
        {
            if(argc == 3)
            {
                treasure new_treasure = create_treasure();
                add(argv[2], &new_treasure);
            }
            else
            {
                perror("usage: ./exe --add <hunt_id>\n");
                exit(-1);
            }
        }
        if(strcmp(argv[1], "--list") == 0)
        {
            if(argc == 3)
            {
                list(argv[2]);
            }
            else
            {
                perror("usage: ./exe --list <hunt_id>\n");
                exit(-1);
            }
        }
        if(strcmp(argv[1], "--view") == 0)
        {
            if(argc == 4)
            {
                view(argv[2], argv[3]);
            }
            else
            {
                perror("usage: ./exe --view <hunt_id> <id>\n");
                exit(-1);
            }
        }
        if(strcmp(argv[1], "--remove_treasure") == 0)
        {
            if(argc == 4)
            {
                remove_treasure(argv[2], argv[3]);
            }
            else
            {
                perror("usage: ./exe --remove_treasure <hunt_id> <id>\n");
                exit(-1);
            }
        }
    }
    else
    {
        perror("usage: ./exe <operation> <other arguments>\n");
        exit(-1);
    }
    return 0;
}