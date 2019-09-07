#include <Timeline.h>
#include <Tween.h>
#include <TweenDuino.h>

/***************************************************
  Sound Light Touch Sourcecode

  Written by Weston Wedding as a contribution to 
  Tabitha Darrah's Sept 2019 installation.

  Based at least in part on example code
  written by Tony DiCola for Adafruit Industries.
 ****************************************************/
#include <TweenDuino.h>
#include <Adafruit_NeoPixel.h>

/************************** Configuration ***********************************/

// edit the config.h tab to change pin configurations
#include "config.h"

/*************************** Sketch Code ************************************/
int remoteVal = 0;
bool remoteWasTouched = false;

Adafruit_NeoPixel stripLEDs = Adafruit_NeoPixel(PIXEL_COUNT, LIGHTS_PIN, NEO_RGB);

// Start dim (0).
float brightness = 0.0;

int myFeed = 0;

TweenDuino::Timeline timeline;

void setup() {
  Serial.begin(115200);

  // Set the 5 touch pins to "input" mode.
  pinMode(TOUCH1_PIN, INPUT);
  pinMode(TOUCH2_PIN, INPUT);
  pinMode(TOUCH3_PIN, INPUT);
  pinMode(TOUCH4_PIN, INPUT);
  pinMode(TOUCH5_PIN, INPUT);
  
  stripLEDs.begin();

  addTweensTo(timeline);

  timeline.begin(millis());

  stripLEDs.clear();
  stripLEDs.show();
  
  pinMode(ONBOARD_LED, OUTPUT);
}

void loop() {
  long loopStart = millis();

  Serial.println(loopStart);
  timeline.update(loopStart);

  bool touched = readTouches();
  if (touched) {
    digitalWrite(ONBOARD_LED, HIGH);
    timeline.restartFrom(millis());
  } else {
    digitalWrite(ONBOARD_LED, LOW);
  }

  if (timeline.isComplete()) {
    brightness = 0.0;
  }

  // 3 colors: Red, Green, Blue whose values range from 0-255.
  const uint32_t stripColor = stripLEDs.Color(108 * brightness, 235 * brightness, 10 * brightness, 0); // Colors are off, white LED is pure white.
  
  setStripColors(stripLEDs, stripColor);

  // This sends the updated pixel color to the hardware.
  stripLEDs.show();
}

void setStripColors(Adafruit_NeoPixel &strip, uint32_t color) {
  const int numPixels = strip.numPixels();

  for (int i=0;i<numPixels;i++) {
    strip.setPixelColor(i, color);
  }
}

/**
 * If any of the sensors is reading HIGH, return true.
 */
bool readTouches () {
  if (digitalRead(TOUCH1_PIN) == HIGH)
    return true;
  if (digitalRead(TOUCH2_PIN) == HIGH)
    return true;
  if (digitalRead(TOUCH3_PIN) == HIGH)
    return true;
  if (digitalRead(TOUCH4_PIN) == HIGH)
    return true;
  if (digitalRead(TOUCH5_PIN) == HIGH)
    return true;

  // Else...
  return false;
}

// Make a pattern.
void addTweensTo(TweenDuino::Timeline &timeline) {

 timeline.addTo(brightness, 1.0, 50);
  timeline.addTo(brightness, 0.25, 50);
  timeline.addTo(brightness, 1.0, 50);
}
