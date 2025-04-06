#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define TREASURE_FILE "treasure.dat"

typedef struct{
    int id;
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
    scanf("%d", &new_treasure.id);
    printf("User Name: ");
    scanf("%20s", new_treasure.user_name);
    printf("Latitude: ");
    scanf("%f", &new_treasure.latitude);
    printf("Longitude: ");
    scanf("%f", &new_treasure.longitude);
    printf("Clue: ");
    scanf("%50s", new_treasure.clue);
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
        printf("Treasure ID: %d\nUser Name: %s\nCoordinates: (%4f, %4f)\nClue: %s\nValue: %d\n", treasure.id, treasure.user_name, treasure.latitude, treasure.longitude, treasure.clue, treasure.value);
    }
    close(fd);
}
int main(int argc, char** argv)
{
    if(argc > 1)
    {
        if(strcmp(argv[1], "--add") == 0)
        {
            treasure new_treasure = create_treasure();
            add(argv[2], &new_treasure);
        }
        if(strcmp(argv[1], "--list") == 0)
        {
            list(argv[2]);
        }
    }
    return 0;
}