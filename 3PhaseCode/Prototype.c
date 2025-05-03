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
void delay(void);

// Main Function
void main(void) {
    controller_init();
    global_init();
    UART_Init(9600);

    while (1) {
        if (stateValue == '0') {         // OFF
        RC4 = 1;
        RD3 = 0;
        RD2 = 0;
        UART_Transmit('A');
        }

        else if (stateValue == '1') {    // STANDBY
            RC4 = 0;
            RD3 = 1;
            RD2 = 0;
            pulse();
            UART_Transmit('A');
        }

        else if (stateValue == '2') {    // RUN
            RC4 = 0;
            RD3 = 0;
            RD2 = 1;
            phaseImp();
            UART_Transmit('A');
        }

        else {
            RC4 = 1;
            RD3 = 0;
            RD2 = 1;
        }
    }
}

// Initialize controller
void controller_init(void) {
    OSCCON = 0b01110000;                        // Set Internal Oscillator to 8 MHz
    TRISD = 0x00;                               // Set PORTD as Output
    PORTD = 0x00;                               // Initialize PORTD to Low

    TRISCbits.TRISC0 = 0;                       //Phase 1
    TRISCbits.TRISC1 = 0;                       //Complement of P1
    TRISCbits.TRISC2 = 0;                       //Phase 2

    TRISCbits.TRISC3 = 0;                       //Complement of P2
                           //Phase 3
                          //Complement of P3

    TRISA = 0x11;  
    PORTA = 0x00;//Output/Input   (1 = Input, 0 = Output)
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

// Delay function
void delay(void) {
    //If timeSet is 0 -> fastest pulsing: 20KHz <=> 50us
    if (timeSet == 0){
        __delay_us(50);
    }
    else{
        for(unsigned char i = 0; i <= timeSet; i++){
            //If timeSet is 1 -> 50us * 2
            if(timeSet == 1){
                __delay_us(100); // <- 50us * 2 = 100us
            }
            //else delay for 50us * i : i < timeSet => 0 <= i <= 255 => 78Hz and 20KHz
            __delay_us(50);
        }
    }

}

// LED pulse
void pulse(void) {
    RD6 = 1; __delay_ms(500);
    RD6 = 0; __delay_ms(500);
}

// Phase Implementation
void phaseImp(void) {
    for (int i = fidelity; i >= 0; i--) {
       
        RC0 = 0; RC1 = 1; delay();        
        RC2 = 1; RC3 = 0; delay();        
        RD0 = 0; RD1 = 1; delay();
        RC0 = 1; RC1 = 0; delay();
        RC2 = 0; RC3 = 1; delay();
        RD0 = 1; RD1 = 0; delay();
       
        RC0 = 0; RC1 = 1; delay();        
        RC2 = 1; RC3 = 0; delay();        
        RD0 = 0; RD1 = 1; delay();
        RC0 = 1; RC1 = 0; delay();
        RC2 = 0; RC3 = 1; delay();
        RD0 = 1; RD1 = 0; delay();
       
        RC0 = 0; RC1 = 1; delay();        
        RC2 = 1; RC3 = 0; delay();        
        RD0 = 0; RD1 = 1; delay();
        RC0 = 1; RC1 = 0; delay();
        RC2 = 0; RC3 = 1; delay();
        RD0 = 1; RD1 = 0; delay();

    }
}