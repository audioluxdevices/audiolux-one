// Merge of RGBW_simple_pot_control & A3 by Chris Mansell (chrismansell@gmail.com)
#include <Adafruit_NeoPixel.h>
#include "EEPROM.h"
#define PIN 6
#define CNT_LIGHTS 149 // Max LEDs is ~267 with RGB array (more is unstable).

// Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_RGB + NEO_KHZ800);
 Adafruit_NeoPixel pixels = Adafruit_NeoPixel(CNT_LIGHTS, PIN, NEO_RGB + NEO_KHZ800);

// the above ' Adafruit_NeoPixel strip ' was defined in one sketch and 'pixel' in another, so I've changed all instances of strip to pixel.

/* === Fixed settings === */

// Hardware Variables
int analog_pin_L = 1; // read from multiplexer using analog input 0
int analog_pin_R = 0; // read from multiplexer using analog input 0
int analog_pinpot = 2;// dial for speed
int analog_pot_range = 3;// dial for range
int stomp = 5;
int strobe_pin = 12; // strobe is attached to digital pin 2
int reset_pin = 13; // reset is attached to digital pin 3

// LED Variables
int color_state = 0;
int refresh = 10;
int refresh_counter = 0;
int tmp_refresh_adj = 0;
int global_brightness = 128; // Sets global brightness, i.e. 64 is 1/4 brightness.
int left_end_point = 0;
int left_start_point = ((CNT_LIGHTS/2)-1);
int right_start_point = ((CNT_LIGHTS/2));
int right_end_point = (CNT_LIGHTS-1);

// Variable to test whether monomode is active
boolean monomode = 0;



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __AVR__
  #include <avr/power.h>
#endif


// Code for Smoothing Pot Analog Input
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int potControlBrightnessAverage = 0;   // the average



// Code for Smoothing Pot Analog Input
const int numReadings2 = 10;
int readings2[numReadings2];      // the readings from the analog input
int readIndex2 = 0;              // the index of the current reading
int total2 = 0;                  // the running total
int potControlHueAverage2 = 0;   // the average

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.

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
 
int hue = 115; 
int saturation;
int brightness = 128; //starting value

//////////////////////////////////////////////////////////additional Code Written by LEDBFG /////////////////////////////////////////////



//make it so that the eeprom is saved when new function is ran,
//the when LEDs are off, and when the button is pressed, it should return to the value stored in eeprom, same as startup

// eeprom
//change from low to high

int current;         // Current state of the button
long millis_held;    // How long the button was held (milliseconds)
long secs_held;      // How long the button was held (seconds)
long prev_secs_held; // How long the button was held in the previous check
byte previous = LOW; ///////////////////////////////////////////////////////////////was HIGH
unsigned long firstTime; // how long since the button was first pressed 



byte modeB;
byte modeA=0;



void setup() {

//  modeB=EEPROM.read(3); modeA=EEPROM.read(2);
  
 Serial.begin(9600);

                                      
   pixels.begin(); // This initializes the NeoPixel library.

  
  pixels.show(); // Initialize all pixels to 'off'

//  if (EEPROM.read(1) > 1){EEPROM.write(1,0);} // Clear EEPROM

  // Set pin modes
  pinMode(analog_pin_L, INPUT);
  pinMode(analog_pin_R, INPUT);  
  pinMode(stomp, INPUT);  
 //  pinMode(5,INPUT_PULLUP);  ////////////this was used to test the program using my hardware (btn wired to gnd)
  pinMode(analog_pin_R, INPUT);
  pinMode(strobe_pin, OUTPUT);
  pinMode(reset_pin, OUTPUT);

  digitalWrite(reset_pin, LOW);
  digitalWrite(strobe_pin, HIGH); 

  // If stomp is being pressed during setup, set monomode to True
  if (digitalRead(stomp) == HIGH){
    if (EEPROM.read(1) == 0){
      EEPROM.write(1,1);
    }else if (EEPROM.read(1) == 1) {
      EEPROM.write(1,0);
    }  
  }

  // Set monomode based on the EEPROM state
  monomode = EEPROM.read(1);

}


void loop() {
detectButtonPress();
if (modeB==0){  colorWipe();}         //run normal hue selection code
else if (modeB==1){ musicReactLoop();} // run music code
else if (modeB==3){
    for( int i = 0; i<CNT_LIGHTS; i++){
        pixels.setPixelColor(i,0,0,0);
        pixels.show();
    }   
   
} //clear LEDs
/*
if (modeA==0)Serial.println(" -----------");
else Serial.println("---");
*/


}

