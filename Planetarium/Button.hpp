#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

// Base Button Class
class Button {
private:
    uint8_t _pin;                           // GPIO pin for the button
    unsigned long lastDebounceTime_down;    // Last time the button state changed down
    unsigned long lastDebounceTime_up;      // Last time the button state changed up
    unsigned long pressStartTime;           // When the button was first pressed
    bool lastState;                         // Last stable state of the button
    bool currentState;                      // Current reading of the button
    const unsigned long debounceDelay = 50; // Debounce delay in milliseconds
    const unsigned long longPressDuration = 1000; // Long press threshold in milliseconds

public:
    Button(uint8_t pin) 
        : _pin(pin), lastDebounceTime_down(0), lastDebounceTime_up(0), pressStartTime(0), lastState(HIGH), currentState(HIGH) {
    }

    void init() {
        pinMode(_pin, INPUT_PULLUP);
    }

    bool held(){
      return !currentState;
    }

    // Checks if the button is pressed (debounced)
    int pressed() {
        bool reading = digitalRead(_pin);
        if (reading != lastState) {
            lastDebounceTime_down = millis();
        }
        
        if (reading != currentState) {
          if ((millis() - lastDebounceTime_down) > debounceDelay) {    
                currentState = reading;
                if (currentState == LOW) { // Button pressed
                    pressStartTime = millis(); // Record the time of press
                    return 1;
                } else if (currentState == HIGH) { // Button released
                    return -1;
                }
            }
        }

        lastState = reading;
        return 0;
    }

    // Checks if the button is currently held for a long press
    bool longPressed() {
      pressed();
      if (currentState == LOW && (millis() - pressStartTime) >= longPressDuration) {
        return true;
      }
      return false;
    }

    uint8_t pin() {
      return _pin;
    }
};

// ToggleButton Class (inherits Button)
class ToggleButton : public Button {
private:
    bool state; // Toggles between true (on) and false (off)

public:
    ToggleButton(uint8_t pin) : Button(pin), state(false) {}

    bool toggled() {
        if (pressed()) {
            state = !state; // Toggle the state
            return true;
        }
        return false;
    }

    bool getState() const {
        return state;
    }
};

#endif // BUTTON_HPP
