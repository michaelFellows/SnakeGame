//Author: Michael Fellows
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int trophyTime;
time_t currentTimePlus1;

//Draws the borders for the snakepit
void drawSnakePit()
{
    mvhline(0, 1, '-', COLS-2);//draws top line
    mvhline(1,1,'#',COLS-2);//Draws # border top
    mvhline(LINES-1,1,'-',COLS-2);//draws bottom line
    mvhline(LINES-2,1,'#',COLS-2);//Draws # border bottom
    for(int i=1; i<LINES-1;i++)
    {
        mvaddch(i,0,'|');//Draws left line
        mvaddch(i,1,'#'); //Draws #border left
        mvaddch(i,COLS-1,'|');//Draws right line
        mvaddch(i,COLS-2,'#');//Draws #border right
    }
    mvaddch(0,0,'+');//Draws upperleft corner
    mvaddch(0,COLS-1,'+');//Draws upperRight corner
    mvaddch(LINES-1,0,'+');//Draws lowerLeft corner
    mvaddch(LINES-1,COLS-1,'+');//Draws lowerRight corner
}

//Responsible for moving the snake and each tail piece
void moveSnake(int *snake, int snakeLength, int xDirection, int yDirection, double speed)
{
    int xTemp = 0, yTemp = 0;
    int xCur = *snake, yCur = *(snake+1);
    int xNext = xCur + xDirection, yNext = yCur + yDirection;
    *(snake) = xNext;
    *(snake+1) = yNext;
    mvprintw(yNext,xNext,"@");//Print SnakeHead
    /*
    Loop that assigns the x and y coordinates to each tail piece
    for the entire length of the snake
    */
    for(int i = 2; i < snakeLength+2; i += 2)
    {
        xTemp = xCur, yTemp = yCur;//setting temp vars to Cur vars
        xCur = *(snake+i), yCur = *(snake+1+i);//setting cur vars to the x and y values of current index
        xNext = xTemp, yNext = yTemp;//updating next values to the temp values
        *(snake+i) = xNext, *(snake+1+i) = yNext;//assigning each index of the snake to its corresponding value
        mvprintw(yNext,xNext,"o");//Print snake tailpiece
    }
    //Used to stop the snake from going faster vertically than it does horizontally
    if(yDirection == 1 || yDirection == -1)//if snake is going up or down
    {
        usleep(50000/speed);
    }
usleep(100000/speed);
}

/*
Takes in pointers to input, dirX, and dirY
It checks for which key is pressed and then
Changes the direction values of the snake accordingly
Blocks player from turning in the opposite direction
*/
void getInput(char input, int *dirX, int *dirY, bool *gameRun)
{
    if (input == 'w' || input == 'W') 
    {
        if(*dirY == 1 && *dirX == 0)
        {
            *gameRun = false;
        }
        *dirY = -1, *dirX = 0;
    }
    if (input == 's' || input == 'S') 
    {
        if(*dirY != -1 && *dirX != 0)
        {
            *gameRun = false;
        }
        *dirY = 1, *dirX = 0;
    }
    if (input == 'a' || input == 'A') 
    {
        if(*dirY != 0 && *dirX != 1)
        {
            *gameRun = false;
        }
        *dirY = 0, *dirX = -1;
    }
    if (input == 'd' || input == 'D') 
    {
        if(*dirY != 0 && *dirX != -1)
        {
            *gameRun = false;
        }
        *dirY = 0, *dirX = 1;
    }
}

//Checks if keyboard is hit
bool kbhit(void) 
{
   char ch;
   bool check;
   nodelay(stdscr, true);
   noecho();
   ch = getch();
   if(ch == ERR)//No keypress
   {
      check = false;
   }
   else//Keypress
   {
      check = true;
      ungetch(ch);
   }
   echo();
   nodelay(stdscr, false);
   return(check);
}

//Checks if snake collides with wall or tail
bool checkCollision(int *snake, int snakeLength, bool *gameRun)
{
    int xPOS = *snake, yPOS = *(snake+1); //points to the x and y values of an index of the 2d snake array
    if((xPOS <= 1) || (xPOS >= COLS-2))
    {
        *gameRun = false;
        return true;
    }
    if((yPOS <= 1)|| (yPOS >= LINES-2))
    {
        *gameRun = false;
        return true;
    }
    for(int i = 2; i <= snakeLength*2+2; i+=2)
    {
        if (xPOS == *(snake+i) && yPOS == *(snake+1+i))
        { 
            *gameRun = false;
            return true;
        }
    }
    *gameRun = true;
    return false;
}

//Generates a trophy at a random spot in the snakepit
void makeTrophy(int* trophyX, int* trophyY, int *trophyTime, bool *collected, bool *expired)
{
    srand(time(0));//changes the seed each time 
    if(*collected || *expired)
    {
        *trophyX = (rand() % ((COLS-3)-2)) + 2; //Upper Bound COLS-3, Lower Bound 2
        *trophyY = (rand() % ((LINES-3)-2)) + 2; //Upper Bound LINES-3, Lower Bound 2
        *trophyTime = (rand() % 9) + 1;//generate random int from 1-9
        *collected = false;
        *expired = false;
    }
    char trophyChar = *trophyTime + '0'; //Converts the trophyTime int to a char to add it to the window
    mvaddch(*trophyY, *trophyX, trophyChar);
}