void detectButtonPress(){
  current = digitalRead(stomp);

///need to replace all highs with lows and lows with highs
// also change byte previous to LOW instead of HIGH situated in variables

  if (current == HIGH && previous == LOW && (millis() - firstTime) > 200) {
    firstTime = millis();
  }

  millis_held = (millis() - firstTime);
  secs_held = millis_held / 1000;
  // This if statement is a basic debouncing tool, the button must be pushed for at least
  // 100 milliseconds in a row for it to be considered as a push.
  if (millis_held > 50) {
    if (current == LOW && previous == HIGH) {
      if (millis_held >= 50 && millis_held<2000) { Serial.println("Second Mode Switch"); modeB++; if(modeB==2){modeB=0;} if(modeB==3){ modeB=1;} }  //modeB=EEPROM.read(3); modeA=EEPROM.read(2);
     // if (millis_held >= 50 && millis_held<1000) { Serial.println("First Mode Switch"); modeA++; if(modeA==2){modeA=0;} if(modeB==3){modeB=1;} } //  modeB=EEPROM.read(3); modeA=EEPROM.read(2);last statement is to activate leds from a single press when they are set to off
     // if (secs_held >= 1 && secs_held<3) { Serial.println("Second Mode Switch"); modeB++; if(modeB==2){modeB=0;} if(modeB==3){ modeB=1;} }  //modeB=EEPROM.read(3); modeA=EEPROM.read(2);
     //  EEPROM.write(2,modeA);
    //   EEPROM.write(3,modeB);
     Serial.print("ModeA = ");Serial.println(modeA);
     Serial.print("ModeB = ");Serial.println(modeB);

     if (secs_held >=3) {Serial.println("LEDs off"); modeB=3; } 

  }
  previous = current;
  prev_secs_held = secs_held;
  }
 // Serial.println(EEPROM.read(1));Serial.println(EEPROM.read(2));Serial.println(EEPROM.read(3));
}


