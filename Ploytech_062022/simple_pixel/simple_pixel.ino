// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 20 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 75 // Time (in milliseconds) to pause between pixels

int buttonState;

void setup() {
  buttonState =0;
  
  for(int i=2;i<6;i++)
  {
    pinMode(i, INPUT);
  }
  pinMode(6,OUTPUT);
  
  pixels.clear(); // Set all pixel colors to 'off'
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  
    //pixels.show();   // Send the updated pixel colors to the hardware.
  
}

void loop() {
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    if(i>1&&i<6)
    {
      buttonState =digitalRead(i);
      if(buttonState!=0)pixels.setPixelColor(i, pixels.Color(0,2,0));
      else pixels.setPixelColor(i, pixels.Color(2,0,0));
    }
    else{
      pixels.setPixelColor(i, pixels.Color(2,0,0));
    }
      
  }
    pixels.show();   // Send the updated pixel colors to the hardware.
    buttonState=0;

    delay(DELAYVAL); // Pause before next pass through loop

}
