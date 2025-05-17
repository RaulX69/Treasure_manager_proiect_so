#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "treasure_manager.h"

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
                printf("usage: ./exe --add <hunt_id>\n");
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
                printf("usage: ./exe --list <hunt_id>\n");
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
                printf("usage: ./exe --view <hunt_id> <id>\n");
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
                printf("usage: ./exe --remove_treasure <hunt_id> <id>\n");
                exit(-1);
            }
        }
        if(strcmp(argv[1], "--remove_hunt") == 0)
        {
            if(argc == 3)
            {
                remove_hunt(argv[2]);
            }
            else
            {
                printf("usage: ./exe --remove_hunt <hunt_id>\n");
                exit(-1);
            }
        }
    }
    else
    {
        printf("usage: ./exe <operation> <other arguments>\n");
        exit(-1);
    }
    return 0;
}