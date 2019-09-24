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
#define TOUCHING_NONE 0
#define TOUCHING_1  1
#define TOUCHING_2  2
#define TOUCHING_3  3

Adafruit_NeoPixel stripLEDs = Adafruit_NeoPixel(PIXEL_COUNT, LIGHTS_PIN, NEO_RGB);

// Start dim (0).
float brightness = 0.0;

int myFeed = 0;

TweenDuino::Timeline timeline;

void setup() {
  Serial.begin(115200);

  // Set the 3 touch pins to "input" mode.
  pinMode(TOUCH1_PIN, INPUT);
  pinMode(TOUCH2_PIN, INPUT);
  pinMode(TOUCH3_PIN, INPUT);

  // Set the 3 audio trigger pins to output.
  pinMode(SOUND1_PIN, OUTPUT);
  pinMode(SOUND2_PIN, OUTPUT);
  pinMode(SOUND3_PIN, OUTPUT);

  digitalWrite(SOUND1_PIN, HIGH);
  digitalWrite(SOUND2_PIN, HIGH);
  digitalWrite(SOUND3_PIN, HIGH);

  // Set the playing/active pin to input.
  pinMode(PLAYING_PIN, INPUT);

  stripLEDs.begin();

  addTweensTo(timeline);

  timeline.begin(millis());

  stripLEDs.clear();
  stripLEDs.show();

  pinMode(ONBOARD_LED, OUTPUT);
}

void loop() {
  long loopStart = millis();
  timeline.update(loopStart);

  bool playing = isPlaying();
  Serial.print("AIs Playing: ");
  Serial.println(playing);

  unsigned int touched = readTouches();

  switch (touched) {
    case TOUCHING_1:
      doTubeRoutine();
      break;
    case TOUCHING_2:
      doSplatterRoutine();
      break;
    case TOUCHING_3:
      doFarRoutine();
      break;
    case TOUCHING_NONE:
      digitalWrite(SOUND1_PIN, HIGH);
      digitalWrite(SOUND2_PIN, HIGH);
      digitalWrite(SOUND3_PIN, HIGH);
      digitalWrite(ONBOARD_LED, HIGH);
      break;
    default:
      Serial.println("Error! Unrecognized input state.");
      break;
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

  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, color);
  }
}

/**
   If any of the sensors is reading HIGH, return the touch number.
*/
int readTouches () {
  if (digitalRead(TOUCH1_PIN) == LOW)
    return TOUCHING_1;
  if (digitalRead(TOUCH2_PIN) == LOW)
    return TOUCHING_2;
  if (digitalRead(TOUCH3_PIN) == LOW)
    return TOUCHING_3;

  // Else...
  return TOUCHING_NONE;
}

bool isPlaying () {
  return digitalRead(PLAYING_PIN) == LOW;
}

// Make a pattern.
void addTweensTo(TweenDuino::Timeline &timeline) {

  timeline.addTo(brightness, 1.0, 50);
  timeline.addTo(brightness, 0.25, 50);
  timeline.addTo(brightness, 1.0, 50);
}

void doTubeRoutine () {
    Serial.println("Tube touch.");
    digitalWrite(SOUND1_PIN, LOW);
    digitalWrite(ONBOARD_LED, LOW);
    timeline.restartFrom(millis());

    digitalWrite(SOUND2_PIN, HIGH);
    digitalWrite(SOUND3_PIN, HIGH);
}

void doSplatterRoutine() {
    Serial.println("Splatter touch.");
    digitalWrite(SOUND2_PIN, LOW);
    digitalWrite(ONBOARD_LED, LOW);
    timeline.restartFrom(millis());

    digitalWrite(SOUND1_PIN, HIGH);
    digitalWrite(SOUND3_PIN, HIGH);
}

void doFarRoutine() {
    Serial.println("Far touch.");
    digitalWrite(SOUND3_PIN, LOW);
    digitalWrite(ONBOARD_LED, LOW);
    timeline.restartFrom(millis());

    digitalWrite(SOUND1_PIN, HIGH);
    digitalWrite(SOUND2_PIN, HIGH);
}
