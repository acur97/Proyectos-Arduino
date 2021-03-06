#include "UnoJoy.h"

boolean led = false;
int pinled1 = 13;
int pinled2 = A5;
int pinled3 = A3;
int pinled4 = A4;
int pinb = 12;
int pinb2 = 11;
int pinmode = 2;

void setup(){
  setupPins();
  setupUnoJoy();
  pinMode(pinled1, OUTPUT);
  pinMode(pinled2, OUTPUT);
  pinMode(pinled3, OUTPUT);
  pinMode(pinled4, OUTPUT);
  pinMode(pinb, INPUT_PULLUP);
  pinMode(pinb2, INPUT_PULLUP);
}

void loop(){
/////////////////////////////////////////////////////////////////////////////////////////////
 if (digitalRead(pinb) == LOW){
  pinmode = 2;
 }
 else if (digitalRead(pinb2) == LOW){
  pinmode = 1;
 }
 else {
  pinmode = 3;
 }
/////////////////////////////////////////////////////////////////////////////////////////////
  if (pinmode == 1){
    digitalWrite(pinled1, LOW);
    digitalWrite(pinled2, LOW);
    digitalWrite(pinled3, LOW);
    digitalWrite(pinled4, LOW);
  }
  else if (pinmode == 2){
    digitalWrite(pinled1, HIGH);
    digitalWrite(pinled2, HIGH);
    digitalWrite(pinled3, HIGH);
    digitalWrite(pinled4, HIGH);
    }
////////////////////////////////////////////////////////////////////////////////////////////
  else if (pinmode == 3){
///////////////////
    if (digitalRead(A1) == LOW){
      led = true;
      }
    else if (digitalRead(A0) == LOW){
      led = true;
      }
    else {
      led = false;
      }
  
    if (led == true){
      digitalWrite(pinled1, HIGH);
      }
    else {
      digitalWrite(pinled1, LOW);
      }
//////////////////////////////////////
    if (digitalRead(4) == LOW){
      led = true;
      }
    else if (digitalRead(5) == LOW){
      led = true;
      }
    else {
      led = false;
      }
  
    if (led == true){
      digitalWrite(pinled2, HIGH);
      }
    else {
      digitalWrite(pinled2, LOW);
      }
//////////////////////////////////////
    if (digitalRead(3) == LOW){
      led = true;
      }
    else if (digitalRead(2) == LOW){
      led = true;
      }
    else {
      led = false;
      }
  
    if (led == true){
      digitalWrite(pinled3, HIGH);
      }
    else {
      digitalWrite(pinled3, LOW);
      }
//////////////////////////////////////
    if (digitalRead(A2) == LOW){
      led = true;
      }
    else if (digitalRead(10) == LOW){
      led = true;
      }
    else {
      led = false;
      }
  
    if (led == true){
      digitalWrite(pinled4, HIGH);
      }
    else {
      digitalWrite(pinled4, LOW);
      }
  }
///////////////////////////////////////////////////////////

  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
}

void setupPins(void){

  for (int i = 2; i <= 10; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(A0, INPUT);
  digitalWrite(A0, HIGH);
  pinMode(A1, INPUT);
  digitalWrite(A1, HIGH);
  pinMode(A2, INPUT);
  digitalWrite(A2, HIGH);
}

dataForController_t getControllerData(void){

  dataForController_t controllerData = getBlankDataForController();

  controllerData.triangleOn = !digitalRead(2);
  controllerData.circleOn = !digitalRead(3);
  controllerData.squareOn = !digitalRead(4);
  controllerData.crossOn = !digitalRead(5);
  controllerData.dpadUpOn = !digitalRead(6);
  controllerData.dpadDownOn = !digitalRead(7);
  controllerData.dpadLeftOn = !digitalRead(8);
  controllerData.dpadRightOn = !digitalRead(9);
  controllerData.l1On = !digitalRead(10);
  controllerData.r1On = !digitalRead(A2);
  controllerData.selectOn = !digitalRead(A0);
  controllerData.startOn = !digitalRead(A1);
  
  return controllerData;
}
