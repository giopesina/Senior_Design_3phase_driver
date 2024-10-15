#include <xc.h>
#include <math.h>  // Include math library for sine function

#define _XTAL_FREQ 8000000   // Use 8 MHz internal oscillator
#define NUM_POINTS 60        // Number of points for half-cycle

// Sine lookup table for generating PWM duty cycle values
const float sine_table[NUM_POINTS] = {
    0.0523, 0.1045, 0.1564, 0.2079, 0.2588, 0.3090, 0.3584, 0.4067, 0.4539, 0.5000,
    0.5446, 0.5878, 0.6293, 0.6691, 0.7071, 0.7431, 0.7771, 0.8090, 0.8387, 0.8660,
    0.8910, 0.9135, 0.9336, 0.9511, 0.9659, 0.9781, 0.9877, 0.9945, 0.9986, 1.0000,
    0.9986, 0.9945, 0.9877, 0.9781, 0.9659, 0.9511, 0.9336, 0.9135, 0.8910, 0.8660,
    0.8387, 0.8090, 0.7771, 0.7431, 0.7071, 0.6691, 0.6293, 0.5878, 0.5446, 0.5000,
    0.4539, 0.4067, 0.3584, 0.3090, 0.2588, 0.2079, 0.1564, 0.1045, 0.0523, 0.0000
};

// Initialize ADC to read from AN0 (potentiometer)
void adc_init() {
    ADCON1 = 0x0E; // Configure AN0 as analog input
    ADCON0 = 0x01; // Enable ADC module
}

// Read ADC value from potentiometer
int adc_read() {
    ADCON0 |= 0x02;  // Start ADC conversion
    while(ADCON0 & 0x02);  // Wait for conversion to complete
    return ((ADRESH << 8) + ADRESL);  // Return 10-bit result
}

// Map ADC value to frequency (60 Hz to 20 kHz)
int map_adc_to_frequency(int adc_value) {
    return (adc_value * (20000 - 60)) / 1023 + 60;
}

// Set PWM frequency using Timer2
void set_pwm_frequency(int frequency) {
    int pwm_period = (_XTAL_FREQ / (4 * frequency)) - 1;  // Calculate Timer2 period
    PR2 = pwm_period;  // Set Timer2 period for desired frequency
}

// PWM initialization for CCP1 and CCP2 (used for positive and negative IGBTs)
void pwm_init() {
    // Configure Timer2 for PWM
    T2CON = 0b00000100;  // Timer2 ON, prescaler = 1

    // Configure CCP1 and CCP2 for PWM mode
    CCP1CON = 0b00001100;  // CCP1 in PWM mode
    CCP2CON = 0b00001100;  // CCP2 in PWM mode

    // Set PWM pins as output (RC2/CCP1 and RC1/CCP2)
    TRISCbits.TRISC2 = 0;  // RC2 as output for CCP1
    TRISCbits.TRISC1 = 0;  // RC1 as output for CCP2

    // Set initial duty cycle to 0
    CCPR1L = 0x00;  // Initial duty cycle for CCP1
    CCPR2L = 0x00;  // Initial duty cycle for CCP2
}

// Generate PWM signals for one phase (positive and negative)
void generate_single_phase(int frequency) {
    set_pwm_frequency(frequency);  // Set PWM frequency based on potentiometer

    for (int i = 0; i < NUM_POINTS; i += 1) {
        // Set duty cycle for the positive IGBT
        int duty_cycle_pos = sine_table[i] * 255;  // Scale sine value to 8-bit PWM
        CCPR1L = duty_cycle_pos;  // Set duty cycle for positive PWM (IGBT for positive)

        // Set duty cycle for the negative IGBT (complementary)
        int duty_cycle_neg = 255 - duty_cycle_pos;  // Complement of the positive signal
        CCPR2L = duty_cycle_neg;  // Set duty cycle for negative PWM (IGBT for negative)

        __delay_us(50);  // Small delay between pulses (adjust as needed)
    }
}

// Main function
void main() {
    // Initialization
    adc_init();  // Initialize ADC for potentiometer
    pwm_init();  // Initialize PWM
    
    // Main loop
    while (1) {
        int pot_value = adc_read();  // Read potentiometer value
        int frequency = map_adc_to_frequency(pot_value);  // Map to frequency

        generate_single_phase(frequency);  // Generate positive and negative signals for one phase
    }
}