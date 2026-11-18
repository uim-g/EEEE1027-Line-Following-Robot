# Progress Record

Notes compiled on 18 November 2026. Session dates and responsibilities are recorded where confirmed.

## Week 1 activities

**Objectives:** Work with H-bridge forward/reverse motor control, assemble the chassis, operate the Arduino UNO and LCD, and investigate optical speed sensing and left/right PWM adjustment.

**Procedure:** The 27–30 October group record covers the H-bridge test, chassis assembly, motor wiring checks and fixed-PWM trials.

**Results:** The Week 1 pin record lists ENA on D11, ENB on D3 and optical inputs on D2 and D12. Motor drive returned when the direction inputs were moved back to A1–A4. The timed run at left/right PWM 110/160 stopped after 10 seconds and drifted slightly left. The teammate recorded straight travel at 110/150.

**Conclusion and reflection:** Reconcile the wiring, measurements and observations before judging completion. PWM commands alone do not establish actual wheel speeds.

## Week 2 Day 1 activities

**Objectives:** Construct a homemade infrared sensor.

**Procedure:** Build the circuit on a breadboard before soldering it to a circuit board.

**Results:** The breadboard prototype worked in an initial check and was transferred to Veroboard. The standalone black/white measurements on the soldered board are pending.

**Conclusion and reflection:** Add the actual circuit, component values, inspection findings and black/white measurements before assessing performance.

## Single-sensor control trial

**Date:** Trial date unconfirmed. Notes compiled on 17 November 2026.

**Objectives:** Attempt line following with one homemade infrared sensor and differential motor commands, without wheel-speed feedback.

**Procedure:** Observe LCD sensor classification and PWM indications. Compare ordinary differential drive, then command one side at PWM 180 and the other at 0. Increase STEER to 120 during bend trials.

**Results:** The LCD black/white indication and PWM commands changed with sensor input. A wheel-speed difference was not clearly visible in ordinary differential testing. In the 180/0 check, the left side ran and the right side stopped on black; the right side ran and the left side stopped on white. Sharp bends, especially right-angle bends, still caused the car to overshoot the line. Increasing STEER to 120 did not resolve this.

**Conclusion:** The two sides could drive independently. The car continued to overshoot right-angle bends, so the single-sensor setup was dropped.

**Reflection:** Inertia was suggested as a possible cause but was not measured. Stronger steering, inner-wheel braking or reversal, and reverse searching on white were discussed; they must not be recorded as successful tests without evidence. Confirm the current sensor layout before further control development.
