// Modified strandtest sketch to support also RGBW strips like SK6812 based:
// http://www.szledcolor.com/download/SK6812RGBW.pdf
// Should be used with Adafruit NeoPixel version 1.0.3
// Hacked by G. Knauf 2015-12-06

#include <Adafruit_NeoPixel.h>
#include "Color_Definitions.h"

// Which pin on the Arduino is connected to the NeoPixels?

#define NEO_PIN		6  // NeoPixel DATA

// What type of NeoPixel strip is attached to the Arduino?
#define NEO_PTYPE	NEO_GRBW	// f.e. SK6812
//#define NEO_PTYPE	NEO_GRB		// most NeoPixel products

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS	33

#define BRIGHTNESS	64 // set max brightness

#define IWAIT		2000
#define SWAIT		20
#define LWAIT		50
#define HWAIT		1500

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (f.e. SK6812)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_PTYPE + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// NOTE: RGBW LEDs draw up to 80mA with all colors + white at full brightness!
// That means that a 60-pixel strip can draw up to 60x 80 = 4800mA, so you
// should use a 5A power supply; for a 144-pixel strip its max 11520mA = 12A!

void setup() {

  strip.begin();
  strip.setBrightness(BRIGHTNESS); // set brightness
  strip.show(); // Initialize all pixels to 'off'

#ifdef IWAIT
  delay(IWAIT);
#endif
}

void loop() {

  int potControlBrightness = analogRead(2); // assign analog reading of left potentiometer knob to control brightness
  int potControlHue = analogRead(3);  // assign analog reading of right potentiometer knob to control hue
  
  int brightness = map(potControlBrightness, 1023, 0, 0, 255); // map left pot knob to 0-255 values of brightness
  int hue = map(potControlHue, 0, 1023, 0, 255); // map right pot knob to 0-255 values of hue

//int i;


  //for(j=0; j<256; j++) {
//    for(i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel((i+hue) & 255));
  //  }
//    strip.show();
//    delay(wait);
//  }
}



void rainbow(uint16_t wait) {
  uint16_t i, j;

 for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
     strip.setPixelColor(i, Wheel((i+j) & 255));
    }
   strip.show();
   delay(wait);
  }

}




// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

