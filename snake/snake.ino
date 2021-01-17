#include <Vector.h>
#include "LedControl.h"

const int UP = 2;
const int RIGHT = 3;
const int DOWN = 4;
const int LEFT = 5;
LedControl lc=LedControl(12,10,11,1);

const long TICK_LENGTH_MILLIS = 400L;
const int MAX_SNAKE_LENGTH = 64;
int SNAKE_START_POS[2] = {3, 3};


void setup() {
  pinMode(UP, INPUT_PULLUP);
  pinMode(LEFT, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  Serial.begin(9600);

  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
}


char check_input() {
  if (digitalRead(UP) == LOW){return 'u';}
  else if (digitalRead(RIGHT) == LOW){return 'r';}
  else if (digitalRead(DOWN) == LOW){return 'd';}
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
  int food_x;
  int food_y;
  bool game_over;

public:
  Game(){
    snake_x[0] = SNAKE_START_POS[0];
    snake_y[0] = SNAKE_START_POS[1];
    snake_length = 1;
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

  void update_board(char d) {
    Serial.print("updating with d = ");
    Serial.print(d);
    Serial.print("\n");
    int head_x = snake_x[0];
    int head_y = snake_y[0];
    if (d == 'u'){head_y -= 1;}
    if (d == 'r'){head_x += 1;}
    if (d == 'd'){head_y += 1;}
    if (d == 'l'){head_x -= 1;}

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
    Serial.print("Snake_pos: ");
    for (int i=0; i<snake_length; i++){
      Serial.print(snake_x[i]);
      Serial.print(snake_y[i]);
      Serial.print(", ");
    }
    Serial.print("\n");
    
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
  char d = 'r';
  Serial.print(d);

  long start_time = millis();
  Game game = Game();

  while (true){
    Serial.print(millis());
    Serial.print(" ");
    Serial.print(start_time);
    
    Serial.print("tick: ");
    Serial.print(tick);
    Serial.print("\n");
    //Constantly check for button presses
    while (millis() - start_time < tick * TICK_LENGTH_MILLIS) {
      Serial.print(millis() - start_time);
      Serial.print("\n");
      char new_d = check_input();
      if (new_d != '-'){
        d = new_d;
      }
    }

    //Once the tick is over, update the game according to the last pressed button
    game.update_board(d);
    if (game.get_game_over()){break;}
    game.write_board();
    
    tick += 1;
  }
  Serial.print("Game over");
}
