/*
Title: DemoReel100 with Button
By: Mark Kriegsman
Button added by: Andrew Tuline
- FastLED library (get 3.1) from https://github.com/FastLED/FastLED
- JChristensen's Button Library from https://github.com/JChristensen/Button
*/

#include "FastLED.h"
#include "JC_Button.h"                                           // Button library. Includes press, long press, double press detection.



// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Pushbutton pin definition
const int buttonPin = 5;                                      // Digital pin used for debounced pushbutton

// int buttonState = 0;
// int lastButtonState = 0;
Button myBtn(buttonPin, true, true, 50);                      // Declare the button



#define DATA_PIN   6
//#define CLK_PIN   11
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    149
CRGB leds[NUM_LEDS];

#define FRAMES_PER_SECOND  120

uint8_t max_bright = 64;

void setup() {
  Serial.begin(57600);
  delay(3000); // 3 second delay for recovery
  
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
//FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(max_bright);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {confetti };
//SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  // EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

    readbutton(); // Button press increases the ledMode up to last contiguous mode and then starts over at 0.

}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 2);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, map(analogRead(3), 0, 1023, 5, 100));
  int pos = random16(NUM_LEDS);
  //leds[pos] += CHSV( gHue + random8(64), 200, 255);
  leds[pos] += CHSV(50, 5, 200);
  FastLED.delay(map(analogRead(2), 0, 1023, 5, 100));
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
 
} 


void readbutton() {                                           // Read the button and increase the mode
  myBtn.read();
  if(myBtn.wasReleased()) {
    nextPattern();
  }
} // readbutton()
