#define KILL_SWITCH 6
#define BUTTON A4

void setup() {
  pinMode(KILL_SWITCH, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  digitalWrite(KILL_SWITCH, HIGH);
}

void loop() {
  if (digitalRead(BUTTON) == LOW) {
    digitalWrite(KILL_SWITCH, LOW);
  }
}
