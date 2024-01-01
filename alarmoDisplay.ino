#include <TM1637Display.h>

// DISPLAY SETUP
#define CLK D5
#define DIO D7
TM1637Display display(CLK, DIO);

int delay_fast = 200;

byte cara1[] = { 0b00110000, 0, 0, 0 };
byte cara2[] = { 0b00110110, 0, 0, 0 };
byte cara3[] = { 0b00000110, 0b00110000, 0, 0 };
byte cara4[] = { 0, 0b00110110, 0, 0 };
byte cara5[] = { 0, 0b00000110, 0b00110000, 0 };
byte cara6[] = { 0, 0, 0b00110110, 0 };
byte cara7[] = { 0, 0, 0b00000110, 0b00110000 };
byte cara8[] = { 0, 0, 0, 0b00110110 };
byte cara9[] = { 0, 0, 0, 0b00000110};

byte* caraArrays[] = {  cara1, cara2, cara3, cara4, cara5, cara6, cara7, cara8, cara9};



void display_regime(int rezim) {
  
  if (rezim == 0) { // Odchod - Exit delay - 3min
    display.clear();
      for (int i = 0; i < 9; i++){
        display.setSegments(caraArrays[i]);
        delay(delay_fast);
    }
  }


  if (rezim == 2) { // Triggered - Entry delay - 2min
    display.clear();
      for (int i = 8; i >= 0; i--){
      display.setSegments(caraArrays[i]);
      delay(delay_fast);
      }
  }
  if (rezim == 3) { // Armed - Kolečko doleva
  }

    if (rezim == 4) { // DISArmed - Kolečko doprava
  }
}

void setup() {
 
  display.setBrightness(1);  // Set the display brightness (0-7)
}

void loop() {
    display_regime(0);
    display_regime(2);
}