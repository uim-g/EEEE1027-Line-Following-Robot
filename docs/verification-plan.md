# Verification Plan

I use these checks to document the build and repeat tests where I still need evidence.

| Check | How I check | Evidence to record |
| --- | --- | --- |
| Motor direction and side mapping | Raise the wheels and command each motor separately in both directions | Pin map, command and observed direction |
| PWM response | Compare both wheels across recorded PWM settings under consistent conditions | Supply, settings and measured speed where available |
| Optical speed sensing | Observe pulses while rotating each wheel | Input pin, pulse count, interval and wheel geometry |
| Infrared discrimination | Measure each sensor on black and white surfaces | Raw output, polarity, threshold, mounting height and lighting |
| Soldered board integrity | Inspect joints and check continuity and polarity before powering | Circuit revision, inspection findings and corrections |
| Straight-line following | Run repeated trials on a defined straight section | Firmware revision, PWM settings, track conditions and deviations |
| Bend handling | Test gradual, sharp and right-angle bends separately | Trial count, completion observations and line-loss locations |
| Line recovery | Test defined loss-of-line cases | Initial condition, recovery behaviour and timeout response |

## My trial records

For each run I record the date, operator, firmware and wiring revisions, battery condition, sensor layout, track, settings and result. I set the success criteria before the trial and keep failed runs alongside successful ones. I label visual estimates separately from measurements.

In the single-sensor trial I saw the display change and confirmed that each side could drive independently. We stopped testing that setup after repeated overshoot at sharp bends. I still need measured wheel-speed data and repeatable recovery results for the three-sensor build.
