#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "treasure_manager.h"

typedef struct{
    char name[100];
    int score;
} user;

int user_count;

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("Usage: ./calculate <hunt_id>\n");
        exit(-1);
    }

    char file_path[150];
    snprintf(file_path, sizeof(file_path), "%s/treasure.dat", argv[1]);

    int fd;
    if((fd = open(file_path, O_RDONLY, 0666)) == -1)
    {
        printf("error at opening file\n");
        exit(-1);
    }

    treasure t;
    user users[100];
    while(read(fd, &t, sizeof(t)) == sizeof(t))
    {
        int found = 0;
        for(int i = 0; i < user_count; i++)
        {
            if(strcmp(t.user_name, users[i].name) == 0)
            {
                users[i].score = users[i].score + t.value;
                found = 1;
                break;
            }
        }
        if(found == 0)
        {
            strcpy(users[user_count].name, t.user_name);
            users[user_count].score = t.value;
            user_count++;
        }
    }

    for(int i = 0; i < user_count; i++)
    {
        printf("%s score: %d\n", users[i].name, users[i].score);
    }
    return 0;
}