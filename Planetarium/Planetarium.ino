// ESP-32S Planetarium Toy Control Program

#include <Arduino.h>
#include <esp_sleep.h>
#include "Button.hpp"



// GPIO Pins
const int MOTOR_FORWARD = 14;
const int MOTOR_BACKWARD = 12;
const int LEFT_BUTTON = 21;
const int RIGHT_BUTTON = 23;
const int POWER_STOP_BUTTON = 27;
const int LEFT_BUTTON_LED = 33;
const int RIGHT_BUTTON_LED = 17;
const int TIMER_LED_30MIN = 13;
const int TIMER_LED_60MIN = 32;
const int LIMIT_SWITCH = 5;
const int METEOR_BUTTON = 25;
const int METEOR_BULB = 26;
const int MAIN_BULB = 2;
const int TIMER_BUTTON = 15;

// States
bool isOn = false;
bool motorRunning = true;
bool motorDirection = true; // true = forward, false = backward

int timerMode = 0;          // 0 = Always On, 1 = 30 Min, 2 = 60 Min

unsigned long _timerStart = 0;
unsigned long _timerDuration = 0;

bool isLongPress = false;
bool lastState_Timer = HIGH;
bool lastState_Meteor = HIGH;
bool lastState_Left = HIGH;
bool lastState_Right = HIGH;
bool lastState_Power = HIGH;

bool state_Meteor = LOW;


Button leftButton(LEFT_BUTTON);
Button rightButton(RIGHT_BUTTON);
Button meteorButton(METEOR_BUTTON);
Button powerButton(POWER_STOP_BUTTON);
Button timerButton(TIMER_BUTTON);


void setup() {
  // List of all GPIO pins to set as LOW
  int gpioPins[] = {
    36,39,34,35,32,33,25,26,27,14,12,13,15,2,0,4,16,17,5,18,19,21,3,1,22,23
  };
  // 9,10,11,6,7,8,
  //20 24 28 29 30 31 37 38 

  // Loop through the list of GPIO pins
  for (int i = 0; i < sizeof(gpioPins) / sizeof(gpioPins[0]); i++) {
    pinMode(gpioPins[i], OUTPUT);  // Set pin as OUTPUT
    digitalWrite(gpioPins[i], LOW); // Set pin to LOW
  }
    // Set pin modes
    pinMode(MOTOR_FORWARD, OUTPUT);
    pinMode(MOTOR_BACKWARD, OUTPUT);
    
    leftButton.init();
    rightButton.init();
    meteorButton.init();
    powerButton.init();
    timerButton.init();

    pinMode(LEFT_BUTTON_LED, OUTPUT);
    pinMode(RIGHT_BUTTON_LED, OUTPUT);
    pinMode(TIMER_LED_30MIN, OUTPUT);
    pinMode(TIMER_LED_60MIN, OUTPUT);
    pinMode(LIMIT_SWITCH, INPUT_PULLUP);
    pinMode(METEOR_BULB, OUTPUT);
    pinMode(MAIN_BULB, OUTPUT);
    
    // Initialize outputs
    digitalWrite(MOTOR_FORWARD, LOW);
    digitalWrite(MOTOR_BACKWARD, LOW);
    digitalWrite(LEFT_BUTTON_LED, LOW);
    digitalWrite(RIGHT_BUTTON_LED, LOW);
    digitalWrite(TIMER_LED_30MIN, LOW);
    digitalWrite(TIMER_LED_60MIN, LOW);
    digitalWrite(METEOR_BULB, HIGH);
    digitalWrite(MAIN_BULB, LOW);
    
}

void meteor(bool on)
{
  digitalWrite(METEOR_BULB, on ? LOW : HIGH);
}

int8_t buttonState(bool &lastState, uint8_t button)
{
  if (digitalRead(button) == LOW) {
    if (lastState == HIGH) {
      lastState = LOW;
      return lastState;
    } else {
      return -1;
    }
  } else {
    if (lastState == LOW) {
      lastState = HIGH;
      return lastState;
    } else {
      return -1;
    }
  }
}

void pressedLeft()
{
  digitalWrite(LEFT_BUTTON_LED, HIGH);
  digitalWrite(RIGHT_BUTTON_LED, LOW);
}

void pressedRight()
{
  digitalWrite(LEFT_BUTTON_LED, LOW);
  digitalWrite(RIGHT_BUTTON_LED, HIGH);
}

void pressedMeteor()
{
  state_Meteor = !state_Meteor;
  digitalWrite(METEOR_BULB, !state_Meteor);
}

void stopMotor()
{
  digitalWrite(LEFT_BUTTON_LED, LOW);
  digitalWrite(RIGHT_BUTTON_LED, LOW);
}

void pressedTimer()
{
  timerMode = (timerMode + 1) % 3;
  _timerStart = millis();
  
  switch (timerMode) {
    case 0:
      _timerDuration = 0;
      digitalWrite(TIMER_LED_30MIN, LOW);
      digitalWrite(TIMER_LED_60MIN, LOW);
      break;
    case 1:
      _timerDuration = 3000;//30 * 60 * 1000;
      digitalWrite(TIMER_LED_30MIN, HIGH);
      digitalWrite(TIMER_LED_60MIN, LOW);
      break;
    case 2:
      _timerDuration = 6000;//60 * 60 * 1000;
      digitalWrite(TIMER_LED_30MIN, LOW);
      digitalWrite(TIMER_LED_60MIN, HIGH);
      break;
  }
}

void checkTimer()
{
  if (timerMode == 0)
    return;
  unsigned long _timerCount = millis() - _timerStart;
  if (_timerCount >= _timerDuration)
  {
    stopMotor();
    timerMode = 0;
    _timerDuration = 0;
    digitalWrite(TIMER_LED_30MIN, LOW);
    digitalWrite(TIMER_LED_60MIN, LOW);
  }
}

void pressedPower()
{
  stopMotor();
  digitalWrite(RIGHT_BUTTON_LED, LOW);
  digitalWrite(LEFT_BUTTON_LED, LOW);
}

void pressedPowerLong()
{
  digitalWrite(RIGHT_BUTTON_LED, HIGH);
  digitalWrite(LEFT_BUTTON_LED, HIGH);
  while(digitalRead(POWER_STOP_BUTTON) == LOW)
  {
    delay(10);
  }
  for (uint8_t i = 255; i > 0; i--)
  {
    analogWrite(RIGHT_BUTTON_LED, i);
    analogWrite(LEFT_BUTTON_LED, i);
    delay(2000/255);
  }
  sleep();
}

void sleep() {
  // Configure the wake-up source
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0); // 0 = LOW
  esp_deep_sleep_start();
}

void loop()
{
  if (leftButton.pressed())
  {
    pressedLeft();
  }

  if (rightButton.pressed())
  {
    pressedRight();
  }
  
  if (meteorButton.pressed())
  {
    pressedMeteor();
  } 
  
  if (timerButton.pressed()) {
    pressedTimer();  
  }

  if (powerButton.pressed()) {
    pressedPower();
  } else if (powerButton.longPressed())
  {
    pressedPowerLong();
  }

  checkTimer();
}
