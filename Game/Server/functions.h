#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "server.h"

#define MAX_BEASTS 3
#define MAX_PLAYERS 4
#define LENGTH 6

extern char maze [ROWS][COLS];
extern sem_t end,beastSem;
extern gameboard_t board;
extern int JustOnce;

void copyMap(const char tab[][COLS],char map[][COLS],uint8_t rows);
void clearTreauseres();
void draw_some_map();
void draw_some_board();
void draw_players_board(unsigned int k);
void PLAYER_DEATHS();
void CURRENT_X_Y();
void TYPE_Condition();
void PID_Condition();

void* pressTheButton(void *arg);
void* draw_player_map(void* arg);
void* print(void *arg);
void* makeCorrections(void *arg);
void* welcomePlayer(void *arg);
void* goodbyePlayer(void *arg);
void* move_beast(void* arg);

int FindA(uint32_t *x,uint32_t *y);
int returnMoney(int index);
int returnBroughtMoney(int index);
int chase(int playerID, int monsterID);
int checkPlayers(int k,int x,int y);
int dealWithScreen(const char *,const int);

#endif