/*
  Copyright 2017 Sam Lewis

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

class Control {
 public:
  virtual String read();
};

class Pot: public Control {
 public:
  explicit Pot(int pin) {
    this->pin = pin;
    pinMode(this->pin, INPUT);
  }

  String read() {
    int pot = analogRead(this->pin);

    if (pot <= deadzone) {
      pot = deadzone;
    } else if (pot >= potMax - deadzone) {
      pot = potMax - deadzone;
    }
    pot = map(pot, this->deadzone, this->potMax - this->deadzone, 0, 200);
    if (abs(pot - this->previousReading) >= 2) {
      this->previousReading = pot;
    }
    return String((this->previousReading / 2) / 100.0);
  }

 private:
  const int deadzone = 5;
  const int potMax = 1023;
  int pin;
  int previousReading;
};

class Button: public Control {
 public:
  explicit Button(int pin) {
    this->pin = pin;
    pinMode(this->pin, INPUT_PULLUP);
  }

  String read() {
    return digitalRead(this->pin)?"false":"true";
  }

 private:
  int pin;
};

const int stagePin = A0;
const int throttlePin = A3;
const int lightPin = 12;

Pot throttle(throttlePin);
Button stage(stagePin);
Button light(lightPin);

void setup() {
  Serial.begin(9600);
  Serial.flush();
  pinMode(A1, OUTPUT);
  digitalWrite(A1, LOW);
  pinMode(A5, OUTPUT);
  digitalWrite(A5, HIGH);
}

String prevThrottle = "";
String prevStageTrigger = "";
String prevLight = "";

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    if (line.equals("get throttle")) {
      Serial.print("set throttle ");
      Serial.println(throttle.read());
    }
  } else {
    String throttleValue = throttle.read();

    if (!throttleValue.equals(prevThrottle)) {
      prevThrottle = throttleValue;
      Serial.print("set throttle ");
      Serial.println(throttleValue);
    }

    String stageTrigger = stage.read();

    if (!stageTrigger.equals(prevStageTrigger)) {
      prevStageTrigger = stageTrigger;
      Serial.print("set stage ");
      Serial.println(stageTrigger);
    }

    String lights = light.read();

    if (!lights.equals(prevLight)) {
      prevLight = lights;
      Serial.print("set lights ");
      Serial.println(lights);
    }
  }
}
