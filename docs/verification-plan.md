# Verification Plan

Use these checks to record results and repeat tests after wiring or firmware changes.

| Check | Method | Evidence to record |
| --- | --- | --- |
| Motor direction and side mapping | Raise the wheels and command each motor separately in both directions | Pin map, command and observed direction |
| PWM response | Compare both wheels across recorded PWM settings under consistent conditions | Supply, settings and measured speed where available |
| Optical speed sensing | Observe pulses while rotating each wheel | Input pin, pulse count, interval and wheel geometry |
| Infrared discrimination | Measure each sensor on black and white surfaces | Raw output, polarity, threshold, mounting height and lighting |
| Soldered board integrity | Inspect joints and check continuity and polarity before powering | Circuit revision, inspection findings and corrections |
| Straight-line following | Run repeated trials on a defined straight section | Firmware revision, PWM settings, track conditions and deviations |
| Bend handling | Test gradual, sharp and right-angle bends separately | Trial count, completion observations and line-loss locations |
| Line recovery | Test defined loss-of-line cases | Initial condition, recovery behaviour and timeout response |

## Recording a trial

Record the actual date, operator, firmware revision, wiring revision, battery condition, sensor configuration, track layout, settings and observed outcome. Agree success criteria before interpreting results. Retain unsuccessful trials and distinguish measurements from visual estimates.

The single-sensor trial confirmed independent drive and a changing LCD indication. The car still overshot sharp bends. Wheel-speed measurements and repeatable right-angle recovery checks remain open.
