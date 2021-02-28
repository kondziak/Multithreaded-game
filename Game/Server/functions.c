#include "functions.h"
#define GAME "game"
sem_t end,beastSem;
gameboard_t board;
int JustOnce = 0;

char maze [ROWS][COLS] ={
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "X   X       X    ####       X         X       X   X",
    "X X XXX XXX XXXXXXXXXXX XXX X XXXXXXX XXX XXXXX   X",
    "X X   X X X           X X X   X     X     X   X   X",
    "X XXX X X XXX###XXXXX X X XXXXX XXXXXXXXXXXXX XXX X",
    "X X X   X           X X X ##  X       X       X X X",
    "X X XXXXX XXX XXXXXXX X X X XXX XXX XXX XXX X X X X",
    "X X         X X       X X X     X   X   X X X   X X",
    "X X XXXXXXX XXX XXXXXXX XXXXX XXX XXX XXX X XXX X X",
    "X X X     X   X   X     X   X   X         X X X X X",
    "X XXX XXX XXX XXX XXX XXX X XXX XXXXXXXXXXX X X X X",
    "X X   X       X X   X  A  X X   X X       X X   X X",
    "X X XXXXXX#XX X XXX XXX XXX XXX X X XXXXX X X XXX X",
    "X X    #X   X X   X   X   X   X   X X     X X X   X",
    "X X X ##X XXX XXX XXX XXXXXXX XXX X XXX XXX X X XXX",
    "X X X## X    #  X   X X  ###  X   X   X     X X X X",
    "X X X#  XXXXXXX X X X X XX#XXXX XXXXX XXXXXXX X X X",
    "X X X#      X   X X X   X     X   X X       ##X   X",
    "X XXXXXXXXX X XXX XXXXXXX XXXXXXX X XXXXX X ##XXX X",
    "X X#      X X     X     X       X   X   X X ##  X X",
    "X X XXXXX X XXXXXXX X XXX XXXXX XXX X X XXX#XXXXX X",
    "X###X     X         X  ###X## X     X X   X###### X",
    "X XXX XXXXXXXXXXXXXXXXXXXXX#X XXXXXXX XXX X#    # X",
    "X   X                 ######X##         X    ##   X",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"};

void copyMap(const char tab[][COLS],char map[][COLS],uint8_t rows)
{
    for(uint8_t i= 0; i < rows; i++){
        strcpy(map[i],tab[i]);
    }
}
void clearTreauseres()
{
    for(int i = 0; i < ROWS;++i){
        for(int j = 0; j < COLS-1; j++){
            board.treasures[i][j]=0;
        }
    }
}
void PID_Condition()
{
    
    for(unsigned int i = 0 ; i < MAX_PLAYERS; i++){
        if(board.gamePlayers[i]){
            mvprintw(7,56 + 10*(i+1)," %d        ",board.gamePlayers[i]->PID);
        }
        else{
            mvprintw(7,56 + 10*(i+1)," --/--        ");
        }
    }
}
void TYPE_Condition()
{
    for(unsigned int i = 0 ; i < MAX_PLAYERS; i++){
        if(!board.gamePlayers[i]){
            mvprintw(8,56 + 10*(i+1)," --/--        ");
        }
        else if(board.gamePlayers[i] != (void*)0 && board.gamePlayers[i]->type == HUMAN){
            mvprintw(8,56 + 10*(i+1)," %s        ","HUMAN");
        }
        else{
            mvprintw(8,56 + 10*(i+1)," %s        ","BOT");
        }
    }
}
void CURRENT_X_Y()
{
    for(unsigned int i = 0 ; i < MAX_PLAYERS; i++){
        if(board.gamePlayers[i] != NULL){
            mvprintw(9,56 + 10*(i+1)," %d/%d      ",board.gamePlayers[i]->x,board.gamePlayers[i]->y);
        }
        else{
            mvprintw(9,56 + 10*(i+1)," --/--        ");
        }
    }
}
void PLAYER_DEATHS()
{
    for(unsigned int i = 0 ; i < MAX_PLAYERS ; i++){
        if(board.gamePlayers[i] != NULL){
            mvprintw(10,56 + 10*(i+1)," %d        ",board.gamePlayers[i]->deaths);
        }
        else{
            mvprintw(10,56 + 10*(i+1)," --/--        ");
        }
    }
}
void draw_some_board(){  
        mvprintw(2,55,"%s %d","Server's PID:",board.ServerPID);
        mvprintw(3,57,"%s %d/%d","Campsite's",board.camp_x,board.camp_y);
        mvprintw(4,57,"%s %u","Round number: ",board.rounds);
        mvprintw(6,55,"%s  %s  %s  %s  %s","Parameter","Player 1","Player 2","Player 3","Player 4");
        mvprintw(7,56,"%s","PID");
        mvprintw(8,56,"%s","Type");
        mvprintw(9,56,"%s","Curr X/Y");
        mvprintw(10,56,"%s","Death");
        mvprintw(12,55,"%s","Coins");
        PID_Condition();
        TYPE_Condition();
        CURRENT_X_Y();
        PLAYER_DEATHS();
        mvprintw(13,56,"%s   %d         %d         %d         %d","Carried",returnMoney(0),returnMoney(1),returnMoney(2),returnMoney(3));
        mvprintw(14,56,"%s   %d         %d         %d         %d","Brought",returnBroughtMoney(0),returnBroughtMoney(1),returnBroughtMoney(2),
        returnBroughtMoney(3));
        mvprintw(16,55,"Legend:");
        attron(COLOR_PAIR(PLAYER));
        mvprintw(17,57,"1234");
        attroff(COLOR_PAIR(PLAYER));
        mvprintw(17,62,"- players");
        attron(COLOR_PAIR(WALL));
        mvprintw(18,57," ");
        attroff(COLOR_PAIR(WALL));
        mvprintw(18,62,"- wall");
        mvprintw(19,57,"#    - bushes");
        attron(COLOR_PAIR(BEAST));
        mvprintw(20,57,"*");
        attroff(COLOR_PAIR(BEAST));
        mvprintw(20,62,"- beast");
        attron(COLOR_PAIR(COINS));
        mvprintw(21,57,"c");
        attroff(COLOR_PAIR(COINS));
        mvprintw(21,62,"- one coin");
        attron(COLOR_PAIR(DROPPED_TREASURE));
        mvprintw(21,82,"D");
        attroff(COLOR_PAIR(DROPPED_TREASURE));
        mvprintw(21,84,"- Dropped treasure");
        attron(COLOR_PAIR(COINS));
        mvprintw(22,57,"t");
        attroff(COLOR_PAIR(COINS));
        mvprintw(22,62,"- treasuere(10 coins)");
        attron(COLOR_PAIR(COINS));
        mvprintw(23,57,"T");
        attroff(COLOR_PAIR(COINS));
        mvprintw(23,62,"- Large Treasure(50 coins)");
        mvprintw(24,57,"A    - Campsite");
}
void draw_some_map(){
    for(unsigned int i = 0,j; i < ROWS ; i++){
        for(j = 0; j < COLS-1 ;j++){
            pthread_mutex_lock(&board.only_one);
            const char letter = board.map[i][j];
            if(letter== 'X'){
                attron(COLOR_PAIR(WALL));
                mvprintw(i,j,"%c",' ');
                attroff(COLOR_PAIR(WALL));
            }
            else if(isdigit(letter)){
                attron(COLOR_PAIR(PLAYER));
                mvprintw(i,j,"%c",letter);
                attroff(COLOR_PAIR(PLAYER));
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
            else if(letter == '*'){
                attron(COLOR_PAIR(BEAST));
                mvprintw(i,j,"%c",'*');
                attroff(COLOR_PAIR(BEAST));
            }
            else if(letter == ' '){
                attron(COLOR_PAIR(FIELD));
                mvprintw(i,j,"%c",' ');
                attroff(COLOR_PAIR(FIELD));
            }
            pthread_mutex_unlock(&board.only_one);
        }
    }
}
void draw_players_board(unsigned int k)
{
        if(board.gamePlayers[k]){
            unsigned int position_x = board.gamePlayers[k]->x;
            unsigned int position_y = board.gamePlayers[k]->y;
            int result;
            if(position_x > 1 && position_x < 23){
                if(position_y > 1 && position_y < 49){
                    for(unsigned int i = 0 ,pos_x = position_x - 2; i < LENGTH-1 ; i++,pos_x++){
                        for(unsigned int j = 0, pos_y = position_y - 2; j < LENGTH-1; j++,pos_y++){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(result != -1 ){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else if(position_x == pos_x && position_y ==pos_y  && result == -1 && result != k){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                            }
                        }
                    }
                }
                else if(position_y == 1){
                    for(unsigned int i = 0, pos_x = position_x-2; i < LENGTH-1; i++, pos_x+=1){
                        for(unsigned int j = 0, pos_y = position_y-1; j < LENGTH-1 ; j++ ,pos_y+=1){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(result != -1){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else if(position_x == pos_x && position_y ==pos_y  && result == -1 && result != k){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                            }
                        }
                    }
                }
                else if(position_y == 49){
                    for(unsigned int i = 0, pos_x = position_x-2; i < LENGTH-1; i++, pos_x+=1){
                        for(unsigned int j = 0, pos_y = position_y-3; j < LENGTH-1; j++ ,pos_y+=1){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(result != -1 ){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else if(position_x == pos_x && position_y ==pos_y  && result == -1){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                            }
                        }
                    }
                }
            }
            else if(position_x == 1){
                if(position_y < 47){
                    for(unsigned int i = 0 ,pos_x = position_x-1; i < LENGTH-1; i++,pos_x++){
                        for(unsigned int j = 0, pos_y = position_y-1 ; j < LENGTH-1; j++,pos_y++){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(result != -1){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else if(position_x == pos_x && position_y ==pos_y ){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                            }
                        }
                    }
                }
                else{
                    for(unsigned int i = 0 ,pos_x = position_x-1; i < LENGTH-1; i++,pos_x++){
                        for(unsigned int j = 0, pos_y = position_y -3 ; j < LENGTH-1; j++,pos_y++){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(result != -1){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else if(position_x == pos_x && position_y ==pos_y){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                            }
                        }
                    }
                }
            }
            else if(position_x == 23){
                if(position_y < 48){
                    for(unsigned int i = 0 ,pos_x = position_x-3; i < LENGTH-1; i++,pos_x++){
                        for(unsigned int j = 0, pos_y = position_y-1; j < LENGTH-1; j++,pos_y++){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(position_x == pos_x && position_y ==pos_y){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else if(result != -1){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                            }
                        }
                    }
                }
                else{
                    for(unsigned int i = 0 ,pos_x = position_x-3; i < LENGTH-1; i++,pos_x++){
                        for(unsigned int j = 0, pos_y = position_y-3 ; j < LENGTH-1; j++,pos_y++){
                            result = checkPlayers(k,pos_x,pos_y);
                            if(position_x == pos_x && position_y ==pos_y ){
                                board.gamePlayers[k]->player_sight[i][j] = board.gamePlayers[k]->ID + '0';
                            }
                            else if(result != -1){
                                board.gamePlayers[k]->player_sight[i][j] = result + '0';
                            }
                            else{
                                board.gamePlayers[k]->player_sight[i][j] = board.map[pos_x][pos_y];
                                
                            }
                        }
                    }
                }
            }
        }
}
int returnBroughtMoney(int index){
    if(board.gamePlayers[index] == NULL)
        return 0;
    return board.gamePlayers[index]->total_money;
}
int returnMoney(int index)
{
    if(board.gamePlayers[index] == NULL)
        return 0;
    return board.gamePlayers[index]->pocket_money;
}
int checkPlayers(int k,int x,int y)
{
    for(int i = 0; i < MAX_PLAYERS;i++){
        if(board.gamePlayers[i]){
            if(i != k){
                 if((int)board.gamePlayers[i]->x == x && (int)board.gamePlayers[i]->y == y) return (int)board.gamePlayers[i]->ID;
            }
        }
    }
    return -1;
}
int dealWithScreen(const char * text, const int signal)
{
    clear();
    endwin();
    printf("%s",text);
    return signal;
}
int chase(int playerID, int monsterID)
{
    int player_x = board.gamePlayers[playerID]->x ,player_y = board.gamePlayers[playerID]->y ; 
    int beast_x = board.beasts[monsterID].beast_x , beast_y = board.beasts[monsterID].beast_y;
    int sight = 3;
    if((player_x <= (beast_x + 2) && player_x >= (beast_x - 2)) &&(player_y <= (beast_y + 2) && player_y >= (beast_y- 2))){
    
        if(player_x == beast_x ){
            if(player_y > beast_y){
                for(int i = beast_y+1 , j = 0; i != player_y && j < sight; i++,j++){
                    if(board.map[player_x][i] == 'X'){
                        return 1;
                    }
                }
                board.beasts[monsterID].move = RIGHT;
                return 0;
            }
            else{
                for(int i = beast_y-1, j = 0; i != player_y && j < sight; i--,j++){
                    if(board.map[player_x][i] == 'X'){
                        return 1;
                    }
                }
                board.beasts[monsterID].move = LEFT;
                return 0;
            }
        }
        else if(player_y == beast_y){
            if(player_x > beast_x){
                for(int i = beast_x+1 , j = 0; i != player_x && j < sight; i++,j++){
                    if(board.map[i][beast_y] == 'X'){
                        return 1;
                    }
                }
                board.beasts[monsterID].move = DOWN;
                return 0;
            }
            else{
                for(int i = beast_x-1 , j = 0; i != (int)board.gamePlayers[playerID]->x && j < sight; i--,j++){
                    if(board.map[i][beast_y] == 'X'){
                        return 1;
                    }
                }
                board.beasts[monsterID].move = UP;
                return 0;
            }
        }
        
        for(int i = beast_x+1, j = beast_y+1,k = 0; k < sight; j++,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(j == player_y){
                if(board.map[beast_x+1][beast_y] != 'X'){
                    board.beasts[monsterID].move = DOWN;
                    return 0;
                }
                else if(board.map[beast_x][beast_y+1] != 'X'){
                    board.beasts[monsterID].move = RIGHT;
                    return 0;
                }
            }
        }
        for(int i = beast_x+1, j = beast_y-1,k = 0; k < sight;j--,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(j == player_y){
                if(board.map[beast_x+1][beast_y] != 'X'){
                    board.beasts[monsterID].move = DOWN;
                    return 0;
                }
                else if(board.map[beast_x][beast_y-1]!= 'X'){
                    board.beasts[monsterID].move = LEFT;
                    return 0;
                }
            }
        }
        for(int i = beast_x-1, j = beast_y+1,k = 0; k < sight;j++,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(j == player_y){
                if(board.map[i][beast_y] != 'X'){
                    board.beasts[monsterID].move = UP;
                    return 0;
                }
                else if(board.map[beast_x][beast_y+1] != 'X'){
                    board.beasts[monsterID].move = RIGHT;
                    return 0;
                }
            }
        }
        for(int i = beast_x-1, j = beast_y-1,k = 0; k < sight;j--,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(j == player_y){
                if(board.map[i][beast_y] != 'X'){
                    board.beasts[monsterID].move = UP;
                    return 0;
                }
                else if(board.map[beast_x][beast_y-1] != 'X'){
                    board.beasts[monsterID].move = LEFT;
                    return 0;
                }
            }
        }
        for(int i = beast_x-1, j = beast_y-1, k = 0; k < sight;i--,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(i == player_x){
                if(board.map[beast_x-1][beast_y] != 'X'){
                    board.beasts[monsterID].move = UP;
                    return 0;
                }
                else if(board.map[beast_x][beast_y-1]!= 'X'){
                    board.beasts[monsterID].move = LEFT;
                    return 0;
                }
            }
        }
        for(int i = beast_x-1, j = beast_y+1, k = 0; k < sight;i--,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(i == player_x){
                if(board.map[beast_x-1][beast_y] != 'X'){
                    board.beasts[monsterID].move = UP;
                    return 0;
                }
                else if(board.map[beast_x][j] != 'X'){
                    board.beasts[monsterID].move = RIGHT;
                    return 0;
                }
            }
        }
        for(int i = beast_x+1, j = beast_y+1, k = 0; k < sight;i++,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(i == player_x){
                if(board.map[beast_x+1][beast_y] != 'X'){
                    board.beasts[monsterID].move = DOWN;
                    return 0;
                }
                else if(board.map[beast_x][beast_y+1] != 'X'){
                    board.beasts[monsterID].move = RIGHT;
                    return 0;
                }
            }
        }
        for(int i = beast_x+1, j = beast_y-1, k = 0; k < sight;i++,k++){
            if(board.map[i][j] == 'X'){
                break;
            }
            if(i == player_x){
                if(board.map[beast_x+1][beast_y] != 'X'){
                    board.beasts[monsterID].move = DOWN;
                    return 0;
                }
                else if(board.map[beast_x][beast_y-1] != 'X'){
                    board.beasts[monsterID].move = LEFT;
                    return 0;
                }
            }
        }
    }
    return 1;
}
int FindA(uint32_t *x,uint32_t *y)
{
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS-1; j++){
            if(board.map[i][j] == 'A'){
                *x = j;
                *y = i;
                return 1;
            }
        }
    }
    return 0;
}

void* print(void *arg)
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
    
    pthread_t thread;
    
    while(1){
        draw_some_board();
        draw_some_map();
        refresh();
        pthread_create(&thread,NULL,makeCorrections,NULL);
        pthread_join(thread,NULL);
    }
    return NULL;
}

void* makeCorrections(void *arg){
    
    bool Playersteps[MAX_PLAYERS]={0,0,0,0};
    for(unsigned int i = 0 ; i < MAX_PLAYERS; i++){
        if(board.gamePlayers[i]){
            draw_players_board(i);
            if(kill(board.gamePlayers[i]->PID,0) != 0){
                board.map[board.gamePlayers[i]->x][board.gamePlayers[i]->y] = board.gamePlayers[i]->previousSign;
                char shm[30];
                sprintf(shm,"Player%u_%d",board.room->Info[i].ID,board.room->Info[i].PID);
                sem_destroy(&board.gamePlayers[i]->continueGame);
                sem_destroy(&board.gamePlayers[i]->move);
                munmap(board.gamePlayers[i],sizeof(struct player_t));
                board.gamePlayers[i] = NULL;
                shm_unlink(shm);
                board.room->Info[i].ID = 255;
                board.room->Info[i].PID = 0;
                board.room->Info[i].action = NOTHING;
            }
        }
    }
        
    for(unsigned int i = 0; i < MAX_PLAYERS ; i++){
        if(board.gamePlayers[i]){
            if(sem_trywait(&board.gamePlayers[i]->move) == 0){
                Playersteps[i] = true;
                int x2 = board.gamePlayers[i]->x,y2 = board.gamePlayers[i]->y;
                if(board.gamePlayers[i]->bush_trap == 0){
                    if(board.gamePlayers[i]->type == BOT){
                        for(int z = 0; z < MAX_BEASTS; z++){
                            if(board.beasts[z].doesItExist == true){
                                int beast_x = board.beasts[z].beast_x, beast_y = board.beasts[z].beast_y;
                                int player_x = board.gamePlayers[i]->x, player_y = board.gamePlayers[i]->y;
                                
                                if((player_x <= (beast_x + 2) && player_x >= (beast_x - 2)) &&(player_y <= (beast_y + 2) && player_y >= (beast_y- 2))){
                                    if(beast_x < player_x){
                                        board.gamePlayers[i]->nextMove = DOWN;
                                    }
                                    else if(beast_x > player_x){
                                        board.gamePlayers[i]->nextMove = UP;
                                    }
                                    else if(player_y > beast_y){
                                        board.gamePlayers[i]->nextMove = RIGHT;
                                    }
                                    else{
                                        board.gamePlayers[i]->nextMove = LEFT;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    if(board.gamePlayers[i]->previousSign == board.gamePlayers[i]->ID + '0'){
                        board.map[x2][y2] = '#';
                    }
                    else{
                        board.map[x2][y2] = board.gamePlayers[i]->previousSign ;
                    }
                    if(board.gamePlayers[i]->nextMove == UP && board.map[x2 - 1][y2] != 'X'){
                        board.gamePlayers[i]->x = --x2;
                        board.rounds++;
                    }
                    else if(board.gamePlayers[i]->nextMove == DOWN && board.map[x2 + 1][y2] != 'X'){
                        board.gamePlayers[i]->x = ++x2;
                        board.rounds++;
                    }
                    else if(board.gamePlayers[i]->nextMove == RIGHT && board.map[x2][y2 + 1] != 'X'){
                        board.gamePlayers[i]->y = ++y2;
                        board.rounds++;
                    }
                    else if(board.gamePlayers[i]->nextMove == LEFT && board.map[x2][y2 - 1] != 'X'){
                        board.gamePlayers[i]->y = --y2;
                        board.rounds++;
                    }
                }
                board.gamePlayers[i]->bush_trap = 0;
                switch(board.map[x2][y2]){
                    case 'c':
                    board.gamePlayers[i]->pocket_money += 1;
                    break;
                case 't':
                    board.gamePlayers[i]->pocket_money += 10;
                    break;
                case 'T':
                    board.gamePlayers[i]->pocket_money += 50;
                    break;
                case 'D':
                    board.gamePlayers[i]->pocket_money += board.treasures[x2][y2];
                    board.treasures[x2][y2] = 0;
                    break;
                case 'A':
                    board.gamePlayers[i]->total_money += board.gamePlayers[i]->pocket_money;
                    board.gamePlayers[i]->pocket_money = 0;
                    break;
                case '#':
                        board.gamePlayers[i]->bush_trap = 1;
                        board.gamePlayers[i]->previousSign = '#';
                    break;
                }
                if(board.map[x2][y2] == 'c' || board.map[x2][y2] == 't' || board.map[x2][y2] == 'T' || board.map[x2][y2] == 'D'){
                    board.gamePlayers[i]->previousSign = ' ';
                }
                else{
                    board.gamePlayers[i]->previousSign = board.map[x2][y2];
                }
                board.map[x2][y2] = board.gamePlayers[i]->ID + '0';
                draw_players_board(i);
                board.gamePlayers[i]->nextMove = NOWHERE;
                board.gamePlayers[i]->rounds = board.rounds;
            }
        }
    }
    for(unsigned int i = 0; i < MAX_PLAYERS; i++){
        for(unsigned int j = i+1;j < MAX_PLAYERS;++j){
            if(board.gamePlayers[i] && board.gamePlayers[j]){
                if(board.gamePlayers[i]->x == board.gamePlayers[j]->x && board.gamePlayers[i]->y == board.gamePlayers[j]->y){
                    board.deadPlayers[i] = true;
                    board.deadPlayers[j] = true;
                    board.map[board.gamePlayers[i]->x][board.gamePlayers[i]->y] = 'D';
                    board.treasures[board.gamePlayers[i]->x][board.gamePlayers[i]->y] = board.gamePlayers[i]->pocket_money+board.gamePlayers[j]->pocket_money;
                }
            }
        }
    }
    
    for(unsigned int i = 0; i < MAX_BEASTS; i++){
        if(board.beasts[i].doesItExist == true){
            sem_post(&board.beasts[i].waitTillGetSignal);
            sem_post(&board.beasts[i].waitTillGetSignal);
            sem_wait(&board.beasts[i].CheckBeastMove);
            for(unsigned int j = 0; j < MAX_PLAYERS; j++){
                if(board.gamePlayers[j]){
                    if(board.gamePlayers[j]->x == (uint32_t)board.beasts[i].beast_x && board.gamePlayers[j]->y == (uint32_t)board.beasts[i].beast_y && board.deadPlayers[j] == false){
                        board.deadPlayers[j] = true;
                        board.map[board.gamePlayers[j]->x][board.gamePlayers[j]->y] = 'D';
                        board.treasures[board.gamePlayers[j]->x][board.gamePlayers[j]->y] += board.gamePlayers[j]->pocket_money;
                        pthread_cancel(board.beasts[i].beast);
                        board.beasts[i].doesItExist = false;
                        sem_destroy(&board.beasts[i].waitTillGetSignal);
                        sem_destroy(&board.beasts[i].CheckBeastMove);
                        break;
                    }
                }
            }
        }
    }
    
    for(unsigned int i = 0; i < MAX_PLAYERS; i++){
        if(board.deadPlayers[i]){
            board.gamePlayers[i]->x = board.gamePlayers[i]->spawn_x;
            board.gamePlayers[i]->y = board.gamePlayers[i]->spawn_y;
            draw_players_board(i);
            board.gamePlayers[i]->previousSign = ' ';
            board.map[board.gamePlayers[i]->x][board.gamePlayers[i]->y] = board.gamePlayers[i]->ID + '0';
            board.gamePlayers[i]->pocket_money = 0;
            board.gamePlayers[i]->deaths += 1;
            board.gamePlayers[i]->bush_trap = 0;
            board.deadPlayers[i] = false;
        }
    }
    for(unsigned int k = 0; k < MAX_PLAYERS; k++){
        if(Playersteps[k]){
            sem_post(&board.gamePlayers[k]->continueGame);
        }
    }
    
    return NULL;
}

void* welcomePlayer(void *arg)
{
    while(1){
        pthread_mutex_lock(&board.second_one);
        sem_wait(&board.room->semaphores.join);
        for(unsigned int i = 0; i < MAX_PLAYERS; i++){
            if(board.room->Info[i].action == JOIN_GAME){
                char shm[30];
                sprintf(shm,"Player%u_%d",board.room->Info[i].ID,board.room->Info[i].PID);
                int fd = shm_open(shm,O_CREAT | O_RDWR,0600);
                if(fd == -1){
                    kill(board.room->Info[i].PID,1);
                    board.room->Info[i].action = NOTHING;
                    sem_post(&board.room->semaphores.waitForMoment);
                    sem_post(&board.room->semaphores.askToDo);
                    return NULL;
                }

                int result = ftruncate(fd,sizeof(struct player_t));
                if(result == -1){
                    kill(board.room->Info[i].PID,1);
                    board.room->Info[i].action = NOTHING;
                    sem_post(&board.room->semaphores.waitForMoment);
                    sem_post(&board.room->semaphores.askToDo);
                    shm_unlink(shm);
                    return NULL;
                }
                
                board.gamePlayers[i] = (struct player_t*)mmap(NULL,sizeof(struct player_t),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
                if(board.gamePlayers[i] == NULL){
                    kill(board.room->Info[i].PID,1);
                    board.room->Info[i].action = NOTHING;
                    sem_post(&board.room->semaphores.waitForMoment);
                    sem_post(&board.room->semaphores.askToDo);
                    shm_unlink(shm);
                    return NULL;
                }
                
                uint32_t x = rand()%ROWS,y = rand()%COLS;
                while(board.map[x][y] != ' '){
                    x = rand()%ROWS;
                    y = rand()%COLS;
                }

                FindA(&board.gamePlayers[i]->campsite_x,&board.gamePlayers[i]->campsite_y);
                board.gamePlayers[i]->nextMove = NOTHING;
                board.gamePlayers[i]->pocket_money = 0;
                board.gamePlayers[i]->total_money = 0;
                board.gamePlayers[i]->bush_trap = 0;
                board.gamePlayers[i]->deaths = 0;
                board.gamePlayers[i]->x = x;
                board.gamePlayers[i]->y = y;
                board.gamePlayers[i]->spawn_x = x;
                board.gamePlayers[i]->spawn_y = y;
                board.gamePlayers[i]->ID = board.room->Info[i].ID;
                board.gamePlayers[i]->PID = board.room->Info[i].PID;
                board.gamePlayers[i]->rounds = board.rounds;
                board.gamePlayers[i]->ServerPID = board.ServerPID;
                board.gamePlayers[i]->type = board.room->Info[i].type;
                board.gamePlayers[i]->previousSign = ' ';
                board.map[x][y] = board.gamePlayers[i]->ID + '0';
                draw_players_board(i);
                sem_init(&board.gamePlayers[i]->move,1,0);
                sem_init(&board.gamePlayers[i]->continueGame,1,1);
                board.room->Info[i].action = CONTINUE;
                sem_post(&board.room->semaphores.askToDo);
                sem_post(&board.room->semaphores.waitForMoment);
                
            }
        }
        pthread_mutex_unlock(&board.second_one);
    
    }
    return NULL;
}
void* goodbyePlayer(void *arg)
{
    while(1){
        pthread_mutex_lock(&board.third_one);
        sem_wait(&board.room->semaphores.exit);
        for(unsigned int i = 0; i < MAX_PLAYERS; i++){
            if(board.room->Info[i].action == EXIT_GAME){
                board.map[board.gamePlayers[i]->x][board.gamePlayers[i]->y] = board.gamePlayers[i]->previousSign;
                uint8_t temp1 = board.room->Info[i].ID;
                pid_t temp2 = board.room->Info[i].PID;
                sem_destroy(&board.gamePlayers[i]->move);
                sem_destroy(&board.gamePlayers[i]->continueGame);
                munmap(board.gamePlayers[i],sizeof(struct player_t));
                board.gamePlayers[i] = NULL;
                char shm[30];
                sprintf(shm,"Player%u_%d",temp1,temp2);
                shm_unlink(shm);
                board.room->Info[i].type = BOT;
                board.room->Info[i].PID = 0;
                board.room->Info[i].ID = 255;
                board.room->Info[i].action = NOTHING;
                sem_post(&board.room->semaphores.askToDo);
                sem_post(&board.room->semaphores.waitForMoment);
                
            }
        }
        pthread_mutex_unlock(&board.third_one);
    
    }
    return NULL;
}

void* pressTheButton(void *arg)
{
    while(1){
        int a = getch(),x,y;
        switch(a){
            case 'q':
            case 'Q':
            sem_post(&end);
            return NULL;
            case 'c':
                pthread_mutex_lock(&board.only_one);
                x = rand()%ROWS,y = rand()%COLS;
                while(board.map[x][y] != ' '){
                    x = rand()%ROWS;
                    y = rand()%COLS;
                }
                board.map[x][y] = 'c';
                pthread_mutex_unlock(&board.only_one);
                break;
            case 't':
                pthread_mutex_lock(&board.only_one);
                x = rand()%ROWS,y = rand()%COLS;
                while(board.map[x][y] != ' '){
                    x = rand()%ROWS;
                    y = rand()%COLS;
                }
                board.map[x][y] = 't';
                pthread_mutex_unlock(&board.only_one);
                break;
            case 'T':
                pthread_mutex_lock(&board.only_one);
                x = rand()%ROWS,y = rand()%COLS;
                while(board.map[x][y] != ' '){
                    x = rand()%ROWS;
                    y = rand()%COLS;
                }
                board.map[x][y] = 'T';
                pthread_mutex_unlock(&board.only_one);
                break;
            case 'b':
            case 'B':
                if(board.maxBeasts >= MAX_BEASTS) break;
                pthread_mutex_lock(&board.only_one);
                for(unsigned int i = 0; i < MAX_BEASTS; i++){
                    if(board.beasts[i].doesItExist == false){
                        sem_init(&board.beasts[i].waitTillGetSignal,0,0);
                        sem_init(&board.beasts[i].CheckBeastMove,0,0);
                        x = rand()%ROWS,y=rand()%COLS;
                        while(board.map[x][y] != ' '){
                            x = rand()%ROWS;
                            y = rand()%COLS;
                        }
                        board.map[x][y]='*';
                        board.beasts[i].beast_x = x;
                        board.beasts[i].beast_y = y;
                        board.beasts[i].previousSign = ' ';
                        board.beasts[i].doesItExist = true;
                        board.maxBeasts++;
                        if(JustOnce == 0){
                            JustOnce = 1;
                            sem_post(&beastSem);
                        }
                        break;
                    }
                }
                pthread_mutex_unlock(&board.only_one);
                break;
        }
    }
    return NULL;
}
void* move_beast(void* arg)
{
    sem_wait(&beastSem);
    while(1){
        pthread_mutex_lock(&board.fourth_one);
        for(int id = 0; id < board.maxBeasts; id++){
            sem_wait(&board.beasts[id].waitTillGetSignal);
            board.beasts[id].move = NOTHING;
            board.beasts[id].hasMoved = false;
            for(unsigned int k = 0; k < MAX_PLAYERS; k++){
                if(board.gamePlayers[k]){
                    int result = chase(k,id);
                    if(result == 0){
                        board.beasts[id].hasMoved = true;
                    }
                }
                if(board.beasts[id].hasMoved)
                    break;
            }
            if(board.beasts[id].hasMoved == false){
                board.beasts[id].rand_dir = rand()%4;
                switch(board.beasts[id].rand_dir){
                    case 0:
                        if(board.map[board.beasts[id].beast_x][board.beasts[id].beast_y-1] != 'X'){
                            board.beasts[id].move = LEFT;
                        }
                        break;
                    case 1:
                        if(board.map[board.beasts[id].beast_x-1][board.beasts[id].beast_y] != 'X'){
                            board.beasts[id].move = UP;
                        }
                        break;
                    case 2:
                        if(board.map[board.beasts[id].beast_x][board.beasts[id].beast_y+1] != 'X'){
                            board.beasts[id].move = RIGHT;
                        }
                        break;
                    case 3:
                        if(board.map[board.beasts[id].beast_x+1][board.beasts[id].beast_y] != 'X'){
                            board.beasts[id].move = DOWN;
                        }
                        break;
                }
            }
            
            board.map[board.beasts[id].beast_x][board.beasts[id].beast_y] = board.beasts[id].previousSign;
            switch(board.beasts[id].move){
                case UP:
                    board.beasts[id].beast_x -= 1;
                    break;
                case DOWN:
                    board.beasts[id].beast_x += 1;
                    break;
                case RIGHT:
                    board.beasts[id].beast_y += 1;
                    break;
                case LEFT:
                    board.beasts[id].beast_y -= 1;
                    break;
                default:
                    break;
            }
            board.beasts[id].previousSign = board.map[board.beasts[id].beast_x][board.beasts[id].beast_y];
            board.map[board.beasts[id].beast_x][board.beasts[id].beast_y] = '*';
            sem_post(&board.beasts[id].CheckBeastMove);
        }
        usleep(70 *10000);
        pthread_mutex_unlock(&board.fourth_one);
    }
    return NULL;
}