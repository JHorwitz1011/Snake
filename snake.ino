//We always have to include the library
#include "LedControl.h"

//customizable board layout variables
const int width = 8;
const int height = 8;

//pin initialization
const int upPin = 53;
const int downPin = 23;
const int leftPin = 24;
const int rightPin = 25;

//library access
LedControl lc = LedControl(8, 9, 10, 1);

//determining ticker timer
unsigned long time1;
int delayT = 800;


//movement logic
int direct = 0;
int headW = 0;
int headH = 0;

//board variables
boolean apple[width][height];
int body[width][height];
int bodyLength = 2;


void setup() {

  //pin initialization
  pinMode(upPin, INPUT_PULLUP);
  pinMode(downPin, INPUT_PULLUP);
  pinMode(leftPin, INPUT_PULLUP);
  pinMode(rightPin, INPUT_PULLUP);

  //wakeup sequence
  lc.shutdown(0, false);
  lc.setIntensity(0, 0);
  lc.clearDisplay(0);

  Serial.begin(9600);

  //starting position
  body[0][0] = 1;

  //random logic
  randomSeed(analogRead(A0));

  //initializes random numbers

  placeApple();
  updateDisplay();
  time1 = millis();
}

void updateDisplay() {
  for (int wid = 0; wid < width; wid++) {
    for (int hei = 0; hei < height; hei++) {
      if (apple[wid][hei]) {
        lc.setLed(0, wid, hei, apple[wid][hei]);
      }
      else {
        lc.setLed(0, wid, hei, body[wid][hei]);
      }
    }
  }
}

int determineDirection() {
  if (digitalRead(upPin) == LOW && direct != 2) {
    direct = 1;
    //Serial.println("up");
  }
  else if (digitalRead(downPin) == LOW && direct != 1) {
    direct = 2;
    //Serial.println("down");
  }
  else if (digitalRead(leftPin) == LOW && direct != 4) {
    direct = 3;
    //Serial.println("left");
  }
  else if (digitalRead(rightPin) == LOW && direct != 3) {
    direct = 4;
    //Serial.println("right");
  }
}

void moveHead() {
  switch (direct) {
    // UP
    case 1:
      headH++;
      Serial.println("UP");
      break;

    // DOWN
    case 2:
      headH--;
      Serial.println("DOWN");
      break;

    // LEFT
    case 3:
      headW--;
      Serial.println("LEFT");
      break;

    // RIGHT
    case 4:
      headW++;
      Serial.println("RIGHT");
      break;
  }
  updateDisplay();
  //check if you ran into a wall
  if (headH > height || headW > width || headH < 0 || headW < 0)
    gameOver();

  //check if you ran into yourself
  if (body[headW][headH] > 0)
    gameOver();

  //check for apple
  if (apple[headW][headH]) {
    for (int wid = 0; wid < width; wid++) {
      for (int hei = 0; hei < height; hei++) {
        if (body[wid][hei] > 1)
          body[wid][hei] += 2;
      }
    }
    bodyLength += 2;
    if (delayT > 300)
      delayT -= 100;
    placeApple();
  }

  //head value installed
  body[headW][headH] = bodyLength;
}

void placeApple() {
  for (int wid = 0; wid < width; wid++) {
    for (int hei = 0; hei < height; hei++) {
      apple[wid][hei] = false;
    }
  }

  int widT = random(0, width);
  int heiT = random(0, height);
  do {
    widT = random(0, width);
    heiT = random(0, height);
    apple[widT][heiT] = true;
  } while (body[widT][heiT] != 0);
}

void loop() {

  do {
    determineDirection();
  } while ((millis() - time1) < delayT || direct == 0);

  //move direction
  moveHead();

  //decay body
  for (int wid = 0; wid < width; wid++) {
    for (int hei = 0; hei < height; hei++) {
      if (body[wid][hei] > 0)
        body[wid][hei]--;
    }
  }

  updateDisplay();
  time1 = millis();
}

void gameOver() {
  for (int wid = 0; wid < width; wid++) {
    for (int hei = 0; hei < height; hei++) {
      lc.setLed(0, wid, hei, false);
    }
  }

  Serial.println("GAMEOVER");
  while (true) {
    delay(100);
  }
}
