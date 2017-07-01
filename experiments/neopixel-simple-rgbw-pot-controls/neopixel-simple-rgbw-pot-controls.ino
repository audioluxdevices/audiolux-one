/*  
  Control SK6812RGBW LEDs with Hue, Saturation and Brightness (HSB / HSV ) plus separate "whiteness" dimmer.
 
  Hue is change by right potentiometer  
  Brightness stays constant at 255.
  Saturation stays constant at 255
  White diode controlled by left potentiometer.
 
  getRGB() function based on <http://www.codeproject.com/miscctrl/CPicker.asp>  
  dim_curve idea by Jims
  based on code by kasperkamperman.com
*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Define which pin on the Audiolux is connected to the LED Data Input
#define PIN            6

// How many LEDs are attached to the Audiolux?
#define NUMPIXELS      300

// Code for Smoothing Pot Analog Input
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int potControlWhitenessAverage = 0;   // the average

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RWGB + NEO_KHZ800);

/* 
  dim_curve 'lookup table' to compensate for the nonlinearity of human vision.
  Used in the getRGB function on saturation and brightness to make 'dimming' look more natural. 
  Exponential function used to create values below : 
  x from 0 - 255 : y = round(pow( 2.0, x+64/40.0) - 1)   
*/
const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

int rgb_colors[3]; 
 
int hue;
int saturation;
int brightness;
int white;

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps:
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  // Assign potentiometer knobs to variables
  int potControlWhiteness = analogRead(2); // assign analog reading of left potentiometer knob to control Whiteness
  int potControlHue = analogRead(3);  // assign analog reading of right potentiometer knob to control hue

  // Code for Analog Pot smoothing
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the pot:
  readings[readIndex] = potControlWhiteness;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
  // ...wrap around to the beginning:
  readIndex = 0;
  }

  // calculate the average:
  potControlWhitenessAverage = total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.println(potControlWhitenessAverage);
  delay(1);        // delay in between reads for stability
        
  // set HSB & White values
  int hue = map(potControlHue, 0, 1023, 0, 255); // map right pot knob to 0-255 values of hue
  int saturation = 255;                               // saturation is a number between 0 - 255
  int brightness = 128;
  int whiteness = map(potControlWhitenessAverage, 1024, 0, 0, 255); // map left pot knob to 0-255 values of Whiteness
  Serial.println(whiteness);
  
  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
 
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  for(int i=0;i<NUMPIXELS;i++){
    // pixels.Color takes RWGB values, from 0,0,0,0 up to 255,255,255,255
    pixels.setPixelColor(i, pixels.Color(rgb_colors[0],whiteness,rgb_colors[1],rgb_colors[2])); // Color order: R,W,G,B.
  }
    pixels.show(); // This sends the updated pixel color to the hardware.

}

void getRGB(int hue, int sat, int val, int colors[3]) { 
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.      
  */
 
  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];
 
  int r;
  int g;
  int b;
  int base;
 
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 
 
    base = ((255 - sat) * val)>>8;
 
    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;
 
    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;
 
    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;
 
    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;
 
    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;
 
    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }
 
    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }   
}
