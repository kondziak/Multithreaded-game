#include <stdio.h>
#include "player.h"
#define SLEEP_TIME 150 * 1000
#define GAME "game"
#define ERROR -1

int main(int argc, char **argv)
{
	srand(time(0));
    int result,fd;
    printf("Gracz[1] czy bot[2]?");
    fd = scanf("%d",&result);
    if((result != HUMAN && result != BOT) || fd == 0) {
        return -1;
    }
    creature = result;
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr,TRUE);
    double time_spent = 0.0;
    clock_t begin = clock();
    for(;;){
        mvprintw(0,0,"Joining...");
        refresh();
        fd = shm_open(GAME,O_RDWR,0600);
        if(fd != -1){
            break;
        }
        if((double)(time_spent)/(double)(CLOCKS_PER_SEC) > 0.05){
             return dealWithScreen("Was not able to connect to server",ERROR);
        }
        usleep(SLEEP_TIME);
        time_spent = clock() - begin;
    }
    clear();
    result = ftruncate(fd,sizeof(struct playerRoom_t));
    if(result == -1){
        return dealWithScreen("Ftruncate failed",ERROR);
    }
    
    player_controller = (struct playerRoom_t*)mmap(NULL,sizeof(struct playerRoom_t),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(player_controller == NULL){
        return dealWithScreen("MMAP failed",ERROR);
    }
    
    result = join(getpid());
    if(result!= 0){
        if(creature == BOT){
            pthread_create(&threadOne,NULL,draw_player_map,NULL);
            pthread_create(&threadTwo,NULL,bot_quit,NULL);
            pthread_create(&threadThree,NULL,move_bot,NULL);

        }
        else{
            pthread_create(&threadOne,NULL,draw_player_map,NULL);
            pthread_create(&threadTwo,NULL,move_player,NULL);
        }
       sem_init(&end,0,0);
       sem_wait(&end); 
	}
    else{
        munmap(player_controller,sizeof(playerRoom_t));
    }
    clear();
    endwin();
    return 0;
}
