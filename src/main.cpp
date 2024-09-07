#include <Arduino.h>
#include <math.h>

#define N_LIGHTS 3
#define NORMAL_DELAY 300
#define ACCELERATION_TIME 5000
#define TIMETRAVEL_TIME 5000
#define MIN_ACCELERATION 100
#define SWITCH_DEBOUNCE 200

#define LIGHT_PIN_0 18
#define LIGHT_PIN_1 19
#define LIGHT_PIN_2 20
#define BUTTON_PIN 21

typedef enum {
  normal,
  accelerate,
  timetravel,
} fmode;

int pins[N_LIGHTS] = {LIGHT_PIN_0,LIGHT_PIN_1,LIGHT_PIN_2};
fmode mode = normal;
unsigned long nextLight = 0;
unsigned long modeStarted = 0;
int currentPin = 0;
unsigned long switchDebounce = 0;

void setLight(int i);
void setAllLights(bool on);

void setup() {
  Serial1.begin(115200);
  for (int j = 0; j < N_LIGHTS; j++) {
    pinMode(pins[j], OUTPUT); 
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  switch (mode)
  {
  case normal:
    if (millis() >= nextLight) {
      setLight(currentPin % N_LIGHTS);
      currentPin++;
      nextLight = millis() + NORMAL_DELAY;
    }
    Serial1.println(digitalRead(BUTTON_PIN));
    if (digitalRead(BUTTON_PIN) == LOW && (millis() - switchDebounce) > SWITCH_DEBOUNCE) {
      switchDebounce = millis();
      mode = accelerate;
      modeStarted = millis();
      Serial1.println("accelerating");
    }
    break;
  case accelerate:
    if (millis() >= nextLight) {
      setLight(currentPin % N_LIGHTS);
      currentPin++;
      double countdown = 1 - (double(millis() - modeStarted) / double(ACCELERATION_TIME));
      double delayMultiplier = min(1, (pow(countdown, 4) + 0.25));
      if (delayMultiplier <= 0.26) {
        mode = timetravel;
        modeStarted = millis();
        Serial1.println("88 miles per hour!");
      }
      nextLight = millis() + (unsigned long)(NORMAL_DELAY * delayMultiplier);
    }
    break;
  case timetravel:
    setAllLights((millis() / 1000) % 2 == 0);
    if (millis() - modeStarted >= TIMETRAVEL_TIME) {
      mode = normal;
      setAllLights(false);
      delay(1000);
    }
  }
}

void setLight(int i) {
  for (int j = 0; j < N_LIGHTS; j++) {
    digitalWrite(pins[j], j == i ? HIGH : LOW);
  }
}

void setAllLights(bool on) {
  for (int j = 0; j < N_LIGHTS; j++) {
    digitalWrite(pins[j], on ? HIGH : LOW);
  }
}