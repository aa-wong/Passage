#include <Adafruit_NeoPixel.h>

#define PIN_1 9                    // our control pin
#define PIN_2 13
#define MAX_BRIGHTNESS (255)

#define NUM_LEDS 7

// LED Params
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(NUM_LEDS, PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS, PIN_2, NEO_GRB + NEO_KHZ800);

bool flip = false;

int g = 255;
int b = 0;

// Ultrasound Params
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip1.show();
   strip2.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip1.setPixelColor(Pixel, strip1.Color(red, green, blue));
   strip2.setPixelColor(Pixel, strip2.Color(red, green, blue));
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
 
void setup() {
  // ULTRASOUND SETUP
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  strip1.begin();
  strip2.begin();
}
 
void loop() {

  long duration = sendPulse();
  
  // Convert the time into a distance
  long cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  long inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  reset();

  if (cm < 120 || cm > 1000) {
    return showFlames(55,120,15);
  }

  return colorGradient();
}

void reset() {
  strip1.clear();
  strip2.clear();
}

void sendPulse() {
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
  return pulseIn(echoPin, HIGH);
}

void colorGradient() {
  if (g == 255 || g == 0) {
    flip = !flip;
  }

  if (flip) {
    --g;
    ++b;
  } else {
    ++g;
    --b;
  }
  
  for (int i = 0; i < NUM_LEDS; ++i) {
    strip1.setPixelColor(i, strip1.Color(255, g, b));
    strip1.show();
    strip2.setPixelColor(i, strip2.Color(g, b, 255));
    strip2.show();
  }
}

void showFlames(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
 
  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; ++i) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
   
    if (cooldown > heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
 
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k= NUM_LEDS - 1; k >= 2; --k) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
   
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if (random(255) < Sparking) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for (int j = 0; j < NUM_LEDS; ++j) {
    setPixelHeatColor(j, heat[j]);
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor(int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if (t192 > 0x80) {                     // hottest
    return setPixel(Pixel, 255, 255, heatramp);
  }
  if (t192 > 0x40 ) {             // middle
    return setPixel(Pixel, 255, heatramp, 0);
  }                              // coolest
  return setPixel(Pixel, heatramp, 0, 0);
}
