/*
 * Three-sensor line follower for Arduino Uno R3.
 * SELECT starts or stops a run. RIGHT switches between sensor and statistics pages.
 * With the wheels raised, check motor direction and the sensor pattern on black and white.
 * Tune centred driving, then corner corrections and lost-line recovery.
 * L/R shows signed PWM commands. D is the estimated distance.
 */
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdio.h>
#include <string.h>

// 16x2 LCD: RS D8, EN D9, data D4-D7. The keypad resistor ladder uses A0.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Wiring
// The sensor inputs are ordered left, centre and right as seen from behind the car.
const byte LEFT_IR   = 2;
const byte CENTER_IR = A5;
const byte RIGHT_IR  = 12;
const byte KEY_PIN   = A0;

// Left motor: enable D11, direction A1/A2. Right motor: enable D3, direction A3/A4.
const byte LEFT_ENABLE  = 11;
const byte RIGHT_ENABLE = 3;
const byte IN1 = A1;
const byte IN2 = A2;
const byte IN3 = A3;
const byte IN4 = A4;

// On black, the left and right sensors read LOW and the centre sensor reads HIGH.
// Cover one sensor at a time to check 100, 010 and 001. All white should read 000.
// Check the black level after replacing a sensor. A displayed 1 always means black.
const byte LEFT_BLACK_LEVEL   = LOW;
const byte CENTER_BLACK_LEVEL = HIGH;
const byte RIGHT_BLACK_LEVEL  = LOW;

// If a wheel runs backwards during the raised-wheel test, invert its forward setting.
const bool LEFT_FORWARD_HIGH  = true;
const bool RIGHT_FORWARD_HIGH = true;

// ---------- PWM ----------
// 010: use the paired straight-line PWM values (0-255).
// Recheck straight-line drift after changing either value.
const int FAST_LEFT  = 210;
const int FAST_RIGHT = 195;

// 110: turn left by slowing the left wheel.
// 011: turn right by slowing the right wheel. Both wheels still drive forward.
const int LEFT_OUTER_PWM  = 185;
const int RIGHT_OUTER_PWM = 165;

const int LEFT_INNER_PWM  = 75;
const int RIGHT_INNER_PWM = 60;

// 100 or 001: drive the outer wheel and let the inner wheel coast.
// Sharp-turn PWM is named for the driven wheel.
const int LEFT_SHARP_PWM  = 205;  // 001: right turn; drive the left wheel.
const int RIGHT_SHARP_PWM = 185;  // 100: left turn; drive the right wheel.

// On 000, pivot only if the last observed side is still valid.
// Drive the outer wheel forward and reverse the inner wheel.
const int LEFT_SEARCH_PWM   = 165;
const int RIGHT_SEARCH_PWM  = 145;

const int LEFT_REVERSE_PWM  = 165;
const int RIGHT_REVERSE_PWM = 205;

// Before finish detection is armed, cross an all-black section at low speed.
const int LEFT_ALL_BLACK_PWM  = 100;
const int RIGHT_ALL_BLACK_PWM = 80;

// Timing in milliseconds
// Run motor control every 5 ms, refresh the LCD every 100 ms and debounce for 35 ms.
const unsigned long CONTROL_MS = 5;
const unsigned long LCD_MS = 100;
const unsigned long BUTTON_DEBOUNCE_MS = 35;

const unsigned long DIRECTION_MEMORY_MS = 350; // Maximum age of the last observed side.
const unsigned long SEARCH_TIMEOUT_MS = 10000; // Stop searching after this interval.
const unsigned long REVERSE_PAUSE_MS = 40;     // Time with drive disabled before reversing.
const unsigned long FINISH_CONFIRM_MS = 15;    // Minimum continuous all-black reading.
const unsigned long FINISH_ARM_MS = 18000UL; // Delay before finish detection is armed.

// Distance calibration
// Each wheel uses a coefficient in mm per PWM-second. Nonpositive values disable the display.
// For calibration, divide a measured straight-line distance by the integrated PWM for that wheel.
// Recheck calibration after changes to the wheels, surface or supply. PWM cannot measure slip.
const float LEFT_MM_PER_PWM_SEC  = 2.58881f;
const float RIGHT_MM_PER_PWM_SEC = 2.45061f;

