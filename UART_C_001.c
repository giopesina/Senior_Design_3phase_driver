#include <xc.h>
#include <stdio.h>
#include <stdlib.h>

#define _XTAL_FREQ 20000000  // 20MHz Crystal
#define LED PORTBbits.RB0    // Define LED as RB0
#define LED1 PORTBbits.RB1    // Define LED as RB1
#define LED2 PORTBbits.RB2    // Define LED as RB2

// CONFIGURATION BITS
#pragma config FOSC = HS    // High-Speed Oscillator
#pragma config WDTE = OFF   // Watchdog Timer Disabled
#pragma config PWRTE = OFF  // Power-up Timer Disabled
#pragma config BOREN = OFF   // Brown-out Reset Enabled
#pragma config LVP = OFF    // Low-Voltage Programming Disabled
#pragma config CPD = OFF    // Data EEPROM Memory Code Protection Disabled
#pragma config WRT = OFF    // Flash Program Memory Write Protection Disabled
#pragma config CP = OFF     // Flash Program Memory Code Protection Disabled

volatile char stateValue;
volatile char indexValue;
volatile int timeSet;
volatile int state;

/*
 All function are passing the build tests
 * 
 * however the rate at which the pi and pic communicate seems to be extremely delayed 
 * when printing the raw data it showed that random bytes were being sent intiallay and then after what seemed like 15 repetiions 
 * a line of correct bytes was recieved 
 * this took around 30secs to a minute per process and delay funcions were removed but no change was seen thus they were late left back in
 * 
 * below I intergrated the functions from 3 phase code but am unable to fully intergrate without further guidance 
 * 
 * I was able to establish a bi directional communication path between the pi and pi in hopes that this will allow us to send data until a recieved data on pi instructs changes
 * 
 * for example sending '0' from pi will be read here and then the pic should send a '1' back this will be recieved by the pi as b'1' in this case we can have the pi then send back a different variable or do another function
 * 
 * 
 */

// Simple delay function
void delay(int setTime) {
    for (int i = setTime; i > 0; i--) {
        __delay_ms(1);
    }
}

// Initialize controller
void controller_init(void) {
    TRISB0 = 0; // Set RB0 as Output
    TRISB1 = 0;
    TRISB2 = 0;
    TRISD = 0;
    PORTD = 0;
    LED = 0;    // Ensure LED is OFF at the start
    LED1 = 0;
    LED2 = 0;
}
// UART Initialization
void UART_Init(long baud_rate) {
    OSCCON = 0b01110101;
    TRISC6 = 0; // TX Output
    TRISC7 = 1; // RX Input
    
    BAUDCTL = 0b00000000;
    BRGH = 0;
    SPBRG = 12;
    TXSTA = 0b00100010;
    RCSTA = 0b10010010;  // Enable Serial Port and Continuous Receive mode
    
    PIE1bits.RCIE = 1;  // Enable UART Receive Interrupt
    INTCONbits.PEIE = 1; // Enable Peripheral Interrupts
    INTCONbits.GIE = 1;  // Enable Global Interrupts
}

// UART Transmit Function
void UART_Transmit(char data) {
    while (!TXSTAbits.TRMT); // Wait until buffer is empty
    TXREG = data; // Load data into TX register
}

// UART Read Function
char UART_Read() {
    while (!PIR1bits.RCIF); // Wait for data to be received
    
    if (RCSTAbits.OERR) { // Check for Overrun Error
        RCSTAbits.CREN = 0; // Reset the receiver
        RCSTAbits.CREN = 1;
    }
    return RCREG; // Return received data
}

void global_init(void) {
    stateValue = 0;
    indexValue = 0;
    timeSet = 0;
    state = 0;
}

void phaseImp(void) {
    RD0 = 0b0;
    delay(timeSet);
    RD1 = 0b1;
    delay(timeSet);
    RD2 = 0b0;
    delay(timeSet);
    RD0 = 0b1;
    delay(timeSet);
    RD1 = 0b0;
    delay(timeSet);
    RD2 = 0b1;
    delay(timeSet);
}

void statusCheck(void) {

    switch (stateValue) {
        case '0': // Off Mode
            RD3 = 1;
            RD4 = 0;
            RD5 = 0;
            break;
        case '1': // Diagnostic Mode
            RD3 = 0;
            RD4 = 1;
            RD5 = 0;
            break;
        case '2': // Run Mode
            RD3 = 0;
            RD4 = 0;
            RD5 = 1;
            break;
    }
}

// UART Interrupt Service Routine
void __interrupt() UART_ISR(void) {
    if (PIR1bits.RCIF) { // UART Receive Interrupt
        stateValue = UART_Read();  // Read received data
        state = stateValue;  // Update system state
        indexValue = UART_Read(); // Read second byte (if available)
        timeSet = indexValue;
        PIR1bits.RCIF = 0;  // Clear Receive Interrupt Flag
    }
}

// Main Function
void main() {
    UART_Init(9600); // Initialize UART at 9600 baud
    controller_init();
    global_init();
    __delay_ms(100);

    while (1) {
        
         // Send '1' over UART
        //__delay_ms(100);
        //*
        char received_data = UART_Read(); // Read received data
        
        if (received_data == '0') {
            LED = 1;  // Turn LED ON if '1' is received
            __delay_ms(100);
            LED = 0;  // Turn LED OFF
            //UART_Transmit('1');
            __delay_ms(100);
            while (received_data == '0'){
                LED = 1;
                UART_Transmit('1');
                LED = 0;  // Turn LED OFF
            }
            
        }
        else if (received_data == '1') {
            LED1 = 1;  // Turn LED ON if '1' is received
            __delay_ms(100);
            LED1 = 0;  // Turn LED OFF
            __delay_ms(100);
            while (received_data == '1'){
                LED = 1;
                UART_Transmit('2');
                LED = 0;  // Turn LED OFF
            }
        }
        else if (received_data == '2') {
            LED2 = 1;  // Turn LED ON if '1' is received
            __delay_ms(100);
            LED2 = 0;  // Turn LED OFF
            __delay_ms(100);
            while (received_data == '2'){
                LED = 1;
                UART_Transmit('0');
                LED = 0;  // Turn LED OFF
            }
        }
        //*/

        //UART_Transmit('A'); // Send 'A' over UART
    }
}

