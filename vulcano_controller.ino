#include <Wire.h>
#include <SPI.h>
#include <MIDI.h>

#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>

//#define BOUNCE_LOCK_OUT
#define BOUNCE_WITH_PROMPT_DETECTION
#include <Bounce2.h>


// Imagen logo vulcano
#include "vulcano_logo.h"


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_ADDR 0x3C  // address of my 128x64 OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



#define LED_PIN 13
int ledState = LOW;




#define NUM_BUTTONS 6
const uint8_t BUTTON_PINS[NUM_BUTTONS] = {14, 15, 16, 17, 18, 19};
Bounce * buttons = new Bounce[NUM_BUTTONS];
unsigned long buttonPressTimeStamp[NUM_BUTTONS];
unsigned long buttonReleaseTimeStamp[NUM_BUTTONS];

// Created and binds the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();



void drawLogoVulcano(void) {
  display.clearDisplay();
  display.drawBitmap(0,0, vulcano_logo, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(2000);
}


#define ERR_DISP_ALLOC 3 // display allocation error
void errBlink(int errCode) {
  byte blinkTime = 200; // duration for each blink
  byte blinkGap = 200; // gap between each blink
  int burstWait = 1000; // wait time between bursts 
  for (;;) { // loop forever
    for (int i = 1; i <= errCode; i++) {
      digitalWrite(LED_PIN,HIGH);
      delay(blinkTime);
      digitalWrite(LED_PIN,LOW);
      delay(blinkGap);
    }
    delay(burstWait);
  }
} // end of errBlink()

void setup() {
  Serial.begin(57600);

  pinMode(LED_PIN,OUTPUT); // Setup the LED
  digitalWrite(LED_PIN,ledState);

  MIDI.begin(MIDI_CHANNEL_OMNI);  // Listen to all incoming messages

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].attach( BUTTON_PINS[i] , INPUT_PULLUP  );       //setup the bounce instance for the current button
    buttons[i].interval(15);              // interval in ms
    buttonPressTimeStamp[i] = 0;
    buttonReleaseTimeStamp[i] = 0;
  }

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) { // Address 0x3D for 128x64 0x78 0x7A
    //Serial.println(F("SSD1306 allocation failed"));
    errBlink(ERR_DISP_ALLOC);
  }

  drawLogoVulcano();
  
  //display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  //display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setFont(&FreeSans9pt7b);
  display.clearDisplay();
}


#define NUM_PAGES 2
int page = 1;

int command = 0;
int last_command = -1;
unsigned long now;

void loop() {
  command = 0;
  now = millis();

  readButtons();
  updateSreen();
  last_command = command;
}

bool wait_all_release = false;
bool long_press = false;

void readButtons() {
  int num_pressed = 0;
  long_press = false;
  for (int i = 0; i < NUM_BUTTONS; i++)  {
      buttons[i].update(); // Update the Bounce instance :
      if(buttons[i].read() == LOW) {
        num_pressed++;
      }
      if(buttons[i].fell()) { // button pressed -> send command
        buttonPressTimeStamp[i] = now;
        command = i+1;
      }
      if(buttons[i].rose()) {
        buttonReleaseTimeStamp[i] = now;
        
        if(buttonReleaseTimeStamp[i]-buttonPressTimeStamp[i] > 500){ // long press detected -> send command
          long_press = true;
          command = i+1;
        }

      }
  }

  if(!wait_all_release) { // if not waiting all up
    if(num_pressed>1) { // if double button command
      wait_all_release = true;
      if(buttons[0].read()==LOW && buttons[1].read()==LOW) { // button 1 + button 2 = pageUp
        pageUp();
      } else if(buttons[1].read()==LOW && buttons[2].read()==LOW) { // button 2 + button 3 = pageDown
        pageDown();
      }
    } /* else if(num_pressed==1 && command!=0) { // if single button command
      Serial.print(now);
      Serial.print(" : ");
      Serial.println(command);
    }*/
  } else if (num_pressed==0) { // all up this cicle
    wait_all_release = false;
    command = 0;
  } else { // still waiting all up
    command = 0;
  }
}


// SCREEN ------------------------
unsigned long last_update = 0;
void updateSreen() {
  if(last_command==-1) {
      drawPage();
  } else if (command > 0) {
    sendCommand(command);
  } else if(last_update>1 && ((now-last_update)>1000) ) {
    drawPage();
    last_update = 0;
  }
}

void drawPage() {
  switch(page) {
    case 1:
      drawPage1();
      break;
    case 2:
      drawPage2();
      break;
    default:
      page = 1;
      drawPage1();    
  }
}

void sendCommand(int _com) {
  if(page==1) {
    sendCommandPage1(_com);
  } else if(page==2) {
    sendCommandPage2(_com);
  }
}

