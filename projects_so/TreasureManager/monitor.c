#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "treasure_manager.h"

#define COMMAND_FILE "command.txt"

void list_hunts() 
{
    DIR *dir = opendir(".");
    if (!dir)
    {
        perror("Cannot open current directory");
        return;
    }

    struct dirent *entry;
    struct stat st;
    char file_path[300];
    int fd, count;
    treasure treasure;

    printf("Available hunts:\n");

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(file_path, sizeof(file_path), "%s/%s", entry->d_name, "treasure.dat");
            if (stat(file_path, &st) == 0)
            {
                fd = open(file_path, O_RDONLY);
                if (fd == -1) {
                    perror("Error opening treasure file");
                    continue;
                }

                count = 0;
                while (read(fd, &treasure, sizeof(treasure)) == sizeof(treasure))
                {
                    count++;
                }
                close(fd);

                printf("- %s: %d treasures\n", entry->d_name, count);
            }
        }
    }
    closedir(dir);
}

void handle_sigusr1(int signum)
{
    list_hunts();
    remove(COMMAND_FILE);
}

void handle_sigusr2(int signum)
{
    FILE *fp = fopen(COMMAND_FILE, "r");
    if (!fp)
    {
        perror("Could not open command file for reading");
        return;
    }

    char hunt_id[32];
    if (fgets(hunt_id, sizeof(hunt_id), fp))
    {
        hunt_id[strcspn(hunt_id, "\n")] = 0;
        list(hunt_id);
    }
    fclose(fp);
    remove(COMMAND_FILE);
}

void handle_sigint(int signum)
{
    FILE *fp = fopen(COMMAND_FILE, "r");
    if (!fp)
    {
        perror("Could not open command file for reading");
        return;
    }

    char line[128];
    if (fgets(line, sizeof(line), fp))
    {
        char hunt_id[32], treasure_id[40];
        if (sscanf(line, "%s %s", hunt_id, treasure_id) == 2)
        {
            view(hunt_id, treasure_id);
        }
        else
        {
            fprintf(stderr, "Invalid command format in file\n");
        }
    }

    fclose(fp);
    remove(COMMAND_FILE);
}

void handle_sigterm(int signum)
{
    exit(0);
}

void setup_signal_handlers()
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = handle_sigusr2;
    sigaction(SIGUSR2, &sa, NULL);

    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa, NULL);
}

int main()
{
    setup_signal_handlers();

    printf("[MONITOR] Running... PID = %d\n", getpid());

    while (1)
    {
        pause();   
    }

    return 0;
}
