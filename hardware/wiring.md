# Hardware and Wiring Record

I recorded these connections during the early builds. I will check them against the car before reusing them.

| Activity | Signal | Recorded connection | Evidence status |
| --- | --- | --- | --- |
| Week 1 motor drive | ENA | Arduino D11 | Week 1 record |
| Week 1 motor drive | ENB | Arduino D3 | Week 1 record |
| Week 1 optical speed sensing | Optical inputs | Arduino D2 and D12 | Week 1 record, speed data pending |
| Single-sensor trial | IN1, IN2, IN3, IN4 | Arduino A1, A2, A3, A4 respectively | Recorded for the discontinued configuration |
| Single-sensor trial | ENA, ENB | Arduino D11, D3 respectively | Recorded for the discontinued configuration |

We used an Arduino UNO, an LCD and an H-bridge with separate left and right drive. We built the IR sensor on a breadboard and soldered it onto Veroboard. I did not use wheel-speed feedback in the single-sensor trial. I still need to confirm which IR input that setup used.

In the three-sensor sketch I see D2, A5 and D12. I will compare these assignments with the car, especially D2 and D12, which we used for optical speed sensing earlier.

## My next wiring checks

1. I will confirm the driver, supplies, shared ground and LCD connections.
2. I will draw the schematic and pin map for the physical car.
3. I will measure each sensor's polarity on black and white and record its mounting height and threshold.
4. With the wheels raised, I will check which motor runs and in which direction for each command.
