# My Progress Record

I put this summary together on 18 November 2026 using my notes and our group timeline.

## Week 1, 27–30 October

I helped assemble and rework the 4WD chassis. We tested the bench H-bridge at 1.5 V and 3 V. My earlier notes list no-load currents of 0.16 A and 0.20 A and loaded currents of around 0.25–0.26 A and 0.40 A. We restored motor drive by returning the direction inputs to A1–A4 and fitted the optical gates.

I worked on the countdown and adjusted the left and right PWM without encoder feedback. At 110/160 I saw the LCD display the countdown and the car stop at 10 s with a slight left drift. My teammate recorded straight travel at 110/150. I still need the demonstrator confirmation and the remaining motor measurements.

## Week 2, sensor build

We tested the homemade infrared sensor on a breadboard and transferred the circuit to Veroboard. I have the board layout and photographs of both sides. I still need to confirm the lab date and record standalone continuity, polarity and black/white measurements.

## Single-sensor trial

I watched the LCD classification and PWM change as the sensor moved between black and white. At 180/0 PWM we confirmed that each side could run independently. The car kept overshooting sharp bends even after increasing the steering setting to 120, so we moved on to three sensors. I did not measure whether inertia caused the overshoot.

## Three-sensor trials

We mounted the three sensors 22 mm apart. The car initially lost the line on a bend. We added a reverse search and recorded full laps of 32 s, 27 s and 19.1 s. The 27 s trial followed a change to a 25 s finish-detection delay. The current sketch uses 18 s. I still need to confirm the settings used for the 19.1 s trial and compare the LCD distance estimate with a measured track length.
