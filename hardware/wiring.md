# Hardware and Wiring Record

## Recorded configurations

The connections below belong to particular activities. They must be checked against the current physical build before reuse.

| Activity | Signal | Recorded connection | Evidence status |
| --- | --- | --- | --- |
| Week 1 motor drive | ENA | Arduino D11 | Reported connection; current build not checked |
| Week 1 motor drive | ENB | Arduino D3 | Reported connection; current build not checked |
| Week 1 optical speed sensing | Optical inputs | Arduino D2 and D12 | Reported connections; measurement results not established |
| Single-sensor trial | IN1, IN2, IN3, IN4 | Arduino A1, A2, A3, A4 respectively | Recorded for the discontinued configuration |
| Single-sensor trial | ENA, ENB | Arduino D11, D3 respectively | Recorded for the discontinued configuration |

The single-sensor trial did not use wheel-speed feedback. Its infrared input pin is not established in the available record.

## Construction record

- Controller: Arduino UNO.
- Display: LCD; current model and wiring need confirmation.
- Drive: H-bridge and independently controlled left/right motors; driver model and supply ratings need confirmation.
- Infrared sensor: homemade; Week 2 Day 1 used breadboard construction followed by soldering to a circuit board.

## Checks before the next build revision

1. Record the driver model, motor supply voltage, controller supply arrangement and ground connections.
2. Produce a schematic and pin allocation matching the physical car.
3. Confirm whether D2 and D12 serve optical speed sensors or infrared sensors in the current design. Do not assign the same inputs to both without a reviewed interface.
4. Record infrared output polarity on black and white surfaces, mounting height and threshold settings.
5. Verify left/right motor identity and forward/reverse directions with the wheels raised before track testing.
