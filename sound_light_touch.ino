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
#define TOUCHING_1    1
#define TOUCHING_2    2
#define TOUCHING_3    3

#define ROUTINE_NONE 0
#define ROUTINE_1    1
#define ROUTINE_2    2
#define ROUTINE_3    3

Adafruit_NeoPixel stripLEDs = Adafruit_NeoPixel(PIXEL_COUNT, LIGHTS_PIN, NEO_RGB);

int myFeed = 0;

TweenDuino::Timeline flickerTimeline;
TweenDuino::Timeline riseTimeline;

uint32_t stripColor1, stripColor2;  // What colors are in use in the strips.
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

  buildFlicker(flickerTimeline);
  buildRiseFall(riseTimeline);

  flickerTimeline.begin(millis());
  riseTimeline.begin(millis());

  stripLEDs.clear();
  stripLEDs.show();

  // 3 colors: Red, Green, Blue whose values range from 0-255.
  // These colors might not be in Red-Green-Blue order; depends on the LEDs
  stripColor1 = stripLEDs.Color(120, 255, 50 ); 
  stripColor2 = stripLEDs.Color(66, 132, 10);

  pinMode(ONBOARD_LED, OUTPUT);
}

// Tracks which animation/sound we're doing.
int routine = ROUTINE_NONE;
// Tracks whether a routine has been started and we are waiting
// for the related sound to begin (to syncronize the visuals with audio).
bool pendingRoutine = false;

// Used to make sure we don't sit locked up for too long
// if something goes wrong and the Routine's sound never
// plays.
long routineStart = 0;
const long MAX_ROUTINE_WAIT = 1000; // millisecs

