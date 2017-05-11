#include <FAB_LED.h>

// Define two LED communication protocols on the same port, D6.
ws2812b<D,6> myWs2812b; // grb  protocol
sk6812<D,6>  mySk6812;  // rgbw protocol

#define NUM_WS2812B_PIXELS 16
#define NUM_SK6812_PIXELS  32
#define NUM_PIXELS (NUM_SK6812_PIXELS + NUM_WS2812B_PIXELS)

// This is a custom drawing routine for the specific configuration
void customSendPiels(grbw pixels) {
  // Don't forget to turn off interrupts if calling functions back-to-back
  __builtin_avr_cli();
  myWs2812b.sendPixels(NUM_WS2812B_PIXELS, &pixels[0]);
  mySk6812b.sendPixels(NUM_SK6812_PIXELS,  &pixels[NUM_WS2812B_PIXELS]);
  SREG = oldSREG;
}

grbw myPixels[NUM_PIXELS] = {};

void loop() {
  update(myPixels);
  customSendPiels(myPixels);
  delay(1000);
}
