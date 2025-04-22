#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

// CONFIGURATION BITS
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = OFF
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 8000000  // 8 MHz crystal

// Global Variables
int fidelity;
volatile char stateValue;
volatile unsigned char timeSet;
volatile char uartMode = 0;

// Function Prototypes
void controller_init(void);
void global_init(void);
void UART_Init(long baud_rate);
void UART_Transmit(unsigned char data);
void __interrupt() UART_ISR(void);
void pulse(void);
void phaseImp(void);
void delay(char t);

// Main Function
void main(void) {
    controller_init();
    global_init();
    UART_Init(9600);

    while (1) {
        if (stateValue == '0') {
            RD3 = 1; RD4 = 0; RD5 = 0;
        } else if (stateValue == '1') {
            RD3 = 0; RD4 = 1; RD5 = 0;
            pulse();
        } else if (stateValue == '2') {
            RD3 = 0; RD4 = 0; RD5 = 1;
            phaseImp();
        } else {
            RD3 = 1; RD4 = 0; RD5 = 1;
        }
    }
}

// Initialization
void controller_init(void) {
    OSCCON = 0b01110000;  // Internal 8 MHz (if using INTOSC)
    TRISD = 0x00;
    PORTD = 0x00;
    TRISA = 0x11;
    PORTA = 0x00;

    ADCON1 = 0x06;  // All analog inputs set to digital
    CM1CON0 = 0x00; // Disable comparators
    CM2CON0 = 0x00;
}

void global_init(void) {
    stateValue = '0';
    timeSet = 75;
    fidelity = 3;
}

// UART
void UART_Init(long baud_rate) {
    TRISC6 = 0;
    TRISC7 = 1;

    BRGH = 1;
    SPBRG = (unsigned char)((_XTAL_FREQ / (16 * baud_rate)) - 1);

    SYNC = 0;
    SPEN = 1;
    TXEN = 1;
    CREN = 1;

    RCIE = 1;
    PEIE = 1;
    GIE = 1;
}

void UART_Transmit(unsigned char data) {
    while (!TXIF);
    TXREG = data;
}

// Interrupt for UART receive
void __interrupt() UART_ISR(void) {
    if (RCIF) {
        if (OERR) {
            CREN = 0; CREN = 1;
        }

        unsigned char received = RCREG;

        if (uartMode == 0) {
            uartMode = received;  // Command: 'S' or 'T'
        } else {
            switch (uartMode) {
                case 'T':
                    timeSet = received;   // Now accepts raw unsigned char
                    UART_Transmit('A');
                    break;
                case 'S':
                    stateValue = received;
                    UART_Transmit('A');
                    break;
                default:
                    UART_Transmit('E');  // Unknown command
                    break;
            }
            uartMode = 0;
        }
    }
}
// In case the other delay function does not work as desired
/*
// Delay routine (microseconds scaled)
void delay(char t) {
    for (char i = 0; i < t; i++) {
        __delay_us(50);
    }
}
*/

// Delay function
void delay(void) {
    // approximate delay_us = 50 + timeSet * 65
    //   at timeSet=0 -> 50 µs; at 255 -> 50 + 255*65 = 16 575 µs (within ~0.5% of 16 616)
    unsigned int d = 50 + (unsigned int)timeSet * 65;

    // do whole milliseconds first
    unsigned int ms = d / 1000;
    while (ms--) {
        __delay_ms(1);
    }

    // then the leftover microseconds
    unsigned int us = d % 1000;
    while (us--) {
        __delay_us(1);
    }
}

// LED pulse
void pulse(void) {
    RD6 = 1; __delay_ms(500);
    RD6 = 0; __delay_ms(500);
}

// Step sequence
void phaseImp(void) {
    for (int i = 0; i < fidelity; i++) {
        RD0 = 0; delay(timeSet);
        RD1 = 1; delay(timeSet);
        RD2 = 0; delay(timeSet);
        RD0 = 1; delay(timeSet);
        RD1 = 0; delay(timeSet);
        RD2 = 1; delay(timeSet);
    }
}
