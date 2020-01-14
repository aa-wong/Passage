#include <Adafruit_NeoPixel.h>

#define PIN 9                    // our control pin
#define MAX_BRIGHTNESS (255)

#define NUM_LEDS 7

// LED Params
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRB + NEO_KHZ800);

uint8_t counter;
boolean countUp = true;

int holdcount = 250;   // in miliseconds
int stepcount = 255;    // in loop() interations, must be less than 255

// color 1... white 
int r1 = 255;
int g1 = 255;
int b1 = 255;

// color 2...  yellow green
int r2 = 100;
int g2 = 210;
int b2 = 0;

// increment variables
float dr = 1.0;
float dg = 1.0;
float db = 1.0;

// current color variables
int cr = 0;
int cg = 0;
int cb = 0;

uint32_t color = strip.Color(cr, cg, cb);

// Ultrasound Params
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;
 
void setup() {
  // LED SETUP
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // figure out the size of the steps
  dr = (float) (r2 - r1) / stepcount;
  dg = (float) (g2 - g1) / stepcount;
  db = (float) (b2 - b1) / stepcount;

  // set the current color to color 1 to get started
  cr = r1;
  cg = g1;
  cb = b1;

  // ULTRASOUND SETUP
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void complexFade() {
  // send current color to shield
  //lg3xCC.jumpToRGB(cr, cg, cb);
  color = strip.Color(cr, cg, cb);
  for (int ii = 0; ii < strip.numPixels(); ii++){
    strip.setPixelColor(ii, color);
    strip.show();
  }  
  // calculate the current rgb
  
  if (countUp) {
   ++counter;
  } else {
   --counter;
  }

  cr = r1 + (counter * dr);   
  cg = g1 + (counter * dg);   
  cb = b1 + (counter * db);   

  // check our limits
  if (counter >= stepcount) {
    countUp = false; 
    delay(holdcount);
  } else if (counter == 0) {
    countUp = true;
    delay(holdcount);
  }
}
 
void loop() {  
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135

  if (cm < 120 || cm > 3000) {
    rainbowCycle(1);
  } else {
    RGBLoop();
  }
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
}

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void RGBLoop() {
  for(int j = 0; j < 3; j++ ) {
    // Fade IN
    for(int k = 0; k < 256; k++) {
      switch(j) {
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
//      delay(3);
    }
    // Fade OUT
    for(int k = 255; k >= 0; k--) {
      switch(j) {
        case 0: setAll(k,0,0); break;
        case 1: setAll(0,k,0); break;
        case 2: setAll(0,0,k); break;
      }
      showStrip();
//      delay(3);
    }
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    showStrip();
    delay(SpeedDelay);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}
