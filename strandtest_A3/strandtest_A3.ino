
// StrandTest from AdaFruit implemented as a state machine
// pattern change by push button
// By Mike Cook Jan 2016

#define PINforControl   6 // pin connected to the small NeoPixels strip
#define CNT_LIGHTS      149 // number of LEDs on strip

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(CNT_LIGHTS, PINforControl, NEO_GRB + NEO_KHZ800);

unsigned long patternInterval = 50 ; // time between steps in the pattern
unsigned long lastUpdate = 0 ; // for millis() when last update occoured
unsigned long intervals [] = { 10, 150, 150, 25 } ; // speed for each pattern
const byte button = 5; // pin to connect button switch to between pin and ground

// Hardware Variables
int analog_pin_L = 1; // read from multiplexer using analog input 0
int analog_pin_R = 0; // read from multiplexer using analog input 0
int analog_pinpot = 2;// dial for speed
int analog_pot_range = 3;// dial for range
//int stomp = 5;
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
        internalLEDOnly();
        break;
    case 1:
        colorWipe(strip.Color(40, 24, 6)); // light yellow
        //colorWipe(strip.Color(0, 0, 0)); // Blackout
        break;
    case 2: 
        soundReactive_A3();
        break;
    case 3: 
        rainbow();
        break;
/*

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
 */
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
    strip.setPixelColor(0, c);
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

void internalLEDOnly(){ // clear all LEDs
      strip.setPixelColor(0, 100,0,100);
      strip.show();
}

void soundReactive_A3() { // Sound Reactive Visualization based on A3 code.

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

  strip.show();
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
      strip.setPixelColor( i, useColor[0], useColor[1], useColor[2] );  //this just sets the white to ZERO
    } else {
      strip.setPixelColor( i, 0, 0, 0 );
      strip.setPixelColor(0, 100,100,100);
    }  
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
