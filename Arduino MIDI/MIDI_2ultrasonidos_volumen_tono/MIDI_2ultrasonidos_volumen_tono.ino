const int EchoPin = 8;
const int TriggerPin = 9;
const int EchoPin2 = 6;
const int TriggerPin2 = 7;

const int LedPinA = 11;
const int LedPinB = 10;

int MejorVolumen = 0;
int MejorBrilloA = 0;
int MejorBrilloB = 0;

#define NOTE_OFF       0x80
#define NOTE_ON        0x90
#define CC             0xB0

#define NUMBER_OF_ANALOG_INPUTS  1
#define NUMBER_OF_DIGITAL_INPUTS 11
#define CHANNEL  1

int A = 0;
int B = 0;
int C = 0;

int D = 0;
int E = 0;
int F = 0;

typedef struct {
  uint8_t status;
  uint8_t data1;
  uint8_t data2;
}
t_midiMsg;

void setup() {
  analogWrite(LedPinA, 0);
  analogWrite(LedPinB, 255);

  //Serial.begin(9600);                   //SERIAL DE PRUEBAS
  Serial.begin(31250);                  //SERIAL MIDI

  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(TriggerPin2, OUTPUT);
  pinMode(EchoPin2, INPUT);
  pinMode(LedPinA, OUTPUT);
  pinMode(LedPinB, OUTPUT);

  analogWrite(LedPinA, 255);
  analogWrite(LedPinB, 0);
}

void loop() {
  t_midiMsg msg;

  int cm = ping(TriggerPin, EchoPin);
  int cm2 = ping2(TriggerPin2, EchoPin2);

  if (cm2 > 20 && cm2 < 225)
  {
    MejorVolumen = map(cm2, 20, 225, 127, 0);
  }

  if (cm > 20 && cm < 225)
  {
    MejorBrilloA = map(cm, 20, 225, 0, 255);
    analogWrite(LedPinA, MejorBrilloA);
  }

  if (cm2 > 20 && cm2 < 225)
  {
    MejorBrilloB = map(cm2, 20, 225, 255, 0);
    analogWrite(LedPinB, MejorBrilloB);
  }
  if (cm2 > 225)
  {
    analogWrite(LedPinB, 0);
  }


  //Serial.println(cm);

  D = A;
  E = B;
  F = C;

  if (cm > 20 && cm < 75)
  {
    A = 1;
    B = 0;
    C = 0;
  }
  if (cm > 76 && cm < 150)
  {
    A = 0;
    B = 1;
    C = 0;
  }
  if (cm > 151 && cm < 225)
  {
    A = 0;
    B = 0;
    C = 1;
  }

  if (A != D)
  {
    if (A == 1)
    {
      msg.status = NOTE_ON;
      msg.status = msg.status | CHANNEL - 1;
      msg.data1   = 0x31;
      msg.data2   = MejorVolumen;
      Serial.write((uint8_t *)&msg, sizeof(msg));
    }
    else
    {
      msg.status = NOTE_OFF;
      msg.status = msg.status | CHANNEL - 1;
      msg.data1   = 0x31;
      msg.data2   = MejorVolumen;
      Serial.write((uint8_t *)&msg, sizeof(msg));
    }
  }
  if (B != E)
  {
    if (B == 1)
    {
      msg.status = NOTE_ON;
      msg.status = msg.status | CHANNEL - 1;
      msg.data1   = 0x32;
      msg.data2   = MejorVolumen;
      Serial.write((uint8_t *)&msg, sizeof(msg));
    }
    else
    {
      msg.status = NOTE_OFF;
      msg.status = msg.status | CHANNEL - 1;
      msg.data1   = 0x32;
      msg.data2   = MejorVolumen;
      Serial.write((uint8_t *)&msg, sizeof(msg));
    }
  }
  if (C != F)
  {
    if (C == 1)
    {
      msg.status = NOTE_ON;
      msg.status = msg.status | CHANNEL - 1;
      msg.data1   = 0x33;
      msg.data2   = MejorVolumen;
      Serial.write((uint8_t *)&msg, sizeof(msg));
    }
    else
    {
      msg.status = NOTE_OFF;
      msg.status = msg.status | CHANNEL - 1;
      msg.data1   = 0x33;
      msg.data2   = MejorVolumen;
      Serial.write((uint8_t *)&msg, sizeof(msg));
    }
  }
}

int ping(int TriggerPin, int EchoPin)
{
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

  distanceCm = duration / 8;
  return distanceCm;
}

int ping2(int TriggerPin2, int EchoPin2)
{
  long duration2, distanceCm2;

  digitalWrite(TriggerPin2, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin2, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin2, LOW);

  duration2 = pulseIn(EchoPin2, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

  distanceCm2 = duration2 / 8;
  return distanceCm2;
}