void drawCommandText(const __FlashStringHelper * command_text) {
  display.setFont(&FreeSansBoldOblique24pt7b);
  display.clearDisplay();
  display.setCursor(5, 50);
  display.print(command_text);
  display.display();
}


void pageUp() {
  page++;
  if(page>NUM_PAGES) page = 1;
  last_command=-1;  
}

void pageDown() {
  page--;
  if(page<1) page = NUM_PAGES;
  last_command=-1;
}

/*
 * PAGE 1
 */

 
void drawPage1()
{
  last_update = now;

  display.setFont(&FreeSans9pt7b);
  
  display.clearDisplay();
  display.setCursor(0, 60);
  display.print(F("SS1"));

  display.setCursor(45, 60);
  display.print(F("SS2"));

  display.setCursor(95, 60);
  display.print(F("SS3"));

  display.drawFastHLine(0, 32, 128, WHITE);
  
  display.setCursor(0, 20);
  display.print(F("FS4"));

  display.setCursor(45, 20);
  display.print(F("FS5"));

  display.setCursor(88, 20);
  display.print(F("T/T"));
  
  display.display();
}

 
void sendCommandPage1(int _com) {
  if(_com == 1) {
    if(long_press) {
      last_update = now;
      MIDI.sendControlChange(71,0,1); // Stomp mode
      drawCommandText(F("Stomp"));
    } else {
      last_update = now;
      MIDI.sendControlChange(69,0,1); // Snapshot 1
      drawCommandText(F("SS1"));
    }
  } else if(_com == 2) {
    if(long_press) {
      last_update = now;
      MIDI.sendControlChange(71,1,1); // Scroll mode
      drawCommandText(F("Scroll"));
    } else {
      last_update = now;
      MIDI.sendControlChange(69,1,1); // Snapshot 2
      drawCommandText(F("SS2"));
    }
  } else if(_com == 3) {
    if(long_press) {
      last_update = now;
      MIDI.sendControlChange(71,3,1); // Snapshot mode
      drawCommandText(F("Snaps"));
    } else {
      last_update = now;
      MIDI.sendControlChange(69,2,1); // Snapshot 3
      drawCommandText(F("SS3"));
    }
  } else if(_com == 4) {
    last_update = now;
    MIDI.sendControlChange(52,2,1); // FS4
    drawCommandText(F("FS4"));
  } else if(_com == 5) {
    last_update = now;
    MIDI.sendControlChange(53,2,1); // FS5
    drawCommandText(F("FS5"));
  } else if(_com == 6) {
    if(long_press) {
      last_update = now;
      MIDI.sendControlChange(68,68,1); // Tuner
      drawCommandText(F("Tuner"));
    } else {
      last_update = now;
      MIDI.sendControlChange(64,127,1); // Tap tempo
      drawCommandText(F("Tap"));
    }
  }
}

/*
 * PAGE 2
 */

void drawPage2()
{
  last_update = now;

  display.setFont(&FreeSans9pt7b);
  
  display.clearDisplay();
  display.setCursor(0, 60);
  display.print(F("Rec"));

  display.setCursor(45, 60);
  display.print(F("Play"));  

  display.setCursor(95, 60);
  display.print(F("Stop"));  

  display.drawFastHLine(0, 32, 128, WHITE);
  
  display.setCursor(0, 20);
  display.print(F("Over"));  

  display.setCursor(45, 20);
  display.print(F("U/R"));
  

  display.setCursor(90, 20);
  display.print(F("Pl1"));
  
  display.display();
}


void sendCommandPage2(int _com) {
  if(_com == 1) {
    last_update = now;
    MIDI.sendControlChange(60,127,1); // Looper record
    drawCommandText(F("Rec"));
  } else if(_com == 2) {
    last_update = now;
    MIDI.sendControlChange(61,127,1); // Looper play
    drawCommandText(F("Play"));
  } else if(_com == 3) {
    if(long_press) { // page down on long press stop to avoid extra commands
      pageDown();
    } else {
      last_update = now;
      MIDI.sendControlChange(61,0,1); // Looper stop
      drawCommandText(F("Stop"));
    }
  } else if(_com == 4) {
    last_update = now;
    MIDI.sendControlChange(60,0,1); // Looper overdub
    drawCommandText(F("Over"));
  } else if(_com == 5) {
    last_update = now;
    MIDI.sendControlChange(63,127,1); // Looper Undo / Redo
    drawCommandText(F("U/R"));
  } else if(_com == 6) {
    last_update = now;
    MIDI.sendControlChange(62,127,1); // Looper play once
    drawCommandText(F("Play1"));
  }
}
