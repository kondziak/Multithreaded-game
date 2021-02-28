#include <stdio.h>
#include "server.h"
#include "functions.h"
#define GAME "game"
int main(int argc, char **argv)
{
    system("resize -s 25 110");
	srand(time(0));
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr,true);
    
    pid_t PID = getpid();
    int fd = shm_open(GAME,O_CREAT | O_RDWR,0600);
    if(fd == -1){
        return dealWithScreen("Shm failed",-1);
    }
    int result = ftruncate(fd,sizeof(gameboard_t));
    if(result == -1){
        shm_unlink(GAME);
        return dealWithScreen("Ftruncate failed",-1);
    }
    board.room = (struct playerRoom_t*)mmap(NULL,sizeof(struct playerRoom_t),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(board.room == NULL){
        shm_unlink(GAME);
        return dealWithScreen("Mmap error",-1);
    }

    pthread_mutexattr_init(&board.attr);
    pthread_mutexattr_settype(&board.attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&board.only_one,&board.attr);
    
    pthread_mutexattr_init(&board.attr2);
    pthread_mutexattr_settype(&board.attr2,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&board.second_one,&board.attr2);

    pthread_mutexattr_init(&board.attr3);
    pthread_mutexattr_settype(&board.attr3,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&board.third_one,&board.attr3);    
    
    pthread_mutexattr_init(&board.attr4);
    pthread_mutexattr_settype(&board.attr4,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&board.fourth_one,&board.attr4);    
    
    copyMap(maze,board.map,ROWS);
    FindA(&board.camp_x,&board.camp_y);
    clearTreauseres();
    board.ServerPID = PID;
    board.rounds = 0;
    board.maxBeasts = 0;
    sem_init(&beastSem,0,0);
    sem_init(&board.room->semaphores.askToDo,1,1);
    sem_init(&board.room->semaphores.waitForMoment,1,0);
    sem_init(&board.room->semaphores.exit,1,0);
    sem_init(&board.room->semaphores.join,1,0);

    for(int i = 0; i < MAX_PLAYERS; i++){
        board.room->Info[i].action = NOTHING;
        board.room->Info[i].type = BOT;
        board.room->Info[i].PID = -1;
        board.room->Info[i].ID = 255;
        board.deadPlayers[i] = false;
    }
    
    for(int i = 0; i < MAX_BEASTS; i++){
        board.beasts[i].doesItExist = false;
    }
    
    pthread_t threads[5];
    pthread_create(&threads[0],NULL,print,NULL);
    pthread_create(&threads[1],NULL,welcomePlayer,NULL);
    pthread_create(&threads[2],NULL,goodbyePlayer,NULL);
    pthread_create(&threads[3],NULL,pressTheButton,NULL);
    pthread_create(&threads[4],NULL,move_beast,NULL);
    
    sem_init(&end,0,0);
    sem_wait(&end);
    
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(board.gamePlayers[i]){
            char shm[30];
            sprintf(shm,"Player%u_%d",board.room->Info[i].ID,board.room->Info[i].PID);
            kill(board.gamePlayers[i]->PID,1);
            sem_destroy(&board.gamePlayers[i]->move);
            sem_destroy(&board.gamePlayers[i]->continueGame);
            munmap(board.gamePlayers[i],sizeof(struct player_t));
            board.gamePlayers[i] = 0;
            shm_unlink(shm);
        }
    }
    for(int i = 0; i < MAX_BEASTS; i++){
        if(board.beasts[i].doesItExist){
            sem_destroy(&board.beasts[i].waitTillGetSignal);
            sem_destroy(&board.beasts[i].CheckBeastMove);
            pthread_cancel(board.beasts[i].beast);
        }
    }
    
    sem_destroy(&board.room->semaphores.askToDo);
    sem_destroy(&board.room->semaphores.waitForMoment);
    sem_destroy(&board.room->semaphores.exit);
    sem_destroy(&board.room->semaphores.join);
    
    munmap(board.room,sizeof(struct playerRoom_t));
    shm_unlink(GAME);
    
    pthread_mutex_destroy(&board.only_one);
    sem_destroy(&end);
    system("clear");
    endwin();
	return 0;
}
