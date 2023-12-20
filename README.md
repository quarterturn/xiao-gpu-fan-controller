# xiao-gpu-fan-controller
A temperature-controlled PWM meant for passive datacenter GPUs
example: P40, P100, V100

```
hardware:
Microcontroller: Seeed XIAO
Recommended fan: Apple iMac 27" A1312 2009, 2010, 2011 Optical Drive Fan - BFB0812HD 610-0035
Fan wires (sticker top to bottom):
PWM
GND
N/C (tach)
12V

(per temperature sensor)
1x 10K NTC (1%)
1x 10K 1% (per PWM channel)
1x small-signal NPN transistor
1x 1K resistor (any tolerance OK)
1x 2.2K resistor

schematics
temperature sensor:

GND----[NTC]----[analog input pin]----[10k]----3.3V

glue the thermistor to the mounting plate end of the heatsink
just below the heatsink fins

PWM output channel:

                12V----[1k]----.----[PWM input to fan]
                              /
[PWM output pin]----[2.2k]----[base]
                              \
                               [emitter]----GND


The code is set up for a single NTC and PWM, it's straightforward enough you'll figure out how to modify it to add more sensors and output.
The code outputs sensor and PWM values to USB serial. Compare to nvidia-smi or nvtop and see if it works for your sensor placement. If not, adjust the temperature range in the code.

```
