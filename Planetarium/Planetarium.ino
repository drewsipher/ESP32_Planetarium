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
int motorDirection = 0; // 1 = forward, -1 = backward

bool timerMode = 0;          // false = 30 Min, true = 60 Min

unsigned long _timerStart = 0;
unsigned long _timerDuration = 0;

bool isLongPress = false;
bool lastState_Timer = HIGH;
bool lastState_Meteor = HIGH;
bool lastState_Left = HIGH;
bool lastState_Right = HIGH;
bool lastState_Power = HIGH;

bool state_Meteor = LOW;

uint8_t bulb_brightness = 128;


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
    pinMode (LIMIT_SWITCH, INPUT_PULLUP);
    
    leftButton.init();
    rightButton.init();
    meteorButton.init();
    powerButton.init();
    timerButton.init();

    pinMode(LEFT_BUTTON_LED, OUTPUT);
    pinMode(RIGHT_BUTTON_LED, OUTPUT);
    pinMode(TIMER_LED_30MIN, OUTPUT);
    pinMode(TIMER_LED_60MIN, OUTPUT);
    pinMode(METEOR_BULB, OUTPUT);
    pinMode(MAIN_BULB, OUTPUT);
    
    // Initialize outputs
    digitalWrite(LEFT_BUTTON_LED, LOW);
    digitalWrite(RIGHT_BUTTON_LED, LOW);
    digitalWrite(TIMER_LED_30MIN, LOW);
    digitalWrite(TIMER_LED_60MIN, LOW);
    digitalWrite(METEOR_BULB, HIGH);
    analogWrite(MAIN_BULB, bulb_brightness);

    motorOff();
    pressedTimer(); //Set the timer to 60 min
}

void motorLeft()
{
  motorDirection = -1;
  digitalWrite(MOTOR_FORWARD, HIGH);
  digitalWrite(MOTOR_BACKWARD, LOW);
}

void motorRight()
{
  motorDirection = 1;
  digitalWrite(MOTOR_FORWARD, LOW);
  digitalWrite(MOTOR_BACKWARD, HIGH);
}

void motorOff()
{
  motorDirection = 0;
  digitalWrite(MOTOR_FORWARD, LOW);
  digitalWrite(MOTOR_BACKWARD, LOW);
}

void pressedLeft()
{
  digitalWrite(LEFT_BUTTON_LED, HIGH);
  digitalWrite(RIGHT_BUTTON_LED, LOW);
  if (motorDirection == 1)
  {
    motorOff();
    delay(250);
  }
  motorLeft();
}

void pressedRight()
{
  digitalWrite(LEFT_BUTTON_LED, LOW);
  digitalWrite(RIGHT_BUTTON_LED, HIGH);
  if (motorDirection == -1)
  {
    motorOff();
    delay(250);
  }
  motorRight();
}

void pressedMeteor()
{
  state_Meteor = !state_Meteor;
  digitalWrite(METEOR_BULB, !state_Meteor);
}

void pressedTimer()
{
  timerMode = !timerMode;
  _timerStart = millis();
  
  if (timerMode) {
    _timerDuration = 60 * 60 * 1000;
    digitalWrite(TIMER_LED_30MIN, LOW);
    digitalWrite(TIMER_LED_60MIN, HIGH);
  }
  else {
    _timerDuration = 30 * 60 * 1000;
    digitalWrite(TIMER_LED_30MIN, HIGH);
    digitalWrite(TIMER_LED_60MIN, LOW);
  }
}

void checkTimer()
{
  if (timerMode == 0)
    return;
  unsigned long _timerCount = millis() - _timerStart;
  if (_timerCount >= _timerDuration)
  {
     pressedPowerLong();
  }
}

void pressedPower()
{
  motorOff();
  digitalWrite(RIGHT_BUTTON_LED, LOW);
  digitalWrite(LEFT_BUTTON_LED, LOW);
  //Turn off meteor if on
  if (state_Meteor)
  {
    pressedMeteor();
  }
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
    analogWrite(MAIN_BULB, min(i,bulb_brightness));
    delay(2000/255);
  }
  sleep();
}

void sleep() {
  // Configure the wake-up source
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0); // 0 = LOW
  esp_deep_sleep_start();
}

void increaseBulbBrightness()
{
  if (bulb_brightness == 255) return;
  bulb_brightness++;
  analogWrite(MAIN_BULB, bulb_brightness);
}

void decreaseBulbBrightness()
{
  if (bulb_brightness == 0) return;
  bulb_brightness--;
  analogWrite(MAIN_BULB, bulb_brightness);
}

void loop()
{

  if (rightButton.pressed() == -1)
  {
    pressedRight();
  }
  if (rightButton.longPressed())
  {
    while(rightButton.held())
    {
      //make sure the button can be marked as released
      rightButton.pressed();
      increaseBulbBrightness();
      delay(8);
    }
  }

  if (leftButton.pressed() == -1)
  {
    pressedLeft();
  } 
  if (leftButton.longPressed())
  {
    while(leftButton.held())
    {
      //make sure the button can be marked as released
      leftButton.pressed();
      decreaseBulbBrightness();
      delay(8);
    }
  }
  
  if (meteorButton.pressed() == 1)
  {
    pressedMeteor();
  } 
  
  if (timerButton.pressed() == 1) 
  {
    pressedTimer();  
  }

  if (powerButton.pressed() == 1) 
  {
    pressedPower();
  } 
  if (powerButton.longPressed())
  {
    pressedPowerLong();
  }

  if (digitalRead(LIMIT_SWITCH) == HIGH)
  {
    pressedPowerLong();
  }

  checkTimer();
}