// Motor control
// Positive PWM drives forward, negative PWM reverses and zero disables drive.
struct Motor {
  byte enablePin;
  byte inputA;
  byte inputB;
  bool forwardHigh;

  // Keep the last direction when stopped. Record the shutoff time only once.
  int lastDirection;
  bool outputOn;
  unsigned long offSince;

  void begin() {
    pinMode(enablePin, OUTPUT);
    analogWrite(enablePin, 0);

    pinMode(inputA, OUTPUT);
    pinMode(inputB, OUTPUT);
    digitalWrite(inputA, LOW);
    digitalWrite(inputB, LOW);

    offSince = millis();
  }

  void stop(unsigned long now) {
    if (outputOn) {
      analogWrite(enablePin, 0);
      outputOn = false;
      offSince = now;
    }
  }

  // Return the applied PWM for the display and distance estimate.
  // Disable drive before reversal. Restarting in the same direction needs no wait.
  int drive(int pwm, unsigned long now) {
    pwm = constrain(pwm, -255, 255);

    if (pwm == 0) {
      stop(now);
      return 0;
    }

    int direction = pwm > 0 ? 1 : -1;
    if (lastDirection != 0 && direction != lastDirection) {
      stop(now);
      if (now - offSince < REVERSE_PAUSE_MS) return 0;
    }

    bool firstPinHigh = direction > 0 ? forwardHigh : !forwardHigh;
    digitalWrite(inputA, firstPinHigh ? HIGH : LOW);
    digitalWrite(inputB, firstPinHigh ? LOW : HIGH);
    analogWrite(enablePin, abs(pwm));

    lastDirection = direction;
    outputOn = true;
    return pwm;
  }
};

Motor leftMotor  = {LEFT_ENABLE, IN1, IN2, LEFT_FORWARD_HIGH, 0, false, 0};
Motor rightMotor = {RIGHT_ENABLE, IN3, IN4, RIGHT_FORWARD_HIGH, 0, false, 0};

// Line-following state
bool running = false;
bool finished = false;
bool lost = false;

bool leftBlack = false;
bool centerBlack = false;
bool rightBlack = false;

int leftOutput = 0;
int rightOutput = 0;

// Last line position: -1 left, 1 right, 0 unknown.
int lastSide = 0;
unsigned long lastSideAt = 0;

// Choose the search direction once when entering FIND, using a recent side reading.
// 101 and 000 share a timeout. Only 000 can request a pivot.
bool searching = false;
int searchSide = 0;
unsigned long searchStartedAt = 0;

bool finishCandidate = false;
unsigned long finishSince = 0;

// Keypad and LCD
const byte BUTTON_NONE   = 0;
const byte BUTTON_SELECT = 1;
const byte BUTTON_RIGHT  = 2;
const byte BUTTON_OTHER  = 3;

byte rawButton = BUTTON_NONE;
byte stableButton = BUTTON_NONE;
bool buttonsArmed = false;
unsigned long buttonChangedAt = 0;

// The sensor page shows IR state, the finish countdown and PWM. The statistics page shows time and distance.
// RIGHT switches pages. FIN opens the statistics page. A new run keeps the selected page.
bool statsPage = false;
unsigned long lastControlAt = 0;
unsigned long lastLcdAt = 0;

// Run timing and PWM integration
unsigned long elapsedMs = 0;
unsigned long lastMeasureAt = 0;
float leftPwmSeconds = 0.0f;
float rightPwmSeconds = 0.0f;

void readSensors() {
  leftBlack   = digitalRead(LEFT_IR)   == LEFT_BLACK_LEVEL;
  centerBlack = digitalRead(CENTER_IR) == CENTER_BLACK_LEVEL;
  rightBlack  = digitalRead(RIGHT_IR)  == RIGHT_BLACK_LEVEL;
}

// Integrate the previous motor output before processing new button or motor commands.
// Include FIND in elapsed time but exclude it from the distance estimate.
void measureRun(unsigned long now) {
  unsigned long dtMs = now - lastMeasureAt;
  lastMeasureAt = now;
  if (!running) return;

  elapsedMs += dtMs;
  if (searching) return;

  float seconds = dtMs / 1000.0f;
  leftPwmSeconds  += leftOutput * seconds;
  rightPwmSeconds += rightOutput * seconds;
}

