# EEEE1027 Project Logbook

Progress through 18 November 2026. The Week 1 dates come from the group log. The dates of the Week 2 build and track sessions still need confirmation.

## Project Week 1

### 27 October 2026 — H-bridge and chassis assembly

**Objective.** Check H-bridge motor reversal and assemble the four-wheel chassis.

**Procedure.** Test the bench H-bridge at 1.5 V and 3 V, measure the motor current with and without load, assemble the chassis and try the motor connections in Tinkercad.

| Supply | No-load current | Current under load |
| --- | ---: | ---: |
| 1.5 V | 0.16 A | 0.25–0.26 A |
| 3 V | 0.20 A | 0.40 A |

**Result and reflection.** The values above are current magnitudes from the bench notes. The four-motor simulation did not run as wired. The simulation ran with one motor per driver channel. The record still needs direction-specific current readings, locked-rotor results and the demonstrator check.

### 28 October 2026 — Motor wiring fault

**Objective.** Rework the chassis wiring and check whether the direction controls could be moved to digital pins.

**Procedure.** Move the direction leads from the analogue pins to digital pins. Check the L298N supply terminals against the markings on the physical board.

**Result and reflection.** The motors started out of sync and did not stop on time. The teammate's notes record a corrected supply connection, after which one side still failed to run. The next test isolated the direction wiring by keeping the remaining connections in place.

### 29 October 2026 — Drive recovery and encoder installation

**Objective.** Restore both motor groups and prepare independent PWM control.

**Procedure.** Return the direction inputs to A1–A4. Fit the optical discs and gates. Use D11 and D3 for ENA and ENB.

**Result and reflection.** Motor drive returned after the direction wiring change. This test did not isolate the cause of the earlier fault. The optical gates were installed, but pulse counts, wheel speeds and encoder distance were not recorded.

### 30 October 2026 — Fixed PWM and timed drive

**Objective.** Check straight travel, LCD countdown and automatic stopping.

**Procedure.** Adjust the left and right PWM without wheel-speed feedback. Observe drift during each run. The teammate's test record contains these settings.

| Left PWM | Right PWM | Teammate's observation |
| ---: | ---: | --- |
| 110 | 110 | Turned right |
| 110 | 140 | Drifted slightly right |
| 110 | 160 | Drifted slightly left |
| 110 | 150 | Travelled straight |

**Result and reflection.** In a separately observed run at 110/160, the LCD showed the countdown and both PWM settings. The car stopped at 10 seconds and drifted slightly left. The two records identify the settings used in each observation. The demonstrator's acceptance of the straight-line challenge still needs confirmation.

## Project Week 2

### Day 1 — Infrared prototype and Veroboard transfer

**Date.** The layout file is dated 16 November 2026. The session date and times remain unconfirmed.

**Objective and procedure.** Build the LM358 infrared circuit on a breadboard, check its response, then transfer it to Veroboard. The layout identifies the analogue output, digital output, 5 V and ground.

**Result and reflection.** The breadboard prototype worked in an initial check. Both faces of the completed Veroboard were photographed. Separate continuity, polarity and black/white measurements on the finished board still need recording.

### Single-sensor trial

**Date.** Trial date unconfirmed. Notes compiled on 17 November 2026.

**Objective and procedure.** Try differential steering with one homemade infrared sensor and no speed feedback. Watch the LCD classification and PWM, then drive one side at PWM 180 and the other at zero to check each side independently.

**Result and reflection.** The LCD classification switched between black and white. On black the left side ran and the right side stopped. The opposite happened on white. The car continued to overshoot sharp turns after the steering setting was increased to 120, so the group moved on to three sensors. Inertia was discussed as a possible cause and was not measured.

### Day 2 — Three-sensor installation and track trials

**Date.** The session log records 13:00–17:00. The calendar date needs confirmation.

**Objective.** Check straight travel, fit three sensors and test corner recovery, finish detection and the distance display.

**Procedure and results.** The straight-line PWM pairs were 100/80 at low speed and 255/240 at high speed. Adjacent photodiodes were mounted 22 mm apart. A broken power lead was resoldered and the connection was changed to a male-to-male jumper.

| Session time | Change or test | Observed result |
| --- | --- | --- |
| 16:00 | First track run | The car left the line on a bend, read 000 and stopped. |
| 16:15 | Reverse search with a 10 s FIND timeout | A full lap was logged at 32 s. |
| 16:30 | Arm finish detection after 25 s | A full lap was logged at 27 s. The delay addressed an early 111 reading on a bend. |
| 16:40–17:00 | Add distance estimation from the left and right PWM commands | The distance display was added. No measured track-length comparison was recorded. |
| Final trial | Exact settings still to be confirmed | A full lap was logged at 19.1 s. |

**Reflection.** The reverse search helped the car recover after losing the line. The [current sketch](../src/Line_Following_1/Line_Following_1.ino) arms finish detection at 18 s. The 25 s setting belongs to the 16:30 trial. The firmware settings for the 19.1 s run still need checking. The distance display estimates travel from motor commands. A measured track length is needed to check its error. Formal Challenge 1 acceptance remains open.

### Day 3 — Logbook update, 18 November 2026

**Objective and outcome.** Organise the Week 2 build and track records. The breadboard, Veroboard and track evidence was indexed alongside the trial times and firmware changes. The track results above belong to the earlier session.

## Project Week 3

The next planned work is to confirm the advanced-feature brief, prepare for Track Challenge 2 and Viva 1, and record the resulting tests. The [Project Roadmap](https://github.com/users/uim-g/projects/3/views/4) tracks the course dates.
