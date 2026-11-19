/*
 * Three-sensor line follower for Arduino Uno and an L298N.
 *
 * Wiring: A1 -> IN4, A2 -> IN3, A3 -> IN2, A4 -> IN1.
 * Tie ENA and ENB to the logic 5 V rail and share ground with the Uno.
 * The sketch commands full forward, full reverse or braking on each side.
 *
 * SELECT starts or stops a run. RIGHT switches the LCD between sensor and
 * motor states and elapsed time.
 */
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdio.h>
#include <string.h>

// LCD keypad shield pins are RS, E, D4, D5, D6 and D7, in that order.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// The three IR signals are digital. The keypad is an analogue resistor ladder.
const byte LEFT_IR   = 2;
const byte CENTER_IR = A5;
const byte RIGHT_IR  = 12;
const byte KEY_PIN   = A0;

// Channel A drives OUT1/OUT2. Channel B drives OUT3/OUT4.
// Each pair is ordered so the first input was high in the tested forward setup.
const byte CHANNEL_A_INPUT1 = A3;
const byte CHANNEL_A_INPUT2 = A4;
const byte CHANNEL_B_INPUT1 = A1;
const byte CHANNEL_B_INPUT2 = A2;

// This build assigns OUT3/OUT4 to the left wheels. Confirm with wheels lifted.
const bool LEFT_IS_CHANNEL_B = true;

// Convert each probe's raw level separately. The centre probe has opposite polarity.
const byte LEFT_BLACK_LEVEL   = LOW;
const byte CENTER_BLACK_LEVEL = HIGH;
const byte RIGHT_BLACK_LEVEL  = LOW;

// Flip the relevant setting if a wheel group drives backwards when commanded forward.
const bool LEFT_FORWARD_HIGH  = true;
const bool RIGHT_FORWARD_HIGH = true;

// Motor states are direction commands, with zero applying dynamic braking.
const int FORWARD = 1;
const int BRAKE = 0;
const int REVERSE = -1;

// Poll sensors and update the motion separately from the slower LCD refresh.
const unsigned long CONTROL_MS = 5;
const unsigned long LCD_MS = 100;
const unsigned long BUTTON_DEBOUNCE_MS = 35;

// Use a recent side detection to select a search direction when all probes go white.
const unsigned long DIRECTION_MEMORY_MS = 350;
// End a run if the line has not been reacquired after 10 seconds.
const unsigned long SEARCH_TIMEOUT_MS = 10000;
// Brake a motor for at least 40 ms before driving it in the opposite direction.
const unsigned long REVERSE_PAUSE_MS = 40;
// Only accept all-black readings after the run has been armed for finish detection.
const unsigned long FINISH_CONFIRM_MS = 15;
const unsigned long FINISH_ARM_MS = 13000UL;

// With EN held high, equal input levels brake the motor. Setting both
// inputs low also provides the resting state used at startup.
struct Motor {
  byte firstInput;
  byte secondInput;
  bool forwardHigh;
  int lastDirection;
  int output;
  unsigned long stoppedAt;

  void begin() {
    digitalWrite(firstInput, LOW);
    digitalWrite(secondInput, LOW);
    pinMode(firstInput, OUTPUT);
    pinMode(secondInput, OUTPUT);
    stoppedAt = millis();
  }

  void brake(unsigned long now) {
    if (output == BRAKE) return;
    digitalWrite(firstInput, LOW);
    digitalWrite(secondInput, LOW);
    output = BRAKE;
    stoppedAt = now;
  }

  void drive(int direction, unsigned long now) {
    if (direction == BRAKE) {
      brake(now);
      return;
    }
    if (direction == output) return;

    // Keep the previous driven direction through a stop so quick reversals
    // still respect the braking interval.
    if (lastDirection != BRAKE && direction != lastDirection) {
      brake(now);
      if (now - stoppedAt < REVERSE_PAUSE_MS) return;
    }

    bool firstHigh = direction == FORWARD ? forwardHigh : !forwardHigh;
    digitalWrite(firstInput, firstHigh ? HIGH : LOW);
    digitalWrite(secondInput, firstHigh ? LOW : HIGH);
    lastDirection = direction;
    output = direction;
  }
};

Motor leftMotor = {
  LEFT_IS_CHANNEL_B ? CHANNEL_B_INPUT1 : CHANNEL_A_INPUT1,
  LEFT_IS_CHANNEL_B ? CHANNEL_B_INPUT2 : CHANNEL_A_INPUT2,
  LEFT_FORWARD_HIGH, BRAKE, BRAKE, 0
};
Motor rightMotor = {
  LEFT_IS_CHANNEL_B ? CHANNEL_A_INPUT1 : CHANNEL_B_INPUT1,
  LEFT_IS_CHANNEL_B ? CHANNEL_A_INPUT2 : CHANNEL_B_INPUT2,
  RIGHT_FORWARD_HIGH, BRAKE, BRAKE, 0
};

bool running = false;
bool finished = false;
bool lost = false;

bool leftBlack = false;
bool centerBlack = false;
bool rightBlack = false;

// Record where the line was last seen: -1 left, 1 right, 0 unknown.
int lastSide = 0;
unsigned long lastSideAt = 0;