// Fill the dots one after the other with a color
void colorWipe() {
  for(int i=0;i<CNT_LIGHTS;i++){
    pixels.setPixelColor(i, pixels.Color(100 ,0,0)); // Color order: R,G,B.
    pixels.show();
  }
}
/*
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbowCycle() {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
  
  }
}
*/
void runNormalLoop(){  // this is the whole function of the previous SIMPLE RBGW POT sketch
  // Assign potentiometer knobs to variables
  int potControlHue  = analogRead(2); // assign analog reading of left potentiometer knob to control Hue
  int potControlBrightness  = analogRead(3);  // assign analog reading of right potentiometer knob to control Brightness

  // Code for Analog Pot smoothing
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the pot:
  readings[readIndex] = potControlBrightness;
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
  potControlBrightness = total / numReadings;
  // send it to the computer as ASCII digits
  //Serial.println(potControlBrightnessAverage);
  delay(1);        // delay in between reads for stability
        
  // set HSB & White values


   // Code for Analog Pot smoothing
  // subtract the last reading:
  total2 = total2 - readings2[readIndex2];
  // read from the pot:
  readings2[readIndex2] = potControlHue;
  // add the reading to the total:
  total2 = total2 + readings2[readIndex2];
  // advance to the next position in the array:
  readIndex2= readIndex2 + 1;

  // if we're at the end of the array...
  if (readIndex2 >= numReadings2) {
  // ...wrap around to the beginning:
  readIndex2 = 0;
  }

  // calculate the average:
  potControlHueAverage2 = total2 / numReadings2;
  // send it to the computer as ASCII digits
  //Serial.println(potControlBrightnessAverage);
  delay(1);        // delay in between reads for stability
        
  // set HSB & White values

  if (modeA==0){hue = map(potControlHueAverage2, 0, 1023, 0, 255);}   //update Hue Values
  else if (modeA==1) {brightness = map(potControlHueAverage2, 0, 1023, 0, 255);} //Update Brightness Values
  
  
    
   // map right pot knob to 0-255 values of hue
   saturation = 255;                               // saturation is a number between 0 - 255
   brightness = map(potControlBrightness, 1023, 0, 0, 255); // map left pot knob to 0-255 values of Brightness
 // Serial.println(whiteness);
  
  getRGB(hue,saturation,brightness,rgb_colors);   // converts HSB to RGB
 
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  for(int i=0;i<CNT_LIGHTS;i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(rgb_colors[0],rgb_colors[1],rgb_colors[2])); // Color order: R,G,B.
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


void musicReactLoop(){

/*
   // If stomp button is pressed, change color_state
  check_color_state();
*/

  digitalWrite(reset_pin, HIGH);
  digitalWrite(reset_pin, LOW);

  int cur_sum_L = 0;  
  int cur_sum_R = 0;

  // Represents all of the LED freq sum values in the LED strip
  int cur_LED_vals[CNT_LIGHTS];
  int prev_LED_vals[CNT_LIGHTS];

  // Get spectrum totals for L and R
  cur_sum_L = read_freq_sum(analog_pin_L);
  cur_sum_R = read_freq_sum(analog_pin_R);

  // If monomode is active, make L equal to R
  if (monomode == 1){
    cur_sum_L = cur_sum_R;
  }

  // Update LED values, moving them down the line based on amplitude of all frequencies
  update_led_positions(cur_LED_vals, prev_LED_vals, cur_sum_L, cur_sum_R);
  set_pixel_colors(cur_LED_vals);       

  pixels.show();
}

/*
int check_color_state() {
  if (digitalRead(stomp) == HIGH){
    delay(100);
    if(color_state < 2){color_state++;}
    else if(color_state == 2){color_state=0;}
  }
}*/

int read_freq_sum(int pin) {

  int i;
  int spectrum_values[7];
  int spectrum_total = 0;

  //get readings from chip, sum freq values
  for (i = 0; i < 7; i++)
  {
    digitalWrite(strobe_pin, LOW);
    delayMicroseconds(30); // to allow the output to settle

    spectrum_values[i] = analogRead(pin);
    adjust_refresh_rate(spectrum_values[i], i);
    
    spectrum_total += spectrum_values[i];
    
    // strobe to the next frequency
    digitalWrite(strobe_pin, HIGH); 
   
  }//for i

  return spectrum_total;
}

void adjust_refresh_rate(int amplitude, int freq_band) {
  if( amplitude > 400 && freq_band <= 1 )
    tmp_refresh_adj += 23;
}

void update_led_positions(int cur_LED_vals[], int prev_LED_vals[], int cur_sum_L, int cur_sum_R) {

  int i;
  int use_refresh = 10;
  float pot_value = analogRead(analog_pinpot); //for use of dial

  if(pot_value < 1000) {
    if (pot_value > 20){
      use_refresh = pot_value / 50;
    } else {
      use_refresh = 0;
    }
  }

  // Constrain temporary refresh rate adjustment to between 0 and 200
  if(tmp_refresh_adj < 0)
    tmp_refresh_adj = 0;
  if(tmp_refresh_adj > 200)
    tmp_refresh_adj = 200;

  refresh_counter++;

  if(refresh_counter >= (use_refresh - round(tmp_refresh_adj * .1))) {
    
    //reset the counter
    refresh_counter = 0;

    // Decay the temporary refresh rate adjustment
    tmp_refresh_adj -= 1;

    //save the history - RIGHT SIDE
    for (i = right_start_point; i <= right_end_point; i++) {
      prev_LED_vals[i] = cur_LED_vals[i-1];
    }//for
    for (i = right_start_point; i <= right_end_point; i++) {
      cur_LED_vals[i] = prev_LED_vals[i];
    }//for
    
    //save the history - LEFT SIDE
    for (i = left_start_point; i >= left_end_point; i--) {
      prev_LED_vals[i] = cur_LED_vals[i+1];
    }//for
    for (i = left_start_point; i >= left_end_point; i--) {
      cur_LED_vals[i] = prev_LED_vals[i];
    }//for

    cur_LED_vals[left_start_point] = cur_sum_L;
    cur_LED_vals[right_start_point] = cur_sum_R;
    
  }//if refresh
}

float get_wave_length(int num)
{
  float min_val = 500;
  float max_val = 4700;
  float min_wave = 350;
  float max_wave = 650;
  float pot_range = analogRead(analog_pot_range);
  max_val = pot_range * 5;
  min_val = pot_range / 2;
  if(num > max_val)
    max_val = num;

  return ((num - min_val) / (max_val-min_val) * (max_wave - min_wave)) + min_wave;

}

void get_RGB(float wave_value, int color_array[])
{

  float rz = 0, gz = 0, bz = 0;
  int r,g,b;
  
  // If wave value is within a certain range, set color value (0.0 - 1.0)
  if(wave_value >380 && wave_value <=439) {
    rz = (wave_value-440)/(440-380);
    gz = 0;
    bz = 1;
  }
  
  if(wave_value >=440 && wave_value <=489) {
    rz = 0;
    gz = (wave_value-440)/(490-440);
    bz = 1;
  }
  
  if(wave_value >=490 && wave_value <=509) {
    rz = 0;
    gz = 1;
    bz = (wave_value-510)/(510-490);
  }
  
  if(wave_value >=510 && wave_value <=579) {
    rz = (wave_value-510)/(580-510);
    gz = 1;
    bz = 0;
  }
  
  if(wave_value >=580 && wave_value <=644) {
    rz = 1;
    gz = (wave_value-645)/(645-580);
    bz = 0;
  }
  
  if(wave_value >=645 && wave_value <=780) {
    rz = 1;
    gz = 0;
    bz = 0;
  }
  
  // Convert RGB decimal value to 0-254 value
  r = rz * 255;
  b = bz * 255;
  g = gz * 255;

  // Shift RGB values based on color_state  
  if(color_state == 0){
    color_array[0] = b;
    color_array[1] = g;
    color_array[2] = r;
  }
  if(color_state == 1){
    color_array[0] = r;
    color_array[1] = g;
    color_array[2] = b;
  }
  if(color_state == 2){
    color_array[0] = g;
    color_array[1] = r;
    color_array[2] = b;
  }
  
}


void set_pixel_colors(int cur_LED_vals[]) {

  int i;
  int useColor[3] = {0, 0, 0}; // Instantiate color array

  for (i = 0; i < CNT_LIGHTS; i++) {

    if(cur_LED_vals[i] > 500) {
      get_RGB(get_wave_length(cur_LED_vals[i]), useColor);
      pixels.setPixelColor( i, useColor[0], useColor[1], useColor[2] );  //this just sets the white to ZERO
    } else {
      pixels.setPixelColor( i, 0, 0, 0 );
    }//if
    
  }//for

}
/*
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}*/

