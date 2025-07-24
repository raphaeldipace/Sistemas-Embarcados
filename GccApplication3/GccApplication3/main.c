#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

/* ===== Display de 7 Segmentos ===== */
static const uint8_t sevenSeg[16] = {
		0x7E, 0x30, 0x6D, 0x79,
		0x33, 0x5B, 0x5F, 0x70,
		0x7F, 0x7B, 0x77, 0x1F,
		0x4E, 0x3D, 0x4F, 0x47
};

void setup7S(void){
	DDRD |= 0x7F;  // segmentos PD0–PD6
	DDRB |= 0x0F;  // dígitos PB0–PB3
}
static inline void showDigit(uint8_t v, uint8_t d){
	PORTD = (PORTD & 0x80) | sevenSeg[v];
	PORTB = (PORTB & 0xF0) | (1<<d);
}

/* ===== Botões PCINT ===== */
typedef enum { A=0,B,C,D } Btn;
typedef void (*cb)(void*);
struct { cb f; void* a; } h[4];

ISR(PCINT1_vect){
	for(uint8_t i=0;i<4;i++){
		if(!(PINC&(1<<i)) && h[i].f) h[i].f(h[i].a);
	}
}
void setupBtns(void){
	DDRC &= ~0x0F; PORTC |= 0x0F;
	PCICR |= (1<<PCIE1); PCMSK1 |= 0x0F;
}
void onBtn(Btn b, cb f, void* a){ h[b].f=f; h[b].a=a; }

/* ===== UART 9600 8N1 ===== */
#define BAUD 9600
#define UBRRVAL ((F_CPU/(16UL*BAUD))-1)
void setupUART(void){
	UBRR0H=UBRRVAL>>8; UBRR0L=UBRRVAL;
	UCSR0B=(1<<TXEN0);
	UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);
}
void uart_putc(char c){
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0 = c;
}
void uart_puts(const char* s){
	while(*s) uart_putc(*s++);
	uart_putc('\n');
}

/* ===== ADC polling em ADC5 ===== */
void adc_init(void){
	ADMUX  = (1<<REFS0) | (5 & 0x0F);        // AVCC, canal 5
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
uint16_t adc_read(void){
	ADCSRA |= (1<<ADSC);
	while(ADCSRA&(1<<ADSC));
	uint16_t l = ADCL, h = ADCH;
	return (h<<8)|l;
}

/* ===== callbacks ===== */
void inc(void* p){
	uint8_t* x = p;
	*x = (*x + 1) % 10;
}

/* ===== main ===== */
int main(void){
	uint8_t u=0,d=0,c=0,m=0;

	setup7S();
	setupBtns();
	setupUART();
	adc_init();
	sei();

	onBtn(A, inc, &m);
	onBtn(B, inc, &c);
	onBtn(C, inc, &d);
	onBtn(D, inc, &u);

	uint16_t timer = 0;
	while(1){
		// ————————————— multiplexa display sem parar —————————————
		showDigit(u,3); _delay_ms(2);
		showDigit(d,2); _delay_ms(2);
		showDigit(c,1); _delay_ms(2);
		showDigit(m,0); _delay_ms(2);

		// envia UART a cada ~200ms
		if (++timer >= 50) {
			timer = 0;
			uint16_t raw = adc_read();
			char buf[6];
			itoa(raw, buf, 10);
			uart_puts(buf);
		}
	}
}