void loop() {
  long loopStart = millis();
  flickerTimeline.update(loopStart);
  riseTimeline.update(loopStart);

  bool playing = isPlaying();

  // Clear "routine" if playing has stopped and we aren't
  // waiting for it to start.
  if (routine != ROUTINE_NONE && !pendingRoutine && !playing) {
    Serial.println("Clearing routine");
    routine = ROUTINE_NONE;
    stripLEDs.clear();
    stripLEDs.show();
  }

  switch (routine) {
    case ROUTINE_1:
      doTubeRoutine(playing);
      break;
    case ROUTINE_2:
      doSplatterRoutine(playing);
      break;
    case ROUTINE_3:
      doFarRoutine(playing);
      break;
    default:
      break;
  }
  
  unsigned int touched = readTouches();

  // Exit the loop early to skip touch sensing if we
  // are still in the middle of doing something.
  if (routine != ROUTINE_NONE) {
    return;
  }

  switch (touched) {
    case TOUCHING_1:
      startTubeRoutine();
      routineStart = loopStart;
      break;
    case TOUCHING_2:
      startSplatterRoutine();
      routineStart = loopStart;
      break;
    case TOUCHING_3:
      startFarRoutine();
      routineStart = loopStart;
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
float flickerBrightness = 0.0f;
void buildFlicker(TweenDuino::Timeline &timeline) {
  timeline.addTo(flickerBrightness, 0.7, 25);
  timeline.addTo(flickerBrightness, 0.2, 25);
}

float overallBrightness = 0.0f;
void buildRiseFall(TweenDuino::Timeline &timeline) {
  timeline.addTo(overallBrightness, 0.10, 0);
  timeline.addTo(overallBrightness, 0.6, 238);
  timeline.addTo(overallBrightness, 0.10, 167);
  timeline.addTo(overallBrightness, 1.0, 169);
  timeline.addTo(overallBrightness, 0.10, 590);
  timeline.addTo(overallBrightness, 1.0, 365);
  timeline.addTo(overallBrightness, 0.10, 825);
  timeline.addTo(overallBrightness, 1.0, 722);
  timeline.addTo(overallBrightness, 0.10, 715);
}


// from Bill Earl's "Multi-tasking the Arduino - Part 3"
// https://learn.adafruit.com/multi-tasking-the-arduino-part-3
const unsigned long THEATER_INTERVAL = 50;
unsigned long lastUpdate = 0;
unsigned int animIndex = 0;
void startTubeRoutine() {
  Serial.println("Tube start.");
  digitalWrite(SOUND1_PIN, LOW);
  digitalWrite(ONBOARD_LED, LOW);

  digitalWrite(SOUND2_PIN, HIGH);
  digitalWrite(SOUND3_PIN, HIGH);

  routine = ROUTINE_1;
  pendingRoutine = true;
  lastUpdate = millis();
}

void doTubeRoutine (bool isPlaying) {
  if (pendingRoutine && isPlaying) {
    pendingRoutine = false;

    digitalWrite(SOUND1_PIN, HIGH);
    digitalWrite(SOUND2_PIN, HIGH);
    digitalWrite(SOUND3_PIN, HIGH);
  }

  if ((millis() - lastUpdate) <= THEATER_INTERVAL) {
    return;
  }

  lastUpdate = millis();
  animIndex++;
  if (animIndex >= PIXEL_COUNT ) {
    animIndex = 1;
  }

  for(int i=0; i < PIXEL_COUNT; i++)
  {
      if ((i + animIndex) % 3 == 0)
      {
          stripLEDs.setPixelColor(i, stripColor1);
      }
      else
      {
          stripLEDs.setPixelColor(i, stripColor2);
      }
  }
  stripLEDs.show();
}

void startSplatterRoutine() {
  Serial.println("Splatter start.");
  digitalWrite(SOUND2_PIN, LOW);
  digitalWrite(ONBOARD_LED, LOW);
  
  digitalWrite(SOUND1_PIN, HIGH);
  digitalWrite(SOUND3_PIN, HIGH);

  routine = ROUTINE_2;
  pendingRoutine = true;
  lastUpdate = millis();
}

const unsigned long MARCH_INTERVAL = 500;
void doSplatterRoutine(bool isPlaying) {
  if (pendingRoutine && isPlaying) {
    pendingRoutine = false;

    digitalWrite(SOUND1_PIN, HIGH);
    digitalWrite(SOUND2_PIN, HIGH);
    digitalWrite(SOUND3_PIN, HIGH);
  }

  if ((millis() - lastUpdate) <= MARCH_INTERVAL) {
    return;
  }

  lastUpdate = millis();
  animIndex++;
  if (animIndex >= PIXEL_COUNT ) {
    animIndex = 0;
  }

  for(int i=0; i < PIXEL_COUNT; i++)
  {
      if ((i + animIndex) % 2 == 0)
      {
          stripLEDs.setPixelColor(i, stripColor1);
      }
      else
      {
          stripLEDs.setPixelColor(i, stripColor2);
      }
  }
  stripLEDs.show();
}

void startFarRoutine() {
  Serial.println("Far start.");
  digitalWrite(SOUND3_PIN, LOW);
  digitalWrite(ONBOARD_LED, LOW);
  flickerTimeline.restartFrom(millis());
  riseTimeline.restartFrom(millis());

  digitalWrite(SOUND1_PIN, HIGH);
  digitalWrite(SOUND2_PIN, HIGH);

  routine = ROUTINE_3;
  pendingRoutine = true;
}

void doFarRoutine(bool isPlaying) {
  if (pendingRoutine && isPlaying) {
    pendingRoutine = false;
    digitalWrite(SOUND1_PIN, HIGH);
    digitalWrite(SOUND2_PIN, HIGH);
    digitalWrite(SOUND3_PIN, HIGH);
  }

  float brightness = overallBrightness * flickerBrightness;
  Serial.print("Rbightness: "); Serial.println(brightness);

  if (flickerTimeline.isComplete()) {
    flickerTimeline.restartFrom(millis());
  }
  
  stripLEDs.fill(stripLEDs.Color(120 * brightness, 255* brightness, 50* brightness));
  stripLEDs.show();
}