// SELECT starts a run and clears the previous results and search history.
void startRun(unsigned long now) {
  running = true;
  finished = false;
  lost = false;

  searching = false;
  finishCandidate = false;
  lastSide = 0;
  searchSide = 0;

  elapsedMs = 0;
  lastMeasureAt = now;
  leftPwmSeconds = 0.0f;
  rightPwmSeconds = 0.0f;
}

// Disable both motors and keep the latest time and distance on display.
void stopRun(unsigned long now) {
  running = false;
  searching = false;

  leftOutput  = leftMotor.drive(0, now);
  rightOutput = rightMotor.drive(0, now);
}

// Keypad ADC values: RIGHT below 50, SELECT 555-789, released at 790 or above.
// Check the thresholds after replacing the shield. Other keys are debounced but ignored.
byte readButton() {
  int value = analogRead(KEY_PIN);

  if (value < 50) return BUTTON_RIGHT;
  if (value >= 555 && value < 790) return BUTTON_SELECT;
  if (value >= 790) return BUTTON_NONE;
  return BUTTON_OTHER;
}

void updateButtons(unsigned long now) {
  byte button = readButton();

  if (button != rawButton) {
    rawButton = button;
    buttonChangedAt = now;
  }

  if (now - buttonChangedAt < BUTTON_DEBOUNCE_MS) return;

  // Wait for all buttons to be released after power-up before accepting a command.
  if (!buttonsArmed) {
    if (button == BUTTON_NONE) buttonsArmed = true;
    return;
  }

  // Act only on a debounced change, so holding a button does not repeat the action.
  if (button == stableButton) return;
  stableButton = button;

  if (button == BUTTON_RIGHT) {
    statsPage = !statsPage;
  } else if (button == BUTTON_SELECT) {
    if (running) {
      stopRun(now);
    } else {
      startRun(now);
    }
  }
}

void followLine(unsigned long now) {
  // Keep reading the sensors while stopped so their levels can be checked on the LCD.
  readSensors();
  if (!running) return;

  // Evaluate the all-black finish marker only after the arming delay.
  if (leftBlack && centerBlack && rightBlack) {
    if (elapsedMs < FINISH_ARM_MS) {
      finishCandidate = false;
      searching = false;

      // Cross the all-black section slowly and leave FIND.
      leftOutput  = leftMotor.drive(LEFT_ALL_BLACK_PWM, now);
      rightOutput = rightMotor.drive(RIGHT_ALL_BLACK_PWM, now);
      return;
    }

    // After arming, require a continuous 111 reading for FINISH_CONFIRM_MS.
    // Keep the previous motor command while checking. Any other pattern cancels the check.
    if (!finishCandidate) {
      finishCandidate = true;
      finishSince = now;
    }

    if (now - finishSince >= FINISH_CONFIRM_MS) {
      stopRun(now);
      finished = true;
      statsPage = true;
    }

    return;
  }
  finishCandidate = false;

  int leftTarget = 0;
  int rightTarget = 0;

  if (!leftBlack && centerBlack && !rightBlack) {
    // 010: centred on the line.
    searching = false;
    leftTarget = FAST_LEFT;
    rightTarget = FAST_RIGHT;

  } else if (leftBlack != rightBlack) {
    // Steer towards the outer sensor that detects black.
    searching = false;
    // Only a side reading updates direction memory. A centred reading lets it expire.
    lastSide = leftBlack ? -1 : 1;
    lastSideAt = now;

    if (leftBlack) {
      leftTarget  = centerBlack ? LEFT_INNER_PWM : 0;
      rightTarget = centerBlack ? RIGHT_OUTER_PWM : RIGHT_SHARP_PWM;
    } else {
      leftTarget  = centerBlack ? LEFT_OUTER_PWM : LEFT_SHARP_PWM;
      rightTarget = centerBlack ? RIGHT_INNER_PWM : 0;
    }

  } else {
    // 000 has lost the line. For ambiguous 101, disable drive and wait.
    if (!searching) {
      searching = true;
      searchStartedAt = now;
      searchSide = 0;

      if (lastSide != 0 && now - lastSideAt <= DIRECTION_MEMORY_MS) {
        searchSide = lastSide;
      }
    }

    if (now - searchStartedAt >= SEARCH_TIMEOUT_MS) {
      stopRun(now);
      lost = true;
      return;
    }

    bool allWhite = !leftBlack && !centerBlack && !rightBlack;

    // For 000, request a pivot. The motor driver handles the reversal delay.
    // If the direction is unknown, hold both motors off until the line returns or the search times out.
    if (allWhite && searchSide < 0) {
      leftTarget  = -LEFT_REVERSE_PWM;
      rightTarget = RIGHT_SEARCH_PWM;
    } else if (allWhite && searchSide > 0) {
      leftTarget  = LEFT_SEARCH_PWM;
      rightTarget = -RIGHT_REVERSE_PWM;
    }
  }

  leftOutput  = leftMotor.drive(leftTarget, now);
  rightOutput = rightMotor.drive(rightTarget, now);
}

