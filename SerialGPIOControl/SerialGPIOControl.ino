#include <Arduino.h>
#include "pitches.h"

String strInput;     // String for the full command
String strTarget;       // String for the target
String strCmd;          // String for the command
String strValue;        // String for the value

#define GP14 14
#define GP15 15
#define GP16 16
#define GP17 17
#define GP18 18
#define GP19 19
#define GP20 20
#define GP21 21
#define GP6 6


/**
 * This could be a bitfield
 */
typedef struct {
  uint8_t _GP21 = 0; //CH1
  uint8_t _GP20 = 0; //CH2
  uint8_t _GP19 = 0; //CH3
  uint8_t _GP18 = 0; //CH4
  uint8_t _GP17 = 0; //CH5
  uint8_t _GP16 = 0; //CH6
  uint8_t _GP15 = 0; //CH7
  uint8_t _GP14 = 0; //CH8
}GPIOStatuses;

GPIOStatuses gpio_statuses;

//e d# e d# e b d c a
// notes in the melody:
int notes[] = {
  NOTE_E6, NOTE_DS6, NOTE_E6, NOTE_DS6, NOTE_E6, NOTE_B5, NOTE_C6, NOTE_A7
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int note_durations[] = {
  4, 4, 8, 4, 2, 2, 4, 4
};

/**
 * @brief Play the startup tone
 * 
 */
void playStartupTone(){
  
  // iterate over the notes of the melody:
  for (int cur_note = 0; cur_note < 8; cur_note++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int note_duration = 1000 / note_durations[cur_note];
    tone(GP6, notes[cur_note], note_duration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = note_duration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(GP6);
  }
}


/**
 * @brief Ensure it's one of the supported GPIOs
 * 
 * @param GPIOPin 
 * @return true 
 * @return false 
 */
bool isValidGPIO(uint8_t GPIOPin){

  switch(GPIOPin){
    case GP14:
    case GP15:
    case GP16:
    case GP17:
    case GP18:
    case GP19:
    case GP20:
    case GP21:
      return true;

    default:
      return false;
  }

  return false;
}

void updateGPIOStatuses(uint8_t GPIOPin, uint8_t status){
  
  switch(GPIOPin){

    case GP14:
      gpio_statuses._GP14 = status;
      break;
    case GP15:
      gpio_statuses._GP15 = status;
      break;
    case GP16:
      gpio_statuses._GP16 = status;
      break;
    case GP17:
      gpio_statuses._GP17 = status;
      break;
    case GP18:
      gpio_statuses._GP18 = status;
      break;
    case GP19:
      gpio_statuses._GP19 = status;
      break;
    case GP20:
      gpio_statuses._GP20 = status;
      break;
    case GP21:
      gpio_statuses._GP21 = status;
      break;

    default:
      //do nothing;
      break;
  }

}

/**
 * @brief Ensure it's one of high or low
 * 
 * @param GPIOStatus 
 * @return true 
 * @return false 
 */
bool isValidGPIOStatus(uint8_t GPIOStatus){
  
  switch(GPIOStatus){
    case 0:
    case 1:
      return true;

    default:
      return false;
  }

  return false;
}

/**
 * @brief Enforce the contract
 * 
 * @param GPIOStatus 
 * @return uint8_t 
 */
uint8_t getValidGPIOStatus(uint8_t GPIOStatus){

  switch(GPIOStatus){
    case 0:
      return LOW;
    case 1:
      return HIGH;

    default:
      return LOW;
  }

  return LOW;
}

/**
 * @brief Set all the relevant GPIO pins to default OUTPUT/LOW
 * 
 */
void setDefaultGPIO(){

  pinMode(GP14, OUTPUT);
  pinMode(GP15, OUTPUT);
  pinMode(GP16, OUTPUT);
  pinMode(GP17, OUTPUT);
  pinMode(GP18, OUTPUT);
  pinMode(GP19, OUTPUT);
  pinMode(GP20, OUTPUT);
  pinMode(GP21, OUTPUT);

  digitalWrite(GP14, LOW);
  gpio_statuses._GP14 = 0;

  digitalWrite(GP15, LOW);
  gpio_statuses._GP15 = 0;

  digitalWrite(GP16, LOW);
  gpio_statuses._GP16 = 0;

  digitalWrite(GP17, LOW);
  gpio_statuses._GP17 = 0;

  digitalWrite(GP18, LOW);
  gpio_statuses._GP18 = 0;

  digitalWrite(GP19, LOW);
  gpio_statuses._GP19 = 0;

  digitalWrite(GP20, LOW);
  gpio_statuses._GP20 = 0;

  digitalWrite(GP21, LOW);
  gpio_statuses._GP21 = 0;

}

void setup() {
  
  playStartupTone();

  setDefaultGPIO();

  Serial.begin(115200); // Start the Serial communication

  // Reserve space for the strings
  strInput.reserve(64);
  strTarget.reserve(16);
  strCmd.reserve(16);
  strValue.reserve(16);
}

void loop() {
  
  if (Serial.available() > 0) {
    readSerialCommand();
  }

  writeSerialStatus();

}

void writeSerialStatus(){
  //do nothing for now
}

void readSerialCommand() {
  strInput = "";  // Clear the string
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      parseCommand();
      break;
    }
    strInput += c;
  }
}

void parseCommand() {

  int firstCommaIndex = strInput.indexOf(',');
  int secondCommaIndex = strInput.indexOf(',', firstCommaIndex + 1);

  strCmd = strInput.substring(0, firstCommaIndex);
  strTarget =    strInput.substring(firstCommaIndex + 1, secondCommaIndex);
  strValue =  strInput.substring(secondCommaIndex + 1);

  uint8_t cmd = static_cast<uint8_t>(strCmd.toInt());
  uint8_t target = static_cast<uint8_t>(strTarget.toInt());
  uint8_t value = static_cast<uint8_t>(strValue.toInt());

  switch(cmd){
    case 0: //set GPIO HIGH/LOW
      setGPIO(target, value);
      writePacket(cmd, target, value);
      break;
    case 99: //PANIC and set defaults
      setDefaultGPIO();
      writePacket(cmd, target, value);
    default:
      Serial.print("UNSUPPORTED CMD\n");
  }
  
}

void writePacket(uint8_t cmd, uint8_t target, uint8_t value){
  Serial.print(cmd);
  Serial.print(',');
  Serial.print(target);
  Serial.print(',');
  Serial.print(value);
  Serial.print('\n');
}

void setGPIO(uint8_t GPIOPin, uint8_t status){

  if(isValidGPIO(GPIOPin)){
    if(isValidGPIOStatus(status)){
      digitalWrite(GPIOPin, getValidGPIOStatus(status));
      updateGPIOStatuses(GPIOPin, status);
    }else{
      Serial.print("ERR BAD STATUS\n");
    }
  }else{
    Serial.print("ERR INVALID GPIO\n");
  }
}
