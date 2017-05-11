// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      33

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RWGB + NEO_KHZ800);

int delayval = 500; // delay for half a second

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
//const int sensorPin = 2;  // pin the potmeter is attached too
//int sensorVal = 0; // store the value coming from the sensor
//int fadeVal   = 0; // value that changes between 0-255
//int fadeSpeed = 4; // 'speed' of fading
int rgb_colors[3]; 
 
int hue;
int saturation;
int brightness;
int white;

void setup() {
  Serial.begin(9600);      // open the serial port at 9600 bps: 
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  //sensorVal = analogRead(sensorPin);     
  //Serial.println(sensorVal);
  // fade from 0 - 255 and back with a certain speed   
 // fadeVal = fadeVal + fadeSpeed;         // change fadeVal by speed
  //fadeVal = constrain(fadeVal, 0, 255);  // keep fadeVal between 0 and 255
 
  //if(fadeVal==255 || fadeVal==0)         // change from up>down or down-up (negative/positive)
  //{ fadeSpeed = -fadeSpeed;  
  //}

  int potControlWhiteness = analogRead(2); // assign analog reading of left potentiometer knob to control Whiteness
  int potControlHue = analogRead(3);  // assign analog reading of right potentiometer knob to control hue

  int brightness = 255;
  int hue = map(potControlHue, 0, 1023, 0, 255); // map right pot knob to 0-255 values of hue
  int whiteness = map(potControlWhiteness, 1023, 0, 0, 255); // map left pot knob to 0-255 values of Whiteness


  // set HSB values
  //hue        = map(sensorVal,0, 1023,0, 359);     // hue is a number between 0 and 360
  saturation = 255;                               // saturation is a number between 0 - 255
  //brightness = fadeVal;                           // value is a number between 0 - 255
 
  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
 
 // analogWrite(ledPinR, rgb_colors[0]);            // red value in index 0 of rgb_colors array
 // analogWrite(ledPinG, rgb_colors[1]);            // green value in index 1 of rgb_colors array
 // analogWrite(ledPinB, rgb_colors[2]);            // blue value in index 2 of rgb_colors array
 
  delay(20); // delay to slow down fading
  
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RWGB values, from 0,0,0,0 up to 255,255,255,255
    //pixels.setPixelColor(i, pixels.Color(0,255,0,0)); // Moderately bright blue color.
    pixels.setPixelColor(i, pixels.Color(rgb_colors[0],whiteness,rgb_colors[1],rgb_colors[2])); // Color order: R,W,G,B.

    pixels.show(); // This sends the updated pixel color to the hardware.

    //delay(delayval); // Delay for a period of time (in milliseconds).

  }
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
