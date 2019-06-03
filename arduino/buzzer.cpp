#include <bluefruit.h>


#define BUZZER_PIN 27

/*********************************************************************************
  Buzzer
*********************************************************************************/
static SoftwareTimer buzzer;

// Callback for buzzer control @1khz
static void buzzerEvent(TimerHandle_t xTimerID) {
  digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
}

void buzzerStart() {
  pinMode(BUZZER_PIN, OUTPUT);
  buzzer.begin(1, buzzerEvent);
  buzzer.start();
}

void buzzerStop() {
  buzzer.stop();
  digitalWrite(BUZZER_PIN, 0);
}
