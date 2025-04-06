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

#define _XTAL_FREQ 8000000

// Global Variables
int fidelity;
volatile char stateValue;
volatile unsigned char timeSet;
volatile char uartMode = 0;

// Function Prototypes
void controller_init(void);
void global_init(void);
void UART_Init(long baud_rate);
void statusCheck(void);
void phaseImp(void);
void delay(char time_delay);
void __interrupt() UART_ISR(void);
void UART_Transmit(char data);
unsigned char mapCharToTimeSet(char c);
void pulse(void);

int main(void) {
    controller_init();
    global_init();
    UART_Init(9600);

    while (1) {
        statusCheck();
    }
}

void statusCheck(void) {
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

void pulse(void) {
    RD6 = 1;
    __delay_ms(1000);
    RD6 = 0;
    __delay_ms(1000);
}

void controller_init(void) {
    OSCCON = 0b01110000;
    TRISD = 0x00;
    PORTD = 0x00;

    TRISA = 0x11;
    PORTA = 0x00;
}

void UART_Init(long baud_rate) {
    TRISC6 = 0;
    TRISC7 = 1;

    BAUDCTL = 0;
    BRGH = 1;
    SPBRG = (_XTAL_FREQ / (16 * baud_rate)) - 1;

    TXSTA = 0b00100100;
    RCSTA = 0b10010000;

    PIE1bits.RCIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void global_init(void) {
    stateValue = '0';
    timeSet = 0;
    //state = 0;
    fidelity = 3;
}

void delay(char time_delay) {
    for (char i = 0x00; i < time_delay; i++) {
        __delay_us(50);
    }
}

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

void UART_Transmit(char data) {
    while (!PIR1bits.TXIF);
    TXREG = data;
}

void __interrupt() UART_ISR(void) {
    if (PIR1bits.RCIF) {
        if (RCSTAbits.OERR) {
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }

        char receivedData = RCREG;

        if (uartMode == 0) {
            uartMode = receivedData;  // 'T' or 'S'
        } else {
            switch (uartMode) {
                case 'T':
                    timeSet = mapCharToTimeSet(receivedData);
                    UART_Transmit('A');
                    break;
                case 'S':
                    stateValue = receivedData;
                    UART_Transmit('A');
                    break;
                default:
                    UART_Transmit('E');
                    break;
            }
            uartMode = 0;  // Reset
        }
    }
}

// Map input characters to delays
unsigned char mapCharToTimeSet(char c) {
    int index = -1;

    if (c >= '0' && c <= '9') {
        index = c - '0';
    } else if (c >= 'a' && c <= 'z') {
        index = 10 + (c - 'a');
    } else if (c >= 'B' && c <= 'Z') {
        index = 36 + (c - 'B');
    }

    if (index >= 0 && index <= 60) {
        return 200 - (index * 3);  // Scaled delay
    }

    return 75;  // Fallback
}
