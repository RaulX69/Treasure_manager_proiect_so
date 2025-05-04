#ifndef TREASURE_MANAGER_H
#define TREASURE_MANAGER_H

typedef struct {
    char id[21];
    char user_name[21];
    float latitude;
    float longitude;
    char clue[51];
    int value;
} treasure;

treasure create_treasure();
void log_op(const char* hunt_id, const char* message);
void add(const char* hunt_id, treasure* new_treasure);
void list(const char* hunt_id);
void view(const char* hunt_id, const char* treasure_id);
void remove_treasure(const char* hunt_id, const char* treasure_id);
void remove_hunt(const char* hunt_id);

#endif
