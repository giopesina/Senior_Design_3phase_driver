
# 3-Phase Power Inverter with PIC16F1517 - Technical Breakdown

## Introduction
This document details the design and implementation of a 3-phase power inverter using a PIC16F1517 microcontroller. The inverter converts DC to AC by controlling the switching of six IGBTs in a CMOS configuration using Pulse Width Modulation (PWM). The system is designed to handle loads up to 6kW, and the output frequency varies between 60Hz to 20kHz, based on user input from a potentiometer.

## Math for PWM Generation

### 1. AC Signal Approximation Using PWM
The PIC16F1517 can't generate analog AC signals directly, so Pulse Width Modulation (PWM) is used to approximate the AC sine wave. By controlling the duty cycle of a high-speed switching signal, we can mimic a sinusoidal waveform. The number of PWM points used per cycle determines the smoothness of the approximation.

### 2. Number of PWM Pulse Points
For this project, we use 60 points for each half-cycle of the sine wave, giving us 120 points for a full sine wave. This allows for a smoother waveform approximation compared to the previously suggested 20 points. The greater the number of points, the closer the waveform resembles a pure sine wave.

### 3. Sine Wave Calculation
Each point in the PWM lookup table corresponds to a specific angle along the sine wave. The sine function is used to generate values between -1 and 1, and these are scaled to 8-bit PWM values (0 to 255). The formula for calculating the sine value at a specific point is:

```
sine_value[i] = sin(i * π / NUM_POINTS)
```

where i is the index (from 0 to NUM_POINTS) and π is the mathematical constant Pi.

### 4. Duty Cycle and Frequency Calculation
The duty cycle for each PWM pulse is derived from the sine wave. For example, if the sine value at a given point is 0.5, the duty cycle would be:

```
0.5 * 255 = 127
```

This means the signal is on for 50% of the time during that pulse.

### 5. Mapping ADC Input to Frequency
The output frequency is controlled by the user via a potentiometer connected to the ADC. The ADC produces values between 0 and 1023, and this value is mapped to a frequency range from 60Hz to 20kHz using the formula:

```
frequency = (adc_value * (20000 - 60)) / 1023 + 60
```

### 6. PWM Frequency Setting
The Timer2 period for generating the PWM signal is calculated using the formula:

```
PR2 = (F_osc / (4 * PWM_frequency)) - 1
```

where F_osc is the oscillator frequency (8MHz in this case). Adjusting PR2 changes the PWM frequency.

## Goals for Expanding to 3-Phase

1. **Implementing 3 Phase Shifted Signals**: Generate two additional signals that are phase-shifted by 120° and 240° from the original phase.
2. **Adding Complementary Signals for IGBTs**: Each IGBT requires complementary signals for switching between the positive and negative half-cycles.
3. **Timing Adjustments**: Ensure that the phase delays are precise, especially at higher frequencies, to avoid damaging the IGBTs.
4. **PWM Frequency Synchronization**: All three phases must have synchronized PWM generation, scaled according to the user's frequency input.
