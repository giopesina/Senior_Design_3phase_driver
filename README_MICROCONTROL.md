# 3-Phase Controller Project

## Introduction

This project is designed to develop an embedded system that controls 3-phase circuitry in real time using a PIC microcontroller. The project is implemented in C, is highly modifiable, and uses function calls to separate different parts of the code. The code has been optimized for an 8 MHz clock (with future plans to scale up to 20 MHz) and uses UART communication with interrupts to handle state and frequency updates.

## Project Overview

The main purpose of the code is to control the pulsing of a 3-phase system. The system uses digital outputs to indicate different states (OFF, STANDBY, RUN) and to generate the required phase signals. The UART communication is used to receive commands (which update system state and timing) and to transmit status information.

## Code Functionality

### 1. Configuration and Global Setup
- **Configuration Bits:**  
  The configuration bits are set to use a High-Speed Oscillator (HS) with watchdog timer, power-up timer, and low-voltage programming disabled.  
- **_XTAL_FREQ Definition:**  
  The oscillator frequency is defined as 8 MHz.
- **Global Variables:**  
  Global variables such as `stateValue`, `indexValue`, `timeSet`, `state`, and `fidelity` are declared. These are used to maintain the system’s state and control the phase pulsing.

### 2. Initialization Functions
- **controller_init():**  
  Sets the internal oscillator (8 MHz) and configures PORTD as an output with an initial low state.
- **global_init():**  
  Initializes global variables for system state and timing.
- **UART_Init(long baud_rate):**  
  Configures the UART with a specified baud rate (9600 in this case). It sets TX/RX pins, selects the high baud rate mode, calculates the baud rate generator value based on the oscillator frequency, and enables both the UART module and the required interrupts.

### 3. Main Loop
- **main():**  
  After calling the initialization functions, the `main()` function enters an infinite loop where it continuously calls `statusCheck()` to monitor and update the system state.

### 4. State Handling and Output Control
- **statusCheck():**  
  Checks the current state of the system (as indicated by `stateValue`) and sets the digital outputs accordingly:
  - **OFF State (`'0'`):**  
    Sets RD3 high while RD4 and RD5 are low.
  - **STANDBY State (`'1'`):**  
    Sets RD4 high (with RD3 and RD5 low), calls `pulse()` to generate a pulse sequence, and transmits status via UART.
  - **RUN State (`'2'`):**  
    Sets RD5 high (with RD3 and RD4 low), transmits status via UART, and calls `phaseImp()` to perform the phase pulsing sequence.
  - If none of these states match, a default output is applied.

- **pulse():**  
  Toggles RD6 to generate a pulse sequence by turning it on and off with half-second delays.

- **phaseImp():**  
  Generates a 3-phase pulsing pattern by toggling RD0, RD1, and RD2 in a specific sequence. This sequence is repeated a number of times determined by the `fidelity` variable, with each step delayed by the value stored in `timeSet`.

### 5. UART Communication
- **UART_ISR():**  
  The UART Interrupt Service Routine handles incoming data. When a receive interrupt occurs, it:
  - Checks and clears any overrun error.
  - Reads the first byte into `stateValue`.
  - If a second byte is available, it reads it into `indexValue` and updates `timeSet`.
- **UART_Read():**  
  Waits for data to be received (with a timeout) and returns the received character.
- **UART_Transmit(char data):**  
  Waits until the transmit register is ready and then sends the given character.

## Summary

This embedded system code controls a 3-phase pulsing circuit by:
- Initializing and configuring the microcontroller (oscillator, UART, I/O pins).
- Continuously checking the system state and updating digital outputs to reflect OFF, STANDBY, or RUN states.
- Generating pulse sequences and phase-specific toggling on various output pins.
- Using UART interrupts to receive updates and communicate the system status.

This project is structured for clear separation of concerns, making it easier to modify and scale as future requirements evolve.
