#include <Arduino.h>
#include <DigiKeyboard.h>


#define DATA_PIN 2
#define LPG_PIN 1
#define GAS_PIN 0

#define NUMBER_OF_CYLINDERS 4
#define FIRES_PER_REV (360 / (720 / NUMBER_OF_CYLINDERS))
#define UPDATE_INTERVAL 300
#define HOLD_INTERVAL 3000
#define RPM_LIMIT 1500

unsigned long lastUpdateTime = 0;
volatile int sparkFireCount = 0;
int cutCount = 0;

void IncrementRpmCount () {
  sparkFireCount++;
}

void Print_Debug(int rpm, int gas, int cutCount) {
  DigiKeyboard.sendKeyStroke(0);
  DigiKeyboard.print("Check :");
  DigiKeyboard.print(sparkFireCount);
  DigiKeyboard.print(", RPM :");
  DigiKeyboard.print(rpm); 
  DigiKeyboard.print(", GAS :");
  DigiKeyboard.print(gas);
  DigiKeyboard.print(", C :");
  DigiKeyboard.println(cutCount); 
}

void Init_Pins()
{
  pinMode(LPG_PIN, OUTPUT);
  pinMode(GAS_PIN, INPUT_PULLUP);
  pinMode(DATA_PIN, INPUT_PULLUP);
  digitalWrite(LPG_PIN, HIGH);
}

/*
 * Defines led pins as output,
 * turns all leds on for 500ms when started
 * attach to serial if available
 */
void setup() {
  Init_Pins();
  attachInterrupt(0, IncrementRpmCount, CHANGE);
}

void loop() {
  if ((millis() - lastUpdateTime) > UPDATE_INTERVAL) {
    int currentRpm = ((sparkFireCount * (1000 / UPDATE_INTERVAL) * 60) / FIRES_PER_REV) / 1.65;
    int gasState = digitalRead(GAS_PIN);
    if ((gasState == HIGH) || currentRpm < RPM_LIMIT) {
      digitalWrite(LPG_PIN, HIGH);
      cutCount = 0;
    } else {
      cutCount++;
      if (cutCount * UPDATE_INTERVAL >= HOLD_INTERVAL) {
        digitalWrite(LPG_PIN, LOW);
        cutCount = 0;
      }
    }

    //Print_Debug(currentRpm, gasState, cutCount);

    sparkFireCount = 0;
    lastUpdateTime = millis();
  }
}
