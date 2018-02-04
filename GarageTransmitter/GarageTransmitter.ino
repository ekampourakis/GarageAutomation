//Transmitter

#include <VirtualWire.h>

#define SensorPin 3
#define RelayPin 5
#define SwitchPin 7
#define TXPin 2

#define RelayOn LOW
#define RelayOff HIGH

#define SensorClosed LOW

bool RelayState = false;
bool SensorState;

unsigned long lastDebounceTime = 0;
const int debounceDelay = 100; 
int buttonState;
int lastButtonState;
int startButtonState;

unsigned long LastTransmission = 0;
#define TransmissionDelay 500

#define LightDuration 90 //in minutes
unsigned long LatestLightOn = 0;

char *Data;

void setup() {
  Serial.begin(9600);
  pinMode(SensorPin, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  SensorState = digitalRead(SensorPin);
  pinMode(SwitchPin, INPUT_PULLUP);
  startButtonState = digitalRead(SwitchPin);
  lastButtonState = startButtonState;
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(TXPin);
  vw_setup(4000);
}

void OverflowMillis() {
  while (millis() < 10) {
    lastDebounceTime = 0;
    LastTransmission = 0;
    LatestLightOn = 0;
    delay(1);
  }
}

void InvertRelay() {
  digitalWrite(RelayPin, RelayState ? RelayOff : RelayOn);
  RelayState = !RelayState;
  if (RelayState) {LatestLightOn = millis();}
}

void CheckButton() {
  int reading = digitalRead(SwitchPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState != startButtonState) {
        startButtonState = buttonState;
        InvertRelay();
      }
    }
  }
  lastButtonState = reading;
}

void AutoLight() {
  if (RelayState) {
    if (millis() > LatestLightOn + (LightDuration * 60000)) {
      InvertRelay();
    }
  }
}

void CheckSensor() {
  SensorState = (digitalRead(SensorPin) == SensorClosed) ? true : false;
}

bool l = false;

void Transmit() {
  if (millis() > LastTransmission + TransmissionDelay) {
    LastTransmission = millis();
    if (SensorState && RelayState) {Data = 255;} else 
    if (SensorState && !RelayState) {Data = 204;} else
    if (!SensorState && RelayState) {Data = 51;} else
    if (!SensorState && !RelayState) {Data = 0;}
    vw_send((uint8_t *)Data, strlen(Data));
    vw_wait_tx();
    digitalWrite(13, l ? HIGH : LOW);
    l = !l;
    Serial.println("Sent");
  }
}

void loop(){
  OverflowMillis();
  CheckButton();
  AutoLight();
  CheckSensor();
  Transmit();
}
