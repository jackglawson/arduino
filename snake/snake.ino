#include "LedControl.h"

const int RIGHT = 3;
const int LEFT = 5;
LedControl lc=LedControl(12,10,11,1);

const long TICK_LENGTH_MILLIS = 400L;
const int MAX_SNAKE_LENGTH = 64;
int SNAKE_START_POS[2] = {3, 3};
char SNAKE_START_DIR = 'e';


void setup() {
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  Serial.begin(9600);

  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}


char check_input() {
  if (digitalRead(RIGHT) == LOW){return 'r';}
  else if (digitalRead(LEFT) == LOW){return 'l';}
  else {return '-';}
}


bool value_in_array(int* arr, int v, int n){
  // Check presence of v in arr up to index=n
  for (int i=0; i<n; i++){
    if (arr[i] == v){
      return true;
    }
  }
  return false;
}


class Game {
private: 
  int snake_x[MAX_SNAKE_LENGTH+1];
  int snake_y[MAX_SNAKE_LENGTH+1];
  int snake_length;
  char snake_dir;
  int food_x;
  int food_y;
  bool game_over;

public:
  Game(){
    snake_x[0] = SNAKE_START_POS[0];
    snake_y[0] = SNAKE_START_POS[1];
    snake_length = 1;
    snake_dir = SNAKE_START_DIR;
    randomise_food_pos();
    game_over = false;
  }

  bool snake_is_here(int x, int y){
    for (int i=0; i<snake_length; i++){
      if (snake_x[i] == x and snake_y[i] == y){
        return true;
      }
    }
    return false;
  }

  void randomise_food_pos(){
    do{
      food_x = random(0, 8);
      food_y = random(0, 8);
    }
    while (snake_is_here(food_x, food_y));
  }

  void update_direction(char instruction){
    // d is in {'n', 's', 'e', 'w'} (North, South, East or West)
    // Instruction is in {'l', 'r', '-'} (turn left, turn right, or don't turn)
    if (instruction == '-'){;}
    else if (instruction == 'l' and snake_dir == 'n'){snake_dir = 'w';}
    else if (instruction == 'l' and snake_dir == 's'){snake_dir = 'e';}
    else if (instruction == 'l' and snake_dir == 'e'){snake_dir = 'n';}
    else if (instruction == 'l' and snake_dir == 'w'){snake_dir = 's';}
    else if (instruction == 'r' and snake_dir == 'n'){snake_dir = 'e';}
    else if (instruction == 'r' and snake_dir == 's'){snake_dir = 'w';}
    else if (instruction == 'r' and snake_dir == 'e'){snake_dir = 's';}
    else if (instruction == 'r' and snake_dir == 'w'){snake_dir = 'n';} 
  }

  void update_board(char instruction) {
    update_direction(instruction);
    
    int head_x = snake_x[0];
    int head_y = snake_y[0];
    if (snake_dir == 'n'){head_y -= 1;}
    if (snake_dir == 'e'){head_x += 1;}
    if (snake_dir == 's'){head_y += 1;}
    if (snake_dir == 'w'){head_x -= 1;}

    // Check for collision with wall
    if (head_x < 0 or head_x > 7 or head_y < 0 or head_y > 7){
      snake_dies();
      return;
    }

    // Check for collision with self
    for (int i=0; i<snake_length; i++){
      for (int j=0; j<snake_length; j++){
        if (i==j){
          continue;
        }
        if (snake_x[i] == snake_x[j] and snake_y[i] == snake_y[j]){
          snake_dies();
          return;
        }
      }
    }

    // Move snake forwards
    for (int i=snake_length; i>0; i--){
      snake_x[i] = snake_x[i-1];
      snake_y[i] = snake_y[i-1];
    }
    
    snake_x[0] = head_x;
    snake_y[0] = head_y;

    // Check if snake has found food
    if (head_x == food_x and head_y == food_y){
      randomise_food_pos();
      snake_length++;
    }    
  }

  void write_board(){
    lc.clearDisplay(0);
    for (int i=0; i<snake_length; i++){
      lc.setLed(0, snake_y[i], snake_x[i], true);
    }
    lc.setLed(0, food_y, food_x, true);
  }

  void snake_dies(){
    game_over = true;
    for (int i=0; i<5; i++){
      lc.clearDisplay(0);
      delay(200);
      for (int row=0; row<8; row++){
        lc.setRow(0, row, B11111111);
      }
      delay(200);
    }
  }

  bool get_game_over() {return game_over;}
};


void loop() {
  long tick = 1;
  char new_instruction;
  char instruction;

  long start_time = millis();
  Game game = Game();

  while (true){
    //Constantly check for button presses
    while (millis() - start_time < tick * TICK_LENGTH_MILLIS) {
      new_instruction = check_input();
      if (new_instruction != '-') {
        instruction = new_instruction;
      }
    }

    //Once the tick is over, update the game according to the last pressed button
    game.update_board(instruction);
    instruction = '-';
    if (game.get_game_over()){break;}
    game.write_board();
    tick += 1;
  }
}
