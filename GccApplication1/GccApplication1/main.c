/*
SISTEMAS EMBARCADOS

ESTUDANTE: RAPHAEL LINS DI PACE
DATA: 28/06/2025

APLICAÇÕES - ATMEGA328P
*/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// ======================================================================
// Definições e funções da biblioteca Seven Segment Display
// ======================================================================

static const uint8_t sevenSegmentCode[16] = {
	0x7E, 0x30, 0x6D, 0x79,
	0x33, 0x5B, 0x5F, 0x70,
	0x7F, 0x7B, 0x77, 0x1F,
	0x4E, 0x3D, 0x4F, 0x47
};

/**
* @brief Configura os pinos necessários para o display de 7 segmentos.
*
* PD0–PD7 será usado para os segmentos (a–g), e PB0–PB3 para selecionar os dígitos.
*/
void setupSevenSegmentDisplay(void) {
	DDRD |= 0x7F; // PORTD como saída (segmentos)
	DDRB |= 0x0F; // PB0–PB3 como saída (seleção dos dígitos)
}

/**
* @brief Mostra um valor (0–F) em um display específico.
*
* @param value Valor hexadecimal a ser exibido (0x0 a 0xF).
* @param displayAddress Endereço do display (0 a 3) correspondente a PB0–PB3.
*/
void showSevenSegmentDisplayValue(uint8_t value, uint8_t displayAddress) {
	if (value < 16) {
		PORTD &= 0x80; // Limpa os 7 últimos bits da porta D
		PORTD |= sevenSegmentCode[value]; // Define o valor a ser mostrado
	}
	PORTB &= 0xF0; // Limpa PB0–PB3
	PORTB |= ((1 << displayAddress) & 0x0F); // Ativa o display desejado
}

/**
* @brief Limpa a seleção de displays, desligando todos.
*/
void cleanSevenSegmentDisplay(void) {
	PORTB &= 0xF0; // Limpa PB0–PB3
}

/**
* @brief Mostra valores individuais em cada um dos 4 displays.
*/
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

// ======================================================================
// Função principal
// ======================================================================

int main(void) {
	uint16_t num0 = 0, num1 = 0;
	setupSevenSegmentDisplay(); // Inicializa os pinos do display de 7 segmentos

	while (1) {
		showAllSevenSegmentDisplay((num1)%10, (num1+1)%10, (num1+2)%10, (num1+3)%10);

		num0++;
		num1 = (num0/100)%10;
	}

	return 0;
}