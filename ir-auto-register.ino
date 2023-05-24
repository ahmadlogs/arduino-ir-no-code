#include <EEPROM.h>
#include <IRremote.h>

#define CONFIG_BUTTON 6
#define IR_RECEIVE_PIN 7

const uint8_t LED_PINS[] = {8,9,10,11};
const uint8_t NUM_HEX_CODES = 4;

uint32_t      hex_codes[NUM_HEX_CODES];
const uint8_t NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);


void setup() {
  Serial.begin(115200);

  IrReceiver.begin(IR_RECEIVE_PIN); // Start the IR receiver
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CONFIG_BUTTON, INPUT_PULLUP);

  // Set LED pins as OUTPUT
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
  }
  
  //read IR Codes
  EEPROM.get(0, hex_codes);
  for(uint8_t i = 0; i < NUM_HEX_CODES; i++) {
    Serial.print(hex_codes[i]); Serial.println();
  }
}

void loop() {
  //-----------------------------------------------------------------------------
  // check if boot button is pressed to enter config mode
  if(digitalRead(CONFIG_BUTTON) == LOW) {
    delay(100); // Key debounce handling
    Serial.println("Push Button");
    enterConfigMode();
  }
  //-----------------------------------------------------------------------------
  // normal mode, check if any registered hex code is received
  if(IrReceiver.decode()) {
    uint32_t ir_code = IrReceiver.decodedIRData.command;
    Serial.println(ir_code);
    for (uint8_t i = 0; i < NUM_HEX_CODES; i++) {
      if (ir_code == hex_codes[i]) {
        digitalWrite(LED_PINS[i], !digitalRead(LED_PINS[i]));
      }
    }
    
    delay(1000);
    IrReceiver.resume();
  }
  //-----------------------------------------------------------------------------
}


void enterConfigMode() {
  Serial.println("Config Mode");
  uint8_t i = 0;
  uint32_t last_ir_code = 0;
  
  while(i < NUM_HEX_CODES) {
    if (IrReceiver.decode()) {
      uint32_t ir_code = IrReceiver.decodedIRData.command;
      //-------------------------------------------------------------------
      if(ir_code != 0 and ir_code != last_ir_code ) { 
        hex_codes[i] = ir_code;
        IrReceiver.resume(); // receive the next value

        Serial.println(ir_code);
        last_ir_code = ir_code;
        digitalWrite(LED_BUILTIN, LOW); delay(3000);
        i++;
      }
      //-------------------------------------------------------------------
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }
  //save IR Codes
  EEPROM.put(0, hex_codes);

  Serial.println("Normal Mode");
  digitalWrite(LED_BUILTIN, HIGH);
  
}
