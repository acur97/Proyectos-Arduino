/* This is the code for the Arduino Uno, it will not work on the Leonardo. */

/* These are constants: whenever we put one of these 3 words in capitals in our code, the precompiler will replace it by the 0xsomething after the word. 
 This makes our code more readable, instead of typing the meaningless 0x90, we can now just type NOTE_ON, if we want a note to be on. */
#define NOTE_OFF       0x80
#define NOTE_ON        0x90
#define CC             0xB0

/* This is also a constant. If you want to change the number of analog inputs, you can simply do it once on this line, instead of changing it everywhere in your code.*/ 
#define NUMBER_OF_ANALOG_INPUTS  6 // The Uno has 6 analog inputs, we'll use all of them in this example. If you only need 4, change this to 4, and you'll be able to use A4 & A5 as normal I/O pins. 
// NOTE: if you change this value, also change the controllers array, to match the number of inputs and the number of controllers.

#define CHANNEL  1 // Send all messages on channel 1.

/* The list with the corresponding controller numbers: for example, the values of the potentiometer on A0 will be sent as the first controller number in this list, A1 as the second, etc... 
 Here's the list with all controller numbers:  http://midi.org/techspecs/midimessages.php#3  You can change them if you want.*/
int controllers[NUMBER_OF_ANALOG_INPUTS] = { 
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15
};  

int analogVal[NUMBER_OF_ANALOG_INPUTS];  // We declare an array for the values from the analog inputs

int analogOld[NUMBER_OF_ANALOG_INPUTS]; // We declare an array for the previous analog values. 


/* The format of the message to send via serial. We create a new data type, that can store 3 bytes at once.  This will be easier to send as MIDI. */
typedef struct {
  uint8_t status;   // first  byte   : status message (NOTE_ON, NOTE_OFF or CC (controlchange) and midi channel (0-15)
  uint8_t data1;    // second byte   : first value (0-127), controller number or note number
  uint8_t data2;    // third  byte   : second value (0-127), controller value or velocity
} 
t_midiMsg;          // We call this data type 't_midiMsg'

void setup() // The setup runs only once, at startup.
{
  pinMode(13, OUTPUT);   // Set pin 13 (the one with the LED) to output
  digitalWrite(13, LOW); // Turn off the LED
  for(int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++){  // We make all values of analogOld -1, so it will always be different from any possible analog reading.
    analogOld[i]=-1;
  }
  Serial.begin(31250);  // Start a serial connection @31250 baud or bits per second on digital pin 0 and 1, this is the connection to the ATmega16U2, which runs the HIDuino MIDI firmware. (31250 baud is the original MIDI speed.)
  delay(1000);           // Wait a second before sending messages, to be sure everything is set up
  digitalWrite(13, HIGH);// Turn on the LED, when the loop is about to start.
}

void loop() // The loop keeps on repeating forever.
{
  t_midiMsg msg;                                     // create a variable 'msg' of data type 't_midiMsg' we just created
  for(int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++){  // Repeat this procedure for every analog input.

    analogVal[i] = analogRead(i+A0)/8;               // The resolution of the Arduino's ADC is 10 bit, and the MIDI message has only 7 bits, 10 - 7 = 3, so we divide by 2^3, or 8.
    if(analogVal[i] != analogOld[i]){                // Only send the value, if it is a different value than last time.
      msg.status = CC;                               // Controll Change
      msg.status = msg.status | CHANNEL-1;           // Channels are zero based (0 = ch1, and F = ch16). Bitwise or to add the status message and channel together: 
                                                    /* status     = 0bssss0000 
                                                     * channel    = 0b0000cccc 
                                                     * | ------------------ (bitwise or)
                                                     * msg.status = 0bsssscccc       
                                                     */
      msg.data1   = controllers[i];                  // Get the controller number from the array above.
      msg.data2   = analogVal[i];                    // Get the value of the analog input from the analogVal array.
      Serial.write((uint8_t *)&msg, sizeof(msg));    // Send the MIDI message.
      analogOld[i] = analogVal[i];                   // Put the analog values in the array for old analog values, so we can compare the new values with the previous ones.
      delay(10);                                     // Wait for 10ms, so it doesn't flood the computer with MIDI-messages
    }
  }

}
