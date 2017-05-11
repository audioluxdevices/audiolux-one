// Modified simple sketch to support RGBW strips like SK6812 based:
// http://www.szledcolor.com/download/SK6812RGBW.pdf
// Should be used with Adafruit NeoPixel version 1.0.3
// Hacked by G. Knauf 2015-12-06

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#ifdef __AVR_ATtiny85__
#define NEO_PIN		0  // NeoPixel DATA
#undef  LED_BUILTIN
//#define LED_BUILTIN	0 // LED on Model B
#define LED_BUILTIN	1 // LED on Model A
#else
#define NEO_PIN		6  // NeoPixel DATA
#endif

// What type of NeoPixel strip is attached to the Arduino?
#define NEO_PTYPE	NEO_GRBW	// f.e. SK6812
//#define NEO_PTYPE	NEO_GRB		// most NeoPixel products

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS	25

#define BRIGHTNESS	255 // set max brightness

#define IWAIT		2000
#define SWAIT		25
#define LWAIT		50
#define HWAIT		500

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
#ifdef __AVR_ATtiny85__
  // This is for Trinket 5V 16MHz
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  // End of trinket special code
#endif

#ifdef LED_BUILTIN
  // Turn the onboard LED off by making the voltage LOW
  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, LOW);
#endif

  strip.begin(); // This initializes the NeoPixel library.
  strip.setBrightness(BRIGHTNESS); // set brightness
  strip.show(); // Initialize all pixels to 'off'

#ifdef IWAIT
  delay(IWAIT);
#endif
}

void loop() {
  // For a set of NeoPixels the first NeoPixel is 0, second is 1,
  // all the way up to the count of pixels minus one.

  // Moderately bright green color.
  colorWipe(strip.Color(0,150,0,0), LWAIT);
  // Fade off with reducing brightness
  fadeOff(LWAIT);
  strip.clear(); // turn all pixels off
  strip.setBrightness(BRIGHTNESS); // restore max brightness value

  // Moderately bright cyan color.
  colorWipe(strip.Color(0,150,150,0), LWAIT);
  // Fade off with reducing brightness
  fadeOff(LWAIT);
  strip.clear(); // turn all pixels off
  strip.setBrightness(BRIGHTNESS); // restore max brightness value

  // Moderately bright purple color.
  colorWipe(strip.Color(150,0,150,0), LWAIT);
  // Fade off with reducing brightness
  fadeOff(LWAIT);
  strip.clear(); // turn all pixels off
  strip.setBrightness(BRIGHTNESS); // restore max brightness value

#if (((NEO_PTYPE & 0xC0) >> 2) != (NEO_PTYPE & 0x30)) // check for RGBW type
  // Moderately bright white LED.
  colorWipe(strip.Color(0,0,0,150), LWAIT);
#else
  // Moderately bright white color (composed RGB).
  colorWipe(strip.Color(150,150,150,0), LWAIT);
#endif
  // Fade off with reducing brightness
  fadeOff(LWAIT);
  strip.clear(); // turn all pixels off
  strip.setBrightness(BRIGHTNESS); // restore max brightness value
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint16_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Fade off with reducing brightness
void fadeOff(uint16_t wait) {
  byte brightness;
  while ((brightness = strip.getBrightness()) > 0) {
    strip.setBrightness(--brightness);
    strip.show(); // This sends the updated pixel brightness to the hardware.
    delay(wait); // Delay for a period of time (in milliseconds).
  }
}
