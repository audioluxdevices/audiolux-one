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

// Code for Smoothing Pot Analog Input
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int potControlHueAverage = 0;   // the average


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);

int delayval = 0; // delay for half a second

void setup() {
    //Serial.begin(9600);      // open the serial port at 9600 bps: 
    
    // initialize all the readings to 0:
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
        readings[thisReading] = 0;
    }

  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
        int potControlBrightness = analogRead(2);
        int potControlHue = analogRead(3);
        
        // Code for Analog Pot smoothing
        // subtract the last reading:
        total = total - readings[readIndex];
        // read from the pot:
        readings[readIndex] = potControlHue;
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
        potControlHueAverage = total / numReadings;
        // send it to the computer as ASCII digits
        //Serial.println(potControlHueAverage);
        delay(1);        // delay in between reads for stability
        
        //int numLedsToLight = map(brightness, 1023, 0, 0, NUM_LEDS); // map number of LEDs to left pot
        int brightness = map(potControlBrightness, 1023, 0, 0, 255); // map brightness to left pot
        int hue = map(potControlHueAverage, 0, 1023, 0, 255); // map hue to right pot
        //Serial.println(potControlBrightness);
        
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.

  //for(int i=0;i<NUMPIXELS;i++){
  for(int i = 0; i < NUMPIXELS; i++) { 

    // pixels.Color takes RBWG values, from 0,0,0,0 up to 255,255,255,255
    //pixels.setPixelColor(i, pixels.Color(0,255,0,0)); // Moderately bright blue color.
    pixels.setPixelColor(i, pixels.Color(0,0,brightness,0)); // bright white color.

    pixels.show(); // This sends the updated pixel color to the hardware.

    delay(delayval); // Delay for a period of time (in milliseconds).

  }
}
