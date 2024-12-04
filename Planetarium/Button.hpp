#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

// Base Button Class
class Button {
private:
    uint8_t _pin;                            // GPIO pin for the button
    unsigned long lastDebounceTime;         // Last time the button state changed
    unsigned long pressStartTime;           // When the button was first pressed
    bool lastState;                         // Last stable state of the button
    bool currentState;                      // Current reading of the button
    const unsigned long debounceDelay = 50; // Debounce delay in milliseconds
    const unsigned long longPressDuration = 1000; // Long press threshold in milliseconds

public:
    Button(uint8_t pin) 
        : _pin(pin), lastDebounceTime(0), pressStartTime(0), lastState(HIGH), currentState(HIGH) {
    }

    void init() {
        pinMode(_pin, INPUT_PULLUP);
    }

    // Checks if the button is pressed (debounced)
    bool pressed() {
        bool reading = digitalRead(_pin);
        if (reading != lastState) {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != currentState) {
                currentState = reading;
                if (currentState == LOW) { // Button pressed
                    pressStartTime = millis(); // Record the time of press
                    return true;
                }
            }
        }

        lastState = reading;
        return false;
    }

    // Checks if the button is currently held for a long press
    bool longPressed() {
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
