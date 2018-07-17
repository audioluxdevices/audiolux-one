
// StrandTest from AdaFruit implemented as a state machine
// pattern change by push button
// By Mike Cook Jan 2016

#define PINforControl   6 // pin connected to the small NeoPixels strip
#define NUMPIXELS1      256 // number of LEDs on strip

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS1, PINforControl, NEO_GRB + NEO_KHZ800);

unsigned long patternInterval = 20 ; // time between steps in the pattern
unsigned long lastUpdate = 0 ; // for millis() when last update occoured
unsigned long intervals [] = { 20, 20, 50, 100 } ; // speed for each pattern
const byte button = 5; // pin to connect button switch to between pin and ground

void setup() {
  strip.begin(); // This initializes the NeoPixel library.
  wipe(); // wipes the LED buffers
  pinMode(button, INPUT_PULLUP); // change pattern button
}

void loop() {
  static int pattern = 0, lastReading;
  int reading = digitalRead(button);
  if(lastReading == HIGH && reading == LOW){
    pattern++ ; // change pattern number
    if(pattern > 3) pattern = 0; // wrap round if too big
    patternInterval = intervals[pattern]; // set speed for this pattern
    wipe(); // clear out the buffer 
    delay(50); // debounce delay
  }
  lastReading = reading; // save for next time

if(millis() - lastUpdate > patternInterval) updatePattern(pattern);
}

void  updatePattern(int pat){ // call the pattern currently being created
  switch(pat) {
    case 0:
        rainbow(); 
        break;
    case 1: 
        rainbowCycle();
        break;
    case 2:
        theaterChaseRainbow(); 
        break;
    case 3:
         colorWipe(strip.Color(255, 0, 0)); // red
         break;     
  }  
}

void rainbow() { // modified from Adafruit example to make it a state machine
  static uint16_t j=0;
    for(int i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
     j++;
  if(j >= 256) j=0;
  lastUpdate = millis(); // time for next change to the display
  
}
void rainbowCycle() { // modified from Adafruit example to make it a state machine
  static uint16_t j=0;
    for(int i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
  j++;
  if(j >= 256*5) j=0;
  lastUpdate = millis(); // time for next change to the display
}

void theaterChaseRainbow() { // modified from Adafruit example to make it a state machine
  static int j=0, q = 0;
  static boolean on = true;
     if(on){
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
             }
     }
      else {
           for (int i=0; i < strip.numPixels(); i=i+3) {
               strip.setPixelColor(i+q, 0);        //turn every third pixel off
                 }
      }
     on = !on; // toggel pixelse on or off for next time
      strip.show(); // display
      q++; // update the q variable
      if(q >=3 ){ // if it overflows reset it and update the J variable
        q=0;
        j++;
        if(j >= 256) j = 0;
      }
  lastUpdate = millis(); // time for next change to the display    
}

void colorWipe(uint32_t c) { // modified from Adafruit example to make it a state machine
  static int i =0;
    strip.setPixelColor(i, c);
    strip.show();
  i++;
  if(i >= strip.numPixels()){
    i = 0;
    wipe(); // blank out strip
  }
  lastUpdate = millis(); // time for next change to the display
}


void wipe(){ // clear all LEDs
     for(int i=0;i<strip.numPixels();i++){
       strip.setPixelColor(i, strip.Color(0,0,0)); 
       }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
