# Progress Record

Compiled: 18 November 2026. This is the documentation date, not the date of any laboratory experiment. Laboratory dates and individual responsibilities still need reconciliation with the original records.

## Week 1 activities

**Objectives:** Work with H-bridge forward/reverse motor control, assemble the chassis, operate the Arduino UNO and LCD, and investigate optical speed sensing and left/right PWM adjustment.

**Procedure:** These topics are recorded as Week 1 activities. The detailed sequence, settings and responsibilities have not yet been established.

**Results:** ENA D11, ENB D3 and optical inputs D2/D12 are reported. No complete set of measurements or pass results is available in this record.

**Conclusion and reflection:** Reconcile the wiring, measurements and observations before judging completion. PWM commands alone do not establish actual wheel speeds.

## Week 2 Day 1 activities

**Objectives:** Construct a homemade infrared sensor.

**Procedure:** Build the circuit on a breadboard before soldering it to a circuit board.

**Results:** The construction sequence is reported. Sensor performance validation remains unconfirmed.

**Conclusion and reflection:** Add the actual circuit, component values, inspection findings and black/white measurements before assessing performance.

## Single-sensor control trial

**Date:** Not established. The supporting summary was compiled on 17 November 2026.

**Objectives:** Attempt line following with one homemade infrared sensor and differential motor commands, without wheel-speed feedback.

**Procedure:** Observe LCD sensor classification and PWM indications. Compare ordinary differential drive, then command one side at PWM 180 and the other at 0. Increase STEER to 120 during bend trials.

**Results:** The LCD black/white indication and PWM commands changed with sensor input. A wheel-speed difference was not clearly visible in ordinary differential testing. In the 180/0 check, the left side ran and the right side stopped on black; the right side ran and the left side stopped on white. Sharp bends, especially right-angle bends, still caused the car to overshoot the line. Increasing STEER to 120 did not resolve this.

**Conclusion:** Independent left/right actuation was observed, but stable right-angle tracking was not demonstrated. This single-sensor control approach was discontinued.

**Reflection:** Inertia was suggested as a possible cause but was not measured. Stronger steering, inner-wheel braking or reversal, and reverse searching on white were discussed; they must not be recorded as successful tests without evidence. Confirm the current sensor layout before further control development.
