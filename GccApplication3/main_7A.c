#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR ((F_CPU / (16UL * BAUD)) - 1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdint.h>

// ---------------- UART ----------------
void setupUART(uint16_t ubrr) {
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void transmitUART_8bits(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void transmitUART_16bits(uint16_t data) {
    transmitUART_8bits((uint8_t)(data >> 8)); // MSB
    transmitUART_8bits((uint8_t)data);        // LSB
}

uint8_t receiveUART(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void sendASCIIFromInt(uint16_t num) {
    char buffer[6];
    itoa(num, buffer, 10);
    for (char *p = buffer; *p; p++) {
        transmitUART_8bits(*p);
        _delay_ms(2);
    }
    transmitUART_8bits('\n');
}

// ---------------- SENSOR DE TEMPERATURA ----------------
volatile uint16_t TemperatureSensorLevel = 0;

ISR(ADC_vect) {
    TemperatureSensorLevel = ADC;
}

void setupTemperatureSensor(void) {
    DDRC  &= ~(1<<5); // PC5 como entrada
    PORTC &= ~(1<<5); // Desabilita pull-up

    ADMUX  = 0b01000101; // Vref=AVCC, canal ADC5 (PC5)
    ADCSRA = 0b11101111; // ADEN=1, ADIE=1, ADATE=1, prescaler=128
    ADCSRB = 0x00;       // Modo de conversão contínua
    DIDR0 |= (1<<ADC5D); // Desabilita buffer digital no PC5

    sei(); // Habilita interrupções globais
}

uint16_t readTemperatureSensorLevel(void) {
    return TemperatureSensorLevel;
}

uint16_t readTemperatureDegrees(void) {
    return (TemperatureSensorLevel - 120) / 8; // Ajuste empírico
}

// ---------------- MAIN ----------------
int main(void) {
    DDRD |= (1<<0);   // PD0 como saída
    PORTD |= (1<<0);  // Seta PD0

    setupUART(MYUBRR);
    setupTemperatureSensor();

    while (1) {
        PORTD ^= (1<<0); // Toggle LED
        sendASCIIFromInt(readTemperatureDegrees());
        _delay_ms(100);
    }

    return 0;
}