void printLcdRow(byte row, const char *message) {
  lcd.setCursor(0, row);
  lcd.print(message);

  // Pad the row to clear characters left by the previous message.
  for (size_t col = strlen(message); col < 16; col++) {
    lcd.print(' ');
  }
}

void updateLcd() {
  // Reserve one extra byte for the string terminator.
  char row[17];
  const char *status = "STOP";

  if (finished) {
    status = "FIN";
  } else if (lost) {
    status = "LOST";
  } else if (running) {
    status = searching ? "FIND" : "RUN";
  }

  if (statsPage) {
    snprintf(row, sizeof(row), "T:%lu.%lus %s",
             elapsedMs / 1000UL, (elapsedMs % 1000UL) / 100UL, status);
    printLcdRow(0, row);

    if (LEFT_MM_PER_PWM_SEC > 0.0f && RIGHT_MM_PER_PWM_SEC > 0.0f) {
      // Convert each wheel integral to millimetres and average the two estimates.
      // Display whole metres and a three-digit fractional part.
      float leftMm  = leftPwmSeconds  * LEFT_MM_PER_PWM_SEC;
      float rightMm = rightPwmSeconds * RIGHT_MM_PER_PWM_SEC;
      long distanceMm = (long)((leftMm + rightMm) / 2.0f);

      unsigned long mm = (unsigned long)labs(distanceMm);
      snprintf(row, sizeof(row), "D:%s%lu.%03lu m",
         distanceMm < 0 ? "-" : "", mm / 1000UL, mm % 1000UL);
      printLcdRow(1, row);
    } else {
      printLcdRow(1, "D: CAL NEEDED");
    }
  } else {
    if (running && elapsedMs < FINISH_ARM_MS) {
      // Round the remaining arming time up to whole seconds.
      unsigned long secondsLeft =
          (FINISH_ARM_MS - elapsedMs + 999UL) / 1000UL;

      snprintf(row, sizeof(row), "IR:%d%d%d %s %lus",
              leftBlack ? 1 : 0,
              centerBlack ? 1 : 0,
              rightBlack ? 1 : 0,
              status, secondsLeft);
    } else {
      snprintf(row, sizeof(row), "IR:%d%d%d %s",
              leftBlack ? 1 : 0,
              centerBlack ? 1 : 0,
              rightBlack ? 1 : 0,
              status);
    }
    printLcdRow(0, row);

    snprintf(row, sizeof(row), "L:%4d R:%4d", leftOutput, rightOutput);
    printLcdRow(1, row);
  }
}

void setup() {
  leftMotor.begin();
  rightMotor.begin();

  pinMode(LEFT_IR, INPUT);
  pinMode(CENTER_IR, INPUT);
  pinMode(RIGHT_IR, INPUT);
  pinMode(KEY_PIN, INPUT);

  rawButton = readButton();
  buttonChangedAt = millis();
  lastMeasureAt = buttonChangedAt;

  lcd.begin(16, 2);
  readSensors();
  updateLcd();
}

void loop() {
  unsigned long now = millis();

  // Account for the previous interval before changing button or motor state.
  measureRun(now);
  updateButtons(now);

  if (now - lastControlAt >= CONTROL_MS) {
    lastControlAt = now;
    followLine(now);
  }

  if (now - lastLcdAt >= LCD_MS) {
    lastLcdAt = now;
    updateLcd();
  }
}