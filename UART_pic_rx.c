#include <xc.h>

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

// Main Function
void main() {
    TRISB0 = 0; // Set RB0 as Output
    TRISB1 = 0;
    TRISB2 = 0;
    LED = 0;    // Ensure LED is OFF at the start
    LED1 = 0;
    LED2 = 0;

    UART_Init(9600); // Initialize UART at 9600 baud
    __delay_ms(100);

    while (1) {
        UART_Transmit('0'); // Send '1' over UART
        __delay_ms(100);
        char received_data = UART_Read(); // Read received data
        
        if (received_data == '0') {
            LED = 1;  // Turn LED ON if '1' is received
            __delay_ms(100);
            LED = 0;  // Turn LED OFF
            __delay_ms(100);
        }
        else if (received_data == '1') {
            LED1 = 1;  // Turn LED ON if '1' is received
            __delay_ms(100);
            LED1 = 0;  // Turn LED OFF
            __delay_ms(100);
        }
        else if (received_data == '2') {
            LED2 = 1;  // Turn LED ON if '1' is received
            __delay_ms(100);
            LED2 = 0;  // Turn LED OFF
            __delay_ms(100);
        }

        UART_Transmit('A'); // Send 'A' over UART
    }
}