/*
Updates the eaten value to true if the head position
and trophy position are equal
Also allows the snake to grow and increases the speed modifier
*/
void collectTrophy(int trophyX, int trophyY, bool *collected, int *snakeLength, double *speed, int *snake)
{
    int xPOS = *snake, yPOS = *(snake+1);//Pointers to the x and y positions of the head of the snake
    if(xPOS == trophyX && yPOS == trophyY)
    {
        *collected = true;
        *snakeLength += trophyTime*2;//Length increases for corresponding number on the trophy
        *speed += .01 * trophyTime;//speed increases proportional to the length of the snake
    }
}

//Chooses a random direction to start the game
void chooseDirection(int *dirX, int *dirY)
{
    srand(time(0));//changes the seed each time 
    int direction =  (rand() % 4);
    switch(direction)
    {
        case 0://UP
            *dirX = 0;
            *dirY = -1;
            break;
        case 1://DOWN
            *dirX = 0;
            *dirY = 1;
            break;
        case 2://LEFT
            *dirX = -1;
            *dirY = 0;
            break;
        case 3://RIGHT
            *dirX = 1;
            *dirY = 0;
            break;
    }

}

//Gets the current time in seconds
time_t currentTime (void) 
{
    time_t seconds;
    seconds = time(NULL);
    return seconds;
}

/*
Checks how many seconds have
elapsed and updates the expired
value if needed
*/
int checkTime(bool *expired) 
{
    if (currentTime() == currentTimePlus1) {
        trophyTime--;
        currentTimePlus1++;
        if(trophyTime == 0)
        {
            *expired = true;
        }
    }
    return 0;
}

int main() 
{
    initscr();//Starts ncurses	
    curs_set(0);
    noecho();//prevents input from showing up in terminal
    char input;//which key the player is pressing
    int dirX = 0, dirY = 0;//initial direction modifiers
    int trophyX = 0, trophyY = 0;
    bool collected = true, gameRun = true, winGame = false, expired = true;
    double speed = 2;
    int winningLength = (2*LINES + 2*COLS)/2;
    int snake[winningLength][2];//2d array to represent the snake
    int snakeLength = 6;//Length of Snake, creates 3 tailpieces 
    for (int i = 0; i < winningLength; i++) 
    {
        for (int j = 0; j < 2; j++) 
        {
            snake[i][j] = 0;
        }
    }
    snake[0][0] = COLS/2; //Initial x coordinate of head
    snake[0][1] = LINES/2; //Initial y coordinate of head
    char splashScreen[] = 
    " .o88b. d8b   db  .d8b.  db   dD d88888b \nd8P  Y8 888o  88 d8' `8b 88 ,8P' 88'     \n8P      88V8o 88 88ooo88 88,8P   88ooooo \n8b      88 V8o88 88~~~88 88`8b   88~~~~~ \nY8b  d8 88  V888 88   88 88 `88. 88.     \n `Y88P' VP   V8P YP   YP YP   YD Y88888P ";
    char prompt[] = "Press any key to start.";
    mvprintw(0,0,splashScreen);
    mvprintw(7,0,prompt);
    start_color();
    init_pair(2, COLOR_CYAN, COLOR_BLUE);
    attron(COLOR_PAIR(2));
    chooseDirection(&dirX, &dirY);
    currentTimePlus1 = currentTime() + 1;
    refresh();
    getch();
    //Game Loop
    while (!checkCollision(&snake[0][0], snakeLength, &gameRun))
    {
        erase();//clears screen
        drawSnakePit();//Draws the snakepit
        checkTime(&expired);//Checks the elapsed time and updates expired status if needed
        makeTrophy(&trophyX, &trophyY, &trophyTime, &collected, &expired);//Creates a trophy with a random value 1-9 and random x,y coordinates
        moveSnake(&snake[0][0], snakeLength, dirX, dirY, speed);//moves the snake and all tail pieces by looping through the 2D array
        collectTrophy(trophyX, trophyY, &collected, &snakeLength, &speed, &snake[0][0]);//Increases snake length and speed if trophy is collected
        refresh();//Updates the screen
        if (kbhit())//Checks for keypress and sets the direction accordingly
        {
            input = getch();
            getInput(input, &dirX, &dirY, &gameRun);
        }
        if(snakeLength/2 >= winningLength)
        {
            winGame = true;
            break;
        }
    }
    erase();
    if(winGame)
    {
        mvprintw(LINES/2,COLS/2,"YOU WIN\n");
    }
    else
    {
        mvprintw(LINES/2,COLS/2,"YOU LOSE");
    }
    refresh();
    noecho();
    getch();   
    getch();    
    endwin();
}