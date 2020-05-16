
#include "UnoJoy.h"

void setup(){
  setupPins();
  setupUnoJoy();
}

void loop(){

  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
}

void setupPins(void){

  for (int i = 2; i <= 12; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(A4, INPUT);
  digitalWrite(A4, HIGH);
  pinMode(A5, INPUT);
  digitalWrite(A5, HIGH);
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
  controllerData.l3On = !digitalRead(10);
  controllerData.r3On = !digitalRead(11);
  controllerData.selectOn = !digitalRead(12);
  controllerData.startOn = !digitalRead(A4);
  controllerData.homeOn = !digitalRead(A5);
  
  controllerData.leftStickX = analogRead(A0) >> 2;
  controllerData.leftStickY = analogRead(A1) >> 2;
  controllerData.rightStickY = analogRead(A2) >> 2;
  controllerData.rightStickX = analogRead(A3) >> 2;

  return controllerData;
}
