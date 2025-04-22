#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

// CONFIGURATION BITS
#pragma config FOSC = HS    // High-Speed Oscillator
#pragma config WDTE = OFF       // Watchdog Timer Disabled
#pragma config PWRTE = OFF      // Power-up Timer Disabled
#pragma config BOREN = OFF      // Brown-out Reset Enabled
#pragma config LVP = OFF        // Low-Voltage Programming Disabled
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection Disabled
#pragma config WRT = OFF        // Flash Program Memory Write Protection Disabled
#pragma config CP = OFF         // Flash Program Memory Code Protection Disabled

#define _XTAL_FREQ 8000000      // 8 MHz INTOSC



// Global Variables
int fidelity;
unsigned char stateValue, indexValue, timeSet, timer;

// Function Prototypes
void controller_init(void);
void global_init(void);
void statusCheck(void);
void phaseImp(void);
void delay(int ms);
void pulse(void);
char UART_Read(void);

//UART Function Prototypes
void UART_Init(long baud_rate);
void __interrupt() UART_ISR(void);
void UART_Transmit(char data);

//Main Function
int main(void) {

    // Controller Initialization
    controller_init();
    global_init();
    UART_Init(9600);                 // Initialize UART with 9600 baud rate
    
    while (1) {
        //Main Program
        statusCheck();  
    }
}

// Function Definitions
void statusCheck(void) {

    if (stateValue == 0) {         // OFF
        RD3 = 1;
        RD4 = 0;
        RD5 = 0;
        UART_Transmit('A');
    } 

    else if (stateValue == 1) {    // STANDBY
        RD3 = 0;
        RD4 = 1;
        RD5 = 0;
        pulse();
        UART_Transmit('B');
    } 

    else if (stateValue == 2) {    // RUN
        RD3 = 0;
        RD4 = 0;
        RD5 = 1;
        phaseImp();
        UART_Transmit('C');
    }

    else {
        RD3 = 1;
        RD4 = 0; 
        RD5 = 1;
    }
}

void pulse(void){

    RD6 = 1;
    for(int i = 0; i < 2; i++){
        __delay_ms(500);
    }
    RD6 = 0;
    for(int i = 0; i < 2; i++){
        __delay_ms(500);
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

    TRISCbits.TRISD0 = 0;                       //Phase 3
    TRISCbits.TRISD1 = 0;                       //Complement of P3

    TRISA = 0x11;  
    PORTA = 0x00;//Output/Input   (1 = Input, 0 = Output)
}


// UART Initialization
void UART_Init(long baud_rate) {
    //OSCCON = 0b01110000;                        // Set Internal Oscillator to 8 MHz

    TRISC6 = 0;                                 // TX Output
    TRISC7 = 1;                                 // RX Input
    
    BAUDCTL = 0;                                // 8-bit baud rate generator
    BRGH = 1;                                   // High Baud Rate mode
    SPBRG = (_XTAL_FREQ / (16 * baud_rate)) - 1;// Adjusted for 8 MHz

    TXSTA = 0b00100100;                         // Enable Transmit, BRGH = 1
    RCSTA = 0b10010000;                         // Enable Serial Port and Continuous Receive mode (8-bit mode)

    PIE1bits.RCIE = 1;  
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE = 1;
}

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


// Phase Implementation
void phaseImp(void) {
    for (int i = fidelity; i >= 0; i--) {
        // Step 1: A⁺, B⁻,  C off
        RC0 = 1; RC1 = 0;   // A+
        RC2 = 0; RC3 = 1;   // B–
        RD0 = 0; RD1 = 0;   // C off
        delay();

        // Step 2: A⁺, C⁻,  B off
        RC0 = 1; RC1 = 0;   // A+
        RC2 = 0; RC3 = 0;   // B off
        RD0 = 0; RD1 = 1;   // C–
        delay();

        // Step 3: B⁺, C⁻,  A off
        RC0 = 0; RC1 = 0;   // A off
        RC2 = 1; RC3 = 0;   // B+
        RD0 = 0; RD1 = 1;   // C–
        delay();

        // Step 4: B⁺, A⁻,  C off
        RC0 = 0; RC1 = 1;   // A–
        RC2 = 1; RC3 = 0;   // B+
        RD0 = 0; RD1 = 0;   // C off
        delay();

        // Step 5: C⁺, A⁻,  B off
        RC0 = 0; RC1 = 1;   // A–
        RC2 = 0; RC3 = 0;   // B off
        RD0 = 1; RD1 = 0;   // C+
        delay();

        // Step 6: C⁺, B⁻,  A off
        RC0 = 0; RC1 = 0;   // A off
        RC2 = 0; RC3 = 1;   // B–
        RD0 = 1; RD1 = 0;   // C+
        delay();
    }
}

}

// Global variable initialization
void global_init(void) {
    stateValue = 0;                   // Used to determine if the system is on or off
    indexValue = 0;                   // Used to determine the pulsing rate of the system
    timeSet = 0;                        // Used, but unsure how - Zarek 
    state = 0;                          // I'm not sure how this is used - Zarek
    fidelity = 3;
}


// UART Interrupt Service Routine
void __interrupt() UART_ISR(void) {
    if (PIR1bits.RCIF) {                // UART Receive Interrupt
        if (RCSTAbits.OERR) {           // Handle Overrun Error
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        
        unsigned char receivedData = RCREG;      // Read first byte
        if(RA0 == 1){
            timeSet = receivedData;
        }
        else if (RA0 == 0){
            stateValue = receivedData;
        }
        else{
            while(1){
                for(int i = 0; i < 1; i++){
                    RD4 = 1;
                    __delay_ms(500);
                }
                for(int i = 0; i < 1; i++){
                    RD4 = 0;
                    __delay_ms(500);
                }
            }
        }
        //stateValue = receivedData;      // Store received byte

        
        // Check if another byte is available before reading again
        if (PIR1bits.RCIF) {
            indexValue = RCREG; 
            timeSet = indexValue;
        }
        
    }
}

// UART Read Function
char UART_Read(void) {
    int timeout = 10000;                // Define a timeout limit
    while (!PIR1bits.RCIF) {
        if (--timeout == 0) return -1;  // Return -1 if no data received
    }
    
    if (RCSTAbits.OERR) {               // Check for Overrun Error
        RCSTAbits.CREN = 0;             // Reset the receiver
        RCSTAbits.CREN = 1;
    }
    return RCREG;                       // Return received data
}

// UART Transmit Function
void UART_Transmit(char data) {
    while (!PIR1bits.TXIF);             // Wait until TXREG is empty
    TXREG = data;                       // Load data into TX register
}



