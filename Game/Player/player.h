#ifndef PLAYER_H
#define PLAYER_H

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <curses.h>
#include <sys/mman.h>
#include <signal.h>

typedef enum type_t type_t;
typedef enum nextMove_t nextMove_t;
typedef enum action_t action_t;
typedef struct playerInfo playerInfo;
typedef struct playerRoom_t playerRoom_t;
typedef struct semaphore_t semaphore_t;
extern enum type_t creature;
extern sem_t end;
#define MAX_PLAYERS 4
#define LENGTH 6
#define ROWS 25
#define COLS 52

enum action_t{
    NOTHING,
    CONTINUE,
    JOIN_GAME,
    EXIT_GAME
};
enum type_t{
    HUMAN = 1,
    BOT
};

enum nextMove_t{
    NOWHERE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

enum color_t{
    WALL = 1,
    BUSH ,
    COINS,
    FIELD,
    BEAST,
    PLAYER,
    DROPPED_TREASURE,
    CAMPSITE
};

struct player_t{
    char player_sight[LENGTH-1][LENGTH];
    char previousSign;
    int bush_trap;
    pid_t PID,ServerPID;
    sem_t move,continueGame;
    uint8_t ID;
    uint16_t deaths;
    uint32_t x,y,spawn_x,spawn_y,campsite_x,campsite_y;
    uint32_t pocket_money,total_money,rounds;
    type_t type;
    nextMove_t nextMove;
};

struct playerInfo{
    pid_t PID;
    action_t action;
    type_t type;
    uint8_t ID;
};

struct semaphore_t{
    sem_t askToDo;
    sem_t waitForMoment;
    sem_t join;
    sem_t exit;
};


struct playerRoom_t{
    playerInfo Info[MAX_PLAYERS];
    semaphore_t semaphores;
};

extern struct player_t *player;
extern struct playerRoom_t * player_controller;
extern pthread_t threadOne,threadTwo,threadThree;
extern enum type_t creature;

void* draw_player_map(void* arg);
void* move_player(void* arg);
void* move_bot(void* arg);
void* bot_quit(void* arg);
int join(pid_t pid);
int TheEnd(pid_t pid);
int dealWithScreen(const char *,const int);
#endif