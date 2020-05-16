//conectar potenciometro al pin analogo 2 y 2 sensores tactiles o cualquier "boton" a pines digitales 4 y 5

#include "UnoJoy.h"
int val = 0;
int val1;
int val2;

void setup(){
  setupPins();
  setupUnoJoy();
}

void loop(){

  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);

  val = analogRead(2);
  if (val >= 0 && val <= 400) {
    val1= HIGH;
  }
  else if (val >= 650 && val <= 1023) {
    val2 = HIGH;
  }
  else {
    val1 = LOW;
    val2 = LOW;
  }
}

void setupPins(void){

  for (int i = 4; i <= 12; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
    pinMode(2, INPUT);
  }
  pinMode(A4, INPUT);
  digitalWrite(A4, HIGH);
  pinMode(A5, INPUT);
  digitalWrite(A5, HIGH);
}

dataForController_t getControllerData(void){

  dataForController_t controllerData = getBlankDataForController();

  controllerData.triangleOn = val1;
  controllerData.circleOn = val2;
  controllerData.squareOn = digitalRead(4);
  controllerData.crossOn = digitalRead(5);
  controllerData.dpadUpOn = !digitalRead(6);
  controllerData.dpadDownOn = !digitalRead(7);
  controllerData.dpadLeftOn = !digitalRead(8);
  controllerData.dpadRightOn = !digitalRead(9);
  controllerData.l1On = !digitalRead(10);
  controllerData.r1On = !digitalRead(11);
  controllerData.selectOn = !digitalRead(12);
  controllerData.startOn = !digitalRead(A4);
  controllerData.homeOn = !digitalRead(A5);
  
  return controllerData;
}
