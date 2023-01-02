Random number generator
=======================

TBD.

## Circuit

<img src="https://github.com/muwerk/examples/blob/master/Resources/RandomNumbers.jpg" width="50%">

## Details

### Noise generator

The noise generator uses an open collector reverse NPN transistor at 9V to generate a noise signal.

<img src="https://github.com/muwerk/examples/blob/master/Resources/circuit_noise_generator.png" width="50%">

The noise signal generated has a low amplitude of +- 100mV and homogeneous fourier spectrum.

<img src="https://github.com/muwerk/examples/blob/master/Resources/Osc_RandomNoise_(N).png" width="50%">

_Output of the noise generator, circuit mark (N)_

### Comparator LM393

The signal (N) is now sent into a somewhat unusual comparator circuit:

The output signal (C) has a higher amplitude, suitable to feed into a Schmitt trigger.

<img src="https://github.com/muwerk/examples/blob/master/Resources/Osc_CompOut_(C).png" width="50%">

_Output of the comparator, circuit mark (C)_

### Schmitt trigger

The Schmitt trigger is a simple circuit that converts a signal (C) into a square wave. Two cascaded inverters are used to generate the square wave with 'positive' blob.

<img src="https://github.com/muwerk/examples/blob/master/Resources/Osc_SchmittTrigger_(D).png" width="50%">

_Output of the Schmitt trigger, circuit mark (D)_

### D flip flop

Since the signal (D) is irregular ('high' states are less probable than 'low' states), the D flip flop is used to generate a regular square wave (D++).

<img src="https://github.com/muwerk/examples/blob/master/Resources/Osc_DFlipFlop_(D++).png" width="50%">

_Output of the D flip flop, circuit mark (D++)_

## Next steps

Statistical tests have to show, if signal (D) or (D++) is more useful for random number generation.
