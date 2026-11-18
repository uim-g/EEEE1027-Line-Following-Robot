# EEEE1027 Line-Following Robot

University of Nottingham Malaysia

EEEE1027 — Applied Electrical and Electronic Engineering Construction Project

## Project aim

A four-wheel robot for following a black line, built around an Arduino UNO, a motor driver and infrared sensors.

## Current status

Week 1 covered the chassis, H-bridge and LCD, followed by fixed-PWM drive tests. One run stopped after 10 seconds with a slight left drift. The group recorded straight travel at a different PWM setting.

In Week 2 the group tested a breadboard infrared prototype and transferred it to Veroboard. Separate measurements on the soldered board are still pending.

The single-sensor setup was dropped after repeated overshoot at sharp bends. The group then fitted three sensors and logged full laps of 32 s, 27 s and 19.1 s. The settings for the 19.1 s run still need confirmation.

## Project records

- [Hardware and wiring](hardware/wiring.md)
- [Work breakdown and scheduling](docs/project-plan.md)
- [Verification plan](docs/verification-plan.md)
- [Progress record](logbook/progress-record.md)

## Firmware

[Line_Following_1.ino](src/Line_Following_1/Line_Following_1.ino) is the current three-sensor development sketch. Open it in Arduino IDE, select **Arduino Uno**, and install the **LiquidCrystal** library if needed.

The sketch reads left, centre and right sensors on D2, A5 and D12. These pins overlap the earlier optical speed inputs on D2 and D12. Check the wiring, sensor polarity and motor directions before a track run. SELECT starts or stops a run, and RIGHT switches between the sensor and statistics displays.

The current sketch arms finish detection after 18 seconds. It estimates distance from the applied PWM. The settings used for the 19.1-second run and the accuracy of the distance estimate still need confirmation.

## Evidence and assessment

Record actual laboratory dates, team responsibilities, settings, measurements and observations. Keep proposed changes separate from tested results. Each laboratory logbook entry must include its date, objectives, procedure, results, conclusions and reflection.

The schedule must eventually include tasks, durations and actual calendar dates. Check the Moodle assessment rubric to establish whether a Microsoft Project `.mpp` submission is required.

## Project tracking

- [WBS and work packages](https://github.com/users/uim-g/projects/3/views/1)
- [Kanban board](https://github.com/users/uim-g/projects/3/views/2)
- [Roadmap by Project Week](https://github.com/users/uim-g/projects/3/views/4)
- [Repository issues](https://github.com/uim-g/EEEE1027-Line-Following-Robot/issues)

Project Week identifies the course period. The Roadmap shows the start dates, finish dates and durations entered so far.
