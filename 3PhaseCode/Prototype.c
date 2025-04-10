/* 
 * File:   PIC_SOURCE.c
 * Author: zaz333
 *
 * Created on March 18, 2025, 5:43 PM
*/
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
unsigned char stateValue;
unsigned char indexValue;
int timeSet;
unsigned char state;

// Function Prototypes
void controller_init(void);
void global_init(void);
void statusCheck(void);
void phaseImp(void);
void delay(int ms);
void pulse(void);

int main(void) {
    controller_init();
    global_init();
    UART_Init(9600);                 // Initialize UART with 9600 baud rate
    
    while (1) {
        statusCheck();  
    }
}

// Function Definitions
void statusCheck(void) {
    if (stateValue == '0') {         // OFF
        RD3 = 1;
        RD4 = 0;
        RD5 = 0;
    } 
    else if (stateValue == '1') {    // STANDBY
        RD3 = 0;
        RD4 = 1;
        RD5 = 0;
        pulse();
    } 
    else if (stateValue == '2') {    // RUN
        RD3 = 0;
        RD4 = 0;
        RD5 = 1;
        phaseImp();
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

    
    TRISA = 0x11;  
    PORTA = 0x00;//Output/Input   (1 = Input, 0 = Output)
}

/*
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
*/


// Delay function
void delay(int ms) {
    if ( ms < 1){
        __delay_us(50);
    }
    else if (ms < 2){
        __delay_us(100)
    }
    else{
        for(unsigned char i = '2'; i < '255'; i++){
            __delay_us(50);
        }
    }
    
}

// Phase Implementation
void phaseImp(void) {
    for(int i = fidelity; i >= 0; i--){
        RD0 = 0;
        delay(timeSet);
        RD1 = 1;
        delay(timeSet);
        RD2 = 0;
        delay(timeSet);
        RD0 = 1;
        delay(timeSet);
        RD1 = 0;
        delay(timeSet);
        RD2 = 1;
        delay(timeSet);
    }   
}

// Global variable initialization
void global_init(void) {
    stateValue = '0';                   // Used to determine if the system is on or off
    indexValue = '0';                   // Used to determine the pulsing rate of the system
    timeSet = 0;                        // Used, but unsure how - Zarek 
    state = 0;                          // I'm not sure how this is used - Zarek
    fidelity = 3;
}

/*

// UART Interrupt Service Routine
void __interrupt() UART_ISR(void) {
    if (PIR1bits.RCIF) {                // UART Receive Interrupt
        if (RCSTAbits.OERR) {           // Handle Overrun Error
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        }
        
        char receivedData = RCREG;      // Read first byte
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

        /* 
        // Check if another byte is available before reading again
        if (PIR1bits.RCIF) {
            indexValue = RCREG; 
            timeSet = indexValue;
        }
        */
    //}
//}

/*

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
*/