// Latch the search direction and timeout once per line-loss episode.
bool searching = false;
int searchSide = 0;
unsigned long searchStartedAt = 0;

// A candidate lasts only while all three probes keep reading black.
bool finishCandidate = false;
unsigned long finishSince = 0;

const byte BUTTON_NONE   = 0;
const byte BUTTON_SELECT = 1;
const byte BUTTON_RIGHT  = 2;
const byte BUTTON_OTHER  = 3;

byte rawButton = BUTTON_NONE;
byte stableButton = BUTTON_NONE;
bool buttonsArmed = false;
unsigned long buttonChangedAt = 0;

bool statsPage = false;
unsigned long lastControlAt = 0;
unsigned long lastLcdAt = 0;

unsigned long elapsedMs = 0;
unsigned long lastMeasureAt = 0;

void readSensors() {
  leftBlack   = digitalRead(LEFT_IR)   == LEFT_BLACK_LEVEL;
  centerBlack = digitalRead(CENTER_IR) == CENTER_BLACK_LEVEL;
  rightBlack  = digitalRead(RIGHT_IR)  == RIGHT_BLACK_LEVEL;
}

// Account for the previous loop interval before handling a start or stop event.
void measureRun(unsigned long now) {
  unsigned long dtMs = now - lastMeasureAt;
  lastMeasureAt = now;
  if (!running) return;

  elapsedMs += dtMs;
}

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
}

void stopRun(unsigned long now) {
  running = false;
  searching = false;

  leftMotor.drive(BRAKE, now);
  rightMotor.drive(BRAKE, now);
}

// Thresholds for the fitted LCD keypad shield. Ignore unused buttons.
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

  // Require a stable release at startup so a held SELECT cannot start the car.
  if (!buttonsArmed) {
    if (button == BUTTON_NONE) buttonsArmed = true;
    return;
  }

  // Act only on a new stable press so holding a key does not keep toggling it.
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

// Sample even when stopped so the live LCD can be used to check the probes.
void followLine(unsigned long now) {

  readSensors();
  if (!running) return;

  // 111: drive through all-black patches during the initial 13 seconds.
  if (leftBlack && centerBlack && rightBlack) {
    if (elapsedMs < FINISH_ARM_MS) {
      finishCandidate = false;
      searching = false;

      leftMotor.drive(FORWARD, now);
      rightMotor.drive(FORWARD, now);
      return;
    }

    // Start a fresh confirmation window after arming. Keep the previous
    // motor commands until the reading is confirmed or changes.
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

  int leftTarget = BRAKE;
  int rightTarget = BRAKE;

  // 010: centred on the line, drive both sides forward.
  if (!leftBlack && centerBlack && !rightBlack) {

    searching = false;
    leftTarget = FORWARD;
    rightTarget = FORWARD;

  // 100/110 steer left and 001/011 steer right. Brake the inner side
  // and drive the outer side forward.
  } else if (leftBlack != rightBlack) {

    searching = false;
    lastSide = leftBlack ? -1 : 1;
    lastSideAt = now;

    if (leftBlack) {
      leftTarget = BRAKE;
      rightTarget = FORWARD;
    } else {
      leftTarget = FORWARD;
      rightTarget = BRAKE;
    }

  } else {

    // 000 and 101 enter the same timeout. Only 000 can initiate a pivot,
    // and only when a recent direction was available on entry.
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

    // Pivot toward the remembered side. Leave both sides braked for 101
    // or when there is no recent side detection.
    if (allWhite && searchSide < 0) {
      leftTarget  = REVERSE;
      rightTarget = FORWARD;
    } else if (allWhite && searchSide > 0) {
      leftTarget  = FORWARD;
      rightTarget = REVERSE;
    }
  }

  leftMotor.drive(leftTarget, now);
  rightMotor.drive(rightTarget, now);
}

void printLcdRow(byte row, const char *message) {
  lcd.setCursor(0, row);
  lcd.print(message);

  // Pad the line to erase any characters left by a longer previous message.
  for (size_t col = strlen(message); col < 16; col++) {
    lcd.print(' ');
  }
}

const char *motorLabel(int direction) {
  if (direction == FORWARD) return "FWD";
  if (direction == REVERSE) return "REV";
  return "STOP";
}

void updateLcd() {
  char row[17];
  const char *status = "STOP";

  if (finished) {
    status = "FIN";
  } else if (lost) {
    status = "LOST";
  } else if (running) {
    status = searching ? "FIND" : "RUN";
  }

  // The summary shows elapsed time. The live page shows sensor readings
  // left to right, motor states and the countdown until finish detection is armed.
  if (statsPage) {
    snprintf(row, sizeof(row), "T:%lu.%lus %s",
             elapsedMs / 1000UL, (elapsedMs % 1000UL) / 100UL, status);
    printLcdRow(0, row);

    printLcdRow(1, "RIGHT: LIVE");
  } else {
    if (running && elapsedMs < FINISH_ARM_MS) {
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

    snprintf(row, sizeof(row), "L:%s R:%s", motorLabel(leftMotor.output), motorLabel(rightMotor.output));
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
