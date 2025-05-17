#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "treasure_manager.h"

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
    fflush(stdout);
}

void handle_sigusr1(int signum)
{
    list_hunts();
    fflush(stdout);
}

void handle_sigusr2(int signum)
{
    char hunt_id[32];
    if (fgets(hunt_id, sizeof(hunt_id), stdin))
    {
        hunt_id[strcspn(hunt_id, "\n")] = 0;
        pid_t pid = fork();
        if(pid == 0)
        {
            execl("./exe", "./exe", "--list", hunt_id, NULL);
            perror("perror at execl\n");
            exit(1);
        }
    }
    else
    {
        perror("invalid format\n");
    }
}

void handle_sigint(int signum)
{
    char line[128];
    if (fgets(line, sizeof(line), stdin))
    {
        char hunt_id[32], treasure_id[40];
        if (sscanf(line, "%s %s", hunt_id, treasure_id) == 2)
        {
            pid_t pid = fork();
            if(pid == 0)
            {
                execl("./exe", "./exe", "--view", hunt_id, treasure_id, NULL);
                perror("error at execl\n");
                exit(1);
            }
        }
        else
        {
            perror("invalid format\n");
        }
    }
}

void handle_sigterm(int signum)
{
    usleep(3000000);
    fflush(stdout);
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

    while (1)
    {
        pause();   
    }

    return 0;
}
