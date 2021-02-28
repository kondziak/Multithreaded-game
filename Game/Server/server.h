#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <curses.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
typedef enum type_t type_t;
typedef enum nextMove_t nextMove_t;
typedef enum action_t action_t;
typedef struct gameboard_t gameboard_t;
typedef struct playerInfo playerInfo;
typedef struct semaphore_t semaphore_t;
#define ROWS 25
#define COLS 52
#define LENGTH 6
#define MAX_PLAYERS 4
#define MAX_BEASTS 3

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

struct monster_t{
    pthread_t beast;
    sem_t waitTillGetSignal,CheckBeastMove;
    int32_t beast_x,beast_y,rand_dir;
    nextMove_t move;
    bool doesItExist,hasMoved;
    char previousSign;
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

struct gameboard_t{
    bool deadPlayers[MAX_PLAYERS];
    char map[ROWS][COLS];
    pthread_mutex_t only_one,second_one,third_one,fourth_one;
    pthread_mutexattr_t attr,attr2,attr3,attr4;
    pid_t ServerPID;
    uint8_t maxBeasts;
    uint32_t camp_x,camp_y,rounds,treasures[ROWS][COLS-1];
    struct playerRoom_t * room;
    struct monster_t beasts[MAX_BEASTS];
    struct player_t* gamePlayers [MAX_PLAYERS];
};



#endif