#include "LedControl.h" 
#include <LiquidCrystal.h>
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

LedControl lc = LedControl(12, 11, 10, 1);
#define SW_PIN 13
#define X_PIN A0
#define Y_PIN A1
#define center 0
#define left 1
#define up 2
#define right 4
#define down 3


bool firstGame = true;
int turns;
int offsetX;
int offsetY;
int switch_value;
int dir;
int xValue;
int yValue;
int score = -1;
int movement = 1100;
bool matrix[8][8];
bool started = false;
typedef struct{
  int x;
  int y;
}Coord;

Coord food;

void writeMatrix() {
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      if (food.x == i && food.y == j) {
        continue;
      }
      lc.setLed(0, i, j, false);
    }
  delay(25);
  
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, matrix[i][j]);
    }
}

void gameOver(){
  int skull[] = {0, 0, 1, 1, 1, 1, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 0,
  1, 1, 0, 1, 1, 0, 1, 1,
  1, 1, 0, 1, 1, 0, 1, 1,
  0, 1, 1, 1, 1, 1, 1, 0,
  0, 0, 1, 0, 0, 1, 0, 0,
  0, 0, 1, 0, 0, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 0, 0
  };
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      matrix[j][i] = skull[i*8+j];
  writeMatrix();
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Game over!");
  lcd.setCursor(4, 1);
  lcd.print("Score: ");
  lcd.setCursor(11, 1);
  lcd.print(score);
  analogWrite(9, 90);
  delay(5000);
  started = false;
  score = -1;
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      matrix[i][j] = 0;
  movement = 1100;
}

class Snake{
  private:
    Coord coord[65];
    int len;
    int dir;
  public:
    Snake() {
      coord[0].x = 1;
      coord[0].y = 3;
      coord[1].x = 0;
      coord[1].y = 3;
      len = 2;
      dir = right;
    }
    Coord getHead(){
      return coord[0];
    }
    bool isSnake(Coord point) {
      for (int i = 0; i < len; i++){
        if  (coord[i].x == point.x && coord[i].y == point.y) {
          return true;
        }
      }
      return false;
    }
    bool isSnake() {//Checks if the head is hitting some other part of himself
      for (int i = 1; i < len; i++){
        if  (coord[i].x == coord[0].x && coord[i].y == coord[0].y) {
          return true;
        }
      }
      return false;
    }
    void slither(){
      len++;
      for (int i = len-1; i > 0; i--) {
        coord[i].x = coord[i-1].x;
        coord[i].y = coord[i-1].y;
      }
      switch(dir) {
        case left:
          coord[0].x--;
          if (coord[0].x < 0) {
            coord[0].x += 8;
          }
          break;
        case up:
          coord[0].y++;
          if (coord[0].y >= 8) {
            coord[0].y -= 8;
          }
          break;
        case right:
          coord[0].x++;
          if (coord[0].x >= 8) {
            coord[0].x -= 8;
          }
          break;
        case down:
          coord[0].y--;
          if (coord[0].y < 0) {
            coord[0].y += 8;
          }
          break;
      }
      if (matrix[coord[0].x][coord[0].y] == true) {
        if (isSnake()) {
          gameOver();
        }
        spawnFood();
      } else {
        len--;
        matrix[coord[len].x][coord[len].y] = false;
        matrix[coord[0].x][coord[0].y] = true;
      }
    }
    
    void slither(int newDir) {
      if (newDir != center && dir + newDir != 5) {
        dir = newDir;
        turns = 0;
      }
      slither();
    }
    
    
};
Snake snake;


void spawnFood(){
  score++;
  while(true) {
    food.x = random(8);
    food.y = random(8);
    if(!snake.isSnake(food)) {
      break;
    }
  }
  matrix[food.x][food.y] = true;
  movement /= 1.1;
}


void setup() 
{
  randomSeed(analogRead(0));
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);
  Serial.begin(9600);

  
  
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(1, 0);
  lc.shutdown(0, false);
  lc.setIntensity(0,2);
  lc.clearDisplay(0);
  lcd.setCursor(3, 0);
  lcd.print("Wanna play?");
  lcd.setCursor(4, 1);
  lcd.print("Go right");
  pinMode(9, OUTPUT);
  analogWrite(9, 90);
}

void loop()
{
  switch_value = digitalRead(SW_PIN);
  xValue = analogRead(X_PIN);
  yValue = analogRead(Y_PIN);
  offsetX = xValue - 511;
  offsetY = yValue - 511;
  turns++;
  if (abs(offsetX) - abs(offsetY) > 0) {
    if (xValue < 400) {
      dir = left;
    } else if (xValue > 600) {
      dir = right;
    } else {
     dir = center;
    }
  } else if (abs(offsetX) - abs(offsetY) < 0) {
    if (yValue < 400) {
      dir = down;
    } else if (yValue > 600) {
      dir = up;
    } else {
      dir = center;
    }
  } else {
    return;
  }
  
  

  
  if (!started) {
    if (dir == right) {
      snake = Snake();
      started = true;
      lcd.clear();
      lcd.setCursor(3, 0);
      if (firstGame) {
//        firstGame = false;
        lcd.print("Wanna play?");
      } else {
        lcd.setCursor(3, 0);
        lcd.print("Game over!");
      }
      lcd.setCursor(8, 1);
      lcd.print("==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print(" ==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("  ==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("   ==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("    ==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("     ==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("      ==");
      analogWrite(9, 90);
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("       =");
      analogWrite(9, 90);
      matrix[0][3] = true;
      writeMatrix();
      delay(1000);
      lcd.setCursor(8, 1);
      lcd.print("        ");
      analogWrite(9, 90);
      matrix[1][3] = true;
      if (firstGame) {
        firstGame = false;
        spawnFood();
      }
      writeMatrix();
      delay(1000);
      turns = 0;
    }
    return;
  }
  
  snake.slither(dir);
  writeMatrix();

  delay(movement);

  
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Score: ");
  lcd.setCursor(11, 0);
  lcd.print(score);
  lcd.setCursor(1, 1);
  if (turns > 10 && turns <= 20){
    lcd.print("Are you there?");
  }
  if (turns > 20){
    lcd.print("I feel lonely:(");
  }  
  analogWrite(9, 90);
  
}
