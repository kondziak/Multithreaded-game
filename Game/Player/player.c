#include "player.h"

struct player_t *player;
struct playerRoom_t * player_controller;
pthread_t threadOne,threadTwo,threadThree;
enum type_t creature = BOT;
sem_t end;
int join(pid_t pid)
{
    sem_wait(&player_controller->semaphores.askToDo);
    int free_spot = -1;
    for(int i = 0 ; i < MAX_PLAYERS; i++){
        if(player_controller->Info[i].action == NOTHING){
            free_spot = i;
            break;
        }
    }
    if(free_spot == -1){
        sem_post(&player_controller->semaphores.askToDo);
        return dealWithScreen("All places are occupied.Come back later",0);
    }
    
    player_controller->Info[free_spot].action = JOIN_GAME;
    player_controller->Info[free_spot].type = creature;
    player_controller->Info[free_spot].PID = pid;
    player_controller->Info[free_spot].ID = free_spot + 1;
    
    mvprintw(0,0,"Connecting to the server.Please wait");
    refresh();
    usleep(1250*1000);
    mvprintw(0,56,"%d",player_controller->Info[free_spot].action);
    refresh();
    sem_post(&player_controller->semaphores.join);
    double time_spent = 0.0;
    clock_t begin;
    begin = clock();
    for(;;){
        if(sem_trywait(&player_controller->semaphores.waitForMoment) == 0){
            break;
        }
        if((double)time_spent/(double)CLOCKS_PER_SEC > 10){
            return dealWithScreen("Couldn't connect to the server",0);
        }
        time_spent = clock() - begin;
        mvprintw(2,25,"%lf",(double)time_spent/(double)CLOCKS_PER_SEC);
        refresh();
    }
    clear();
    char shm[30];
    sprintf(shm,"Player%u_%d",player_controller->Info[free_spot].ID,player_controller->Info[free_spot].PID);
    int result = shm_open(shm,O_RDWR,0600);
    if(result == -1){
        return dealWithScreen("shm_open failure",0);
    }
    
    int check = ftruncate(result,sizeof(struct player_t));
    if(check == -1){
        return dealWithScreen("ftruncate_error",0);
    }
    player = (struct player_t *)mmap(NULL,sizeof(struct player_t),PROT_WRITE|PROT_READ,MAP_SHARED,result,0);
    if(player == NULL){
        shm_unlink(shm);
        return dealWithScreen("mmap_error",0);
    }
    return 1;
}
int TheEnd(pid_t pid)
{
    sem_wait(&player_controller->semaphores.askToDo);
    int your_spot = -1;
    for(int i = 0; i < MAX_PLAYERS; i++){
        if(player_controller->Info[i].PID == pid){
            your_spot = i;
            break;
        }
    }
    player_controller->Info[your_spot].action = EXIT_GAME;
    sem_post(&player_controller->semaphores.exit);
    clear();
    mvprintw(0,0,"Exitting Server");
    refresh();
    usleep(100*100);
    clock_t begin;
    double time_spent = 0.0;
    begin = clock();
    char shm[30];
    sprintf(shm,"Player%u_%d",player_controller->Info[your_spot].ID,player_controller->Info[your_spot].PID);
    for(;;){
        if(sem_trywait(&player_controller->semaphores.waitForMoment) == 0){
            break;
        }
        if(time_spent/CLOCKS_PER_SEC > 5){
            shm_unlink(shm);
            munmap(player_controller,sizeof(struct playerRoom_t));
            return dealWithScreen("Couldn't connect to the server",0);
        }
        time_spent = clock() - begin;
    }
    shm_unlink(shm);
    munmap(player_controller,sizeof(playerRoom_t));
    return 1;
}
void* move_player(void* arg)
{
    int step = 'z';
    while(1){
        step = getch();
        switch(step){
            case KEY_LEFT:
                    player->nextMove = LEFT;
                break;
            case KEY_UP:
                    player->nextMove = UP;
                break;
            case KEY_RIGHT:
                    player->nextMove = RIGHT;
                break;
            case KEY_DOWN:
                    player->nextMove = DOWN;
                break;                
            case 'q':
                TheEnd(player->PID);
                break;
        }
        if(step == 'q') break;
        sem_post(&player->move);
        sem_wait(&player->continueGame);
    }
    pthread_cancel(threadOne);
    usleep(100 * 150);
    sem_post(&end);
    
    return (void*)0;
}
void* draw_player_map(void* arg)
{
    start_color();
    use_default_colors();
    
    init_pair(WALL,COLOR_BLACK,COLOR_BLACK);
    init_pair(BUSH,COLOR_GREEN,COLOR_WHITE);
    init_pair(COINS,COLOR_YELLOW,COLOR_WHITE);
    init_pair(FIELD,COLOR_WHITE,COLOR_WHITE);
    init_pair(BEAST,COLOR_RED,COLOR_WHITE);
    init_pair(PLAYER,COLOR_WHITE,COLOR_MAGENTA);
    init_pair(DROPPED_TREASURE,COLOR_GREEN,COLOR_YELLOW);
    init_pair(CAMPSITE,COLOR_YELLOW,COLOR_GREEN);
    
    while(1){
        for(int i = 0 ,j; i < LENGTH-1; i++){
            for(j = 0; j < LENGTH-1; j++){
                const char letter = player->player_sight[i][j];
                if(letter== 'X'){
                    attron(COLOR_PAIR(WALL));
                    mvprintw(i,j,"%c",'X');
                    attroff(COLOR_PAIR(WALL));
                }
                else if(letter == '#'){
                    attron(COLOR_PAIR(BUSH));
                    mvprintw(i,j,"%c",'#');
                    attroff(COLOR_PAIR(BUSH));
                }
                else if(letter == 'c' || letter == 't' || letter == 'T'){
                    attron(COLOR_PAIR(COINS));
                    mvprintw(i,j,"%c",letter);
                    attroff(COLOR_PAIR(COINS));
                }
                else if(letter == '*'){
                    attron(COLOR_PAIR(BEAST));
                    mvprintw(i,j,"%c",'*');
                    attroff(COLOR_PAIR(BEAST));
                }
                else if(isdigit(letter)){
                    attron(COLOR_PAIR(PLAYER));
                    mvprintw(i,j,"%c",letter);
                    attroff(COLOR_PAIR(PLAYER));
                }
                else if(letter == 'A'){
                    attron(COLOR_PAIR(CAMPSITE));
                    mvprintw(i,j,"%c",'A');
                    attroff(COLOR_PAIR(CAMPSITE));
                }
                else if(letter == 'D'){
                    attron(COLOR_PAIR(DROPPED_TREASURE));
                    mvprintw(i,j,"%c",'D');
                    attroff(COLOR_PAIR(DROPPED_TREASURE));
                }
                else{
                    attron(COLOR_PAIR(FIELD));
                    mvprintw(i,j,"%c",' ');
                    attroff(COLOR_PAIR(FIELD));
                }
                refresh();
            }
            mvprintw(i,j,"%c",'\n');
            refresh();
        }
    }
    
    return NULL;
}
void* move_bot(void* arg)
{
    const int tab[4]={LEFT,UP,RIGHT,DOWN};
    while(1){
        player->nextMove = tab[rand()%4];
        sem_post(&player->move);
        sem_wait(&player->continueGame);
        sleep(1);
        usleep(100 * 1250);
    }
    return NULL;
}
void* bot_quit(void* arg)
{
    int step;
    while(1){
        step = getch();
        if(step == 'q'){
            TheEnd(player->PID);
            pthread_cancel(threadThree);
            pthread_cancel(threadOne);
            break;
        }
    }
    usleep(125 * 1000);
    sem_post(&end);
    return NULL;
}
int dealWithScreen(const char * text, const int signal)
{
    clear();
    endwin();
    printf("%s",text);
    return signal;
}