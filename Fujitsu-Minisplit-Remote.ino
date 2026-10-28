#include <Arduino.h>

#define IR_LED_PIN 16

//IR Constants
#define FUJITSU_AC_HDR_MARK    3224U
#define FUJITSU_AC_HDR_SPACE   1574U
#define FUJITSU_AC_BIT_MARK    448U
#define FUJITSU_AC_ONE_SPACE   1182U
#define FUJITSU_AC_ZERO_SPACE  367U
#define FUJITSU_AC_TRL_MARK    448U
#define FUJITSU_AC_TRL_SPACE   8100U

//Fujitsu Remote Constants
#define FUJITSU_AC_STATE_LENGTH      16
#define FUJITSU_AC_STATE_LENGTH_SHORT 7

const uint32_t CARRIER_FREQ = 38000;
const uint32_t CARRIER_PERIOD = 1000000 / CARRIER_FREQ; //micros

uint8_t remote_state[FUJITSU_AC_STATE_LENGTH];

String inputString;
uint8_t temp, onOffFlag, mode, swing, fanSpeed;
bool off = false;

//Basic IR Functions
void sendCarrier(uint32_t duration) {
  uint32_t cycles = duration / (CARRIER_PERIOD);
  for (uint32_t i = 0; i < cycles; i++) {
    digitalWrite(IR_LED_PIN, HIGH);
    delayMicroseconds(CARRIER_PERIOD / 2);
    digitalWrite(IR_LED_PIN, LOW);
    delayMicroseconds(CARRIER_PERIOD / 2);
  }
}
void mark(uint32_t duration) {
  sendCarrier(duration);
}
void space(uint32_t duration) {
  digitalWrite(IR_LED_PIN, LOW);
  delayMicroseconds(duration);
}
void sendByteLSBFirst(uint64_t data){
  for (uint16_t bit = 0; bit < 8; bit++, data >>= 1){
    if (data & 1) {
      mark(FUJITSU_AC_BIT_MARK);
      space(FUJITSU_AC_ONE_SPACE);
    } else {
      mark(FUJITSU_AC_BIT_MARK);
      space(FUJITSU_AC_ZERO_SPACE);
    }
  }
}

//Fujitsu Specific Functions
void sendFujitsuData(unsigned char data[], uint16_t numBytes){
  //Fujitsu header
  mark(FUJITSU_AC_HDR_MARK);
  space(FUJITSU_AC_HDR_SPACE);
  //Data
  for(uint16_t i = 0; i < numBytes; i++){
    sendByteLSBFirst(data[i]);
  }
  //Footer
  mark(FUJITSU_AC_TRL_MARK);
  space(FUJITSU_AC_TRL_SPACE);

}
uint8_t* getRawData() {
  remote_state[0] = 0x14;
  remote_state[1] = 0x63;
  remote_state[2] = 0x00;
  remote_state[3] = 0x10;
  remote_state[4] = 0x10;
  if(!off){
    remote_state[5] = 0xFE;
    remote_state[6] = 0x09;
    remote_state[7] = 0x30;
    remote_state[8] = (onOffFlag) | (temp << 4);
    remote_state[9] = mode | 0 << 4;  //We're not implementing the timer
    remote_state[10] = fanSpeed | swing << 4;
    remote_state[11] = 0;  //timer
    remote_state[12] = 0;  //timer
    remote_state[13] = 0;  //timer
    remote_state[14] = 0x20;

    uint8_t checksum = 0;
    for (uint8_t i = 7 ; i < 15; ++i) {
      checksum += remote_state[i];
    }
    remote_state[15] = 0 - checksum;
  }else{
    remote_state[5] = 0x02;
    remote_state[6] = ~remote_state[5];
  }
  return remote_state;
}


void setup() {
    pinMode(IR_LED_PIN, OUTPUT);
    digitalWrite(IR_LED_PIN, LOW);
}

void loop() {
  if (Serial.available()) {
    inputString = Serial.readStringUntil('\n');
    inputString.trim();

    if(inputString == "off"){
      Serial.println("off");
      off = true;
    }else{
      off = false;
      int commaIndex = inputString.indexOf(',');
    
      temp = inputString.substring(0, commaIndex).toInt();  
      inputString = inputString.substring(commaIndex + 1);
    
      commaIndex = inputString.indexOf(',');
      onOffFlag = inputString.substring(0, commaIndex).toInt();
      inputString = inputString.substring(commaIndex + 1);
    
      commaIndex = inputString.indexOf(',');
      mode = inputString.substring(0, commaIndex).toInt();
      inputString = inputString.substring(commaIndex + 1);
    
      commaIndex = inputString.indexOf(',');
      swing = inputString.substring(0, commaIndex).toInt();
      inputString = inputString.substring(commaIndex + 1);
    
      fanSpeed = inputString.toInt();

      Serial.println("Received values:");
      Serial.print("temp: ");
      Serial.println(temp);
      Serial.print("onOffFlag: ");
      Serial.println(onOffFlag);
      Serial.print("Mode: ");
      Serial.println(mode);
      Serial.print("Swing: ");
      Serial.println(swing);
      Serial.print("Fan: ");
      Serial.println(fanSpeed);
    }

  }
  if(!off){
    sendFujitsuData(getRawData(), 16);
  }else{
    sendFujitsuData(getRawData(), 7);
  }
  delay(1000);

}
