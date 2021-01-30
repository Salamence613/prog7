#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int height;
int width;
char controls[] = {'a','d','w','s','q'};
char defaultIcon;
char poweredIcon;
int r;
int c;
int items = 0;
int score = 0;
int poweredTurns = 0;

//functions used for setting up board
char** readFileMakeBoard(const char* filename); //reads the file, defines global vars, makes board
char** makeEmptyBoard(void); //makes an empty board
void placeObj(char** board, char indicator, int row, int col); //places an object

//functions used for correct formatting and getting input
void printScore(void); //prints the score
void printItemsRemaining(void); //prints items remaining
void printBoard(char** board); //prints the board
char getInput(void); //prompts user for input and returns it

//functions used to determine what to do with input
int update(char** board, char input); //tries to update board based on input, calls collision update
int collisionUpdate(char** board, int row, int col); //actually updates the board
void replace(char** board, int row, int col); //replaces board[row][col] with correct icon and replaces prev icon location with space

//functions for ending the game
void quit(void); //prints stuff for quit
void win(void); //prints stuff for win
void die(char** board); //prints stuff for die

int playGame(const char* filename)
{
  if(filename == NULL)
  {
    return 0;
  }

  char** board = readFileMakeBoard(filename);

  int condition;

  do
  {
    printScore();
    printItemsRemaining();
    printBoard(board);
    char input = getInput();
    condition = update(board, input);
  }
  while (!condition);

  if(condition == 1)
  {
    quit();
  }
  else if(condition == 2)
  {
    win();
  }
  else if(condition == 3)
  {
    die(board);
  }
  else
  {
    printf("oop something went wrong lmao\n");
  }

  for(int i = 0; i < height; i++)
  {
    free(board[i]);
  }
  free(board);
  return 1;
}

char** readFileMakeBoard(const char* filename)
{
  FILE* fp = fopen(filename, "r");

  fscanf(fp, "%d", &height);
  fscanf(fp, "%d", &width);
  fseek(fp, 1, SEEK_CUR);

  char** board = makeEmptyBoard();

  // char buf[8];
  // fgets(buf, 8, fp);
  //
  // if(buf[0] != '\n')
  // {
  //   int i = 0;
  //   for(int j = 0; j < 9; j += 2)
  //   {
  //     controls[i] = buf[j];
  //     i++;
  //   }
  // }

  char ch;
  fscanf(fp, "%c", &ch);

  if(ch != '\n')
  {
    controls[0] = ch;
    for(int i = 1; i < 5; i++)
    {
      fscanf(fp, " %c", &controls[i]);
    }
  }

  fscanf(fp, " %c", &defaultIcon);
  fscanf(fp, " %c", &poweredIcon);
  fscanf(fp, "%d", &r);
  fscanf(fp, "%d", &c);

  board[r][c] = defaultIcon;

  char indicator;
  int objr;
  int objc;

  while(fscanf(fp, " %c %d %d", &indicator, &objr, &objc) == 3)
  {
    placeObj(board, indicator, objr, objc);
  }

  fclose(fp);
  return board;
}

char** makeEmptyBoard(void)
{
  char** board = malloc(height * sizeof(char*));
  for(int i = 0; i < height; i++)
  {
    board[i] = malloc(width * sizeof(char));
    for(int j = 0; j < width; j++)
    {
      board[i][j] = ' ';
    }
  }
  return board;
}

void placeObj(char** board, char indicator, int row, int col)
{
  if(indicator == 'B')
  {
    for(int i = 0; i < 4; i++)
    {
      for(int j = 0; j < 6; j++)
      {
        if(i > 1 && (j > 1 && j < 4))
        {
          board[row + i][col + j] = '&';
        }
        else
        {
          board[row + i][col + j] = '-';
        }
      }
    }
  }

  if(indicator == 'E')
  {
    board[row][col] = 'X';
  }

  if(indicator == 'I')
  {
    board[row][col] = '$';
    items++;
  }

  if(indicator == 'P')
  {
    board[row][col] = '*';
  }
}

void printScore(void)
{
  printf("Score: %d\n", score);
}

void printItemsRemaining(void)
{
  printf("Items remaining: %d\n", items);
}

void printBoard(char** board)
{
  for(int x = 0; x < width + 2; x++)
  {
    printf("*");
  }
  printf("\n");

  for(int i = 0; i < height; i++)
  {
    printf("*");
    for(int j = 0; j < width; j++)
    {
      printf("%c", board[i][j]);
    }
    printf("*\n");
  }

  for(int x = 0; x < width + 2; x++)
  {
    printf("*");
  }
  printf("\n");
}

char getInput(void)
{
  char input;
  printf("Enter input: ");
  scanf(" %c", &input);
  return input;
}

int update(char** board, char input)
{
  if(input == controls[0])
  {
    return collisionUpdate(board, r, c - 1);
  }
  else if(input == controls[1])
  {
    return collisionUpdate(board, r, c + 1);
  }
  else if(input == controls[2])
  {
    return collisionUpdate(board, r - 1, c);
  }
  else if(input == controls[3])
  {
    return collisionUpdate(board, r + 1, c);
  }
  else if(input == controls[4])
  {
    return 1;
  }
  else
  {
    printf("Invalid input.\n");
    return update(board, getInput());
  }
}

int collisionUpdate(char** board, int row, int col)
{
  if(poweredTurns > 0)
  {
    poweredTurns--;

    if(poweredTurns == 0)
    {
      board[r][c] = defaultIcon;
    }
  }

  if(row < 0 || row >= height || col < 0 || col >= width)
  {
    return 0;
  }

  char ch = board[row][col];

  if(ch == '-' || ch == '&')
  {
    return 0;
  }

  if(ch == '$')
  {
    score++;
    items--;

    if(items == 0)
    {
      return 2;
    }

    replace(board, row, col);
    return 0;
  }

  if(ch == 'X')
  {
    if(poweredTurns > 0)
    {
      score++;
      replace(board, row, col);
      return 0;
    }

    board[r][c] = '@';
    replace(board, row, col);
    return 3;
  }

  if(ch == '*')
  {
    poweredTurns = 7;
    board[r][c] = poweredIcon;
    replace(board, row, col);
    return 0;
  }

  if(ch == ' ')
  {
    replace(board, row, col);
    return 0;
  }

  return -1;
}

void replace(char** board, int row, int col)
{
  board[row][col] = board[r][c];
  board[r][c] = ' ';
  r = row;
  c = col;
}

void quit(void)
{
  printf("You have quit.\n");
  printf("Final score: %d\n", score);
}

void win(void)
{
  printf("Congratulations! You have won.\n");
  printf("Final score: %d\n", score);
}

void die(char** board)
{
  printScore();
  printItemsRemaining();
  printBoard(board);
  printf("You have died.\n");
  printf("Final score: %d\n", score);
}
