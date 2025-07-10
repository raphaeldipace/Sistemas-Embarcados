/*
SISTEMAS EMBARCADOS

ESTUDANTE: RAPHAEL LINS DI PACE
DATA: 10/07/2025

INTERRUPÇÕES - ATMEGA328P
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

/* -------------------------- Seven Segment Display -------------------------- */

static const uint8_t sevenSegmentCode[16] = {
	0x7E, 0x30, 0x6D, 0x79,
	0x33, 0x5B, 0x5F, 0x70,
	0x7F, 0x7B, 0x77, 0x1F,
	0x4E, 0x3D, 0x4F, 0x47
};

void setupSevenSegmentDisplay(void) {
	DDRD |= 0x7F;  // PD0-PD6 como saída (segmentos)
	DDRB |= 0x0F;  // PB0-PB3 como saída (seleção dos dígitos)
}

void showSevenSegmentDisplayValue(uint8_t value, uint8_t displayAddress) {
	if (value < 16) {
		PORTD &= 0x80;  // Limpa segmentos
		PORTD |= sevenSegmentCode[value];
	}

	PORTB &= 0xF0;                    // Limpa PB0–PB3
	PORTB |= ((1 << displayAddress) & 0x0F);  // Seleciona o display
}

void cleanSevenSegmentDisplay(void) {
	PORTB &= 0xF0;  // Desliga todos os displays
}

void showAllSevenSegmentDisplay(uint8_t valueDisp_0, uint8_t valueDisp_1, uint8_t valueDisp_2, uint8_t valueDisp_3) {
	cleanSevenSegmentDisplay();
	showSevenSegmentDisplayValue(valueDisp_0, 0);
	_delay_ms(2);

	cleanSevenSegmentDisplay();
	showSevenSegmentDisplayValue(valueDisp_1, 1);
	_delay_ms(2);

	cleanSevenSegmentDisplay();
	showSevenSegmentDisplayValue(valueDisp_2, 2);
	_delay_ms(2);

	cleanSevenSegmentDisplay();
	showSevenSegmentDisplayValue(valueDisp_3, 3);
	_delay_ms(2);
}

/* -------------------------- Buttons -------------------------- */

typedef enum {
	BUTTON_A = 0,  // PC0
	BUTTON_B = 1,  // PC1
	BUTTON_C = 2,  // PC2
	BUTTON_D = 3   // PC3
} ButtonPin;

typedef void (*button_callback_t)(void* arg);

typedef struct {
	button_callback_t callback;
	void* arg;
} button_handler_t;

static volatile button_handler_t button_handlers[4] = {0};

void setupButtons(void) {
	DDRC &= ~(0x0F);  // PC0–PC3 como entrada
	PORTC |= 0x0F;    // Pull-up interno habilitado

	PCICR |= (1 << PCIE1);  // Habilita interrupção no grupo PCINT[14:8]
	PCMSK1 |= 0x0F;         // Habilita interrupção nos pinos PC0–PC3

	sei();  // Habilita interrupções globais
}

void register_button_callback(ButtonPin button, button_callback_t callback, void* arg) {
	if (button >= BUTTON_A && button <= BUTTON_D) {
		button_handlers[button].callback = callback;
		button_handlers[button].arg = arg;
	}
}

ISR(PCINT1_vect) {
	for (uint8_t pin = 0; pin < 4; pin++) {
		if (!(PINC & (1 << pin)) && button_handlers[pin].callback) {
			button_handlers[pin].callback(button_handlers[pin].arg);
		}
	}
}

/* -------------------------- Application -------------------------- */

void addNum(void* arg) {
	uint8_t* num = (uint8_t*)arg;
	(*num)++;
	(*num) %= 10;
}

int main(void) {
	uint8_t numU = 0, numD = 0, numC = 0, numM = 0;

	setupButtons();
	setupSevenSegmentDisplay();

	register_button_callback(BUTTON_A, addNum, &numM);
	register_button_callback(BUTTON_B, addNum, &numC);
	register_button_callback(BUTTON_C, addNum, &numD);
	register_button_callback(BUTTON_D, addNum, &numU);

	while (1) {
		showAllSevenSegmentDisplay(numM, numC, numD, numU);
	}
}
