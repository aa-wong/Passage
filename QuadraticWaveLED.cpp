#include "stdint.h"
#include "arduino.h"
#include "QuadraticWaveLED.h"

#define NUMPIXELS 16
int i = 0;

f7p9 QuadraticWaveLED::quadraticWavef7p9(f7p9 x) {
  if (x <= inttof7p9(1)) {
    return mulf7p9(x, x);
  }
  if (x <= inttof7p9(3)) {
    x -= inttof7p9(2);
    x = mulf7p9(x, x);
    x = inttof7p9(0) - x + inttof7p9(2);
    return x;
  }
  x -= inttof7p9(4);
  return mulf7p9(x, x);
}

void QuadraticWaveLED::setup(const Adafruit_NeoPixel strip, const unsigned long waveDuration, const unsigned long pinNumber) {
  this->strip = strip;
  this->waveLen = waveDuration;
  this->inverseWaveLen = floattouf16p16(1.0f / waveDuration);
  pinMode(pinNumber, OUTPUT);
}

void QuadraticWaveLED::update(const unsigned long milli) {
  unsigned long mod = milli % this->waveLen;
  // transform to 0 -- 1 range
  this->value = muluf16p16(inttouf16p16(mod), this->inverseWaveLen);
  //convert to 0--4 range: multiply by 4 by shifting left twice
	this->value = this->value << 2;
	//apply wave
	this->value = quadraticWavef7p9(this->value);
	//convert to 0-255 range
	uint8_t pwm_value = ((int32_t)this->value * floattof23p9(127.5f)) >> 18;
	//write to LED
  this->strip.setBrightness(pwm_value);
  this->strip.setPixelColor(i, this->strip.Color(0, 150, 0));
  this->strip.show();
}
