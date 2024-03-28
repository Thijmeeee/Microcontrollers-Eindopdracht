
/*
 * Microcontrollers - Eindopdracht
 *
 * Created: 01/03/2024 12:36:36
 * Author : Rubbie
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define LCD_E 3
#define LCD_RS 2
#define BIT(x) (1 << (x))
#define BIT_OFF(x) (0 << (x))

#define MAX_BUZZER_THRESHOLD 30000
#define MIN_BUZZER_THRESHOLD 7000

void lcd_strobe_lcd_e(void);
void init_4bits_mode(void);
void set_cursor(int position);

volatile int distance = 0;
volatile uint16_t delay_count = 35000;
volatile uint8_t valueRed = 0x00;
volatile uint8_t valueGreen = 0x00;


ISR(TIMER1_COMPA_vect) {
	if (delay_count < MIN_BUZZER_THRESHOLD)
	{
		PORTF = BIT(1);
		delay_count = MIN_BUZZER_THRESHOLD;
		} else if (delay_count > MAX_BUZZER_THRESHOLD){
		delay_count = MAX_BUZZER_THRESHOLD;
		PORTF = 0;
		} else {
		PORTF ^= BIT(1);
	}
	delay_count = distance;
	OCR1A = delay_count;
}

ISR (INT0_vect){
	// Lees de waarde van Timer 3
	int timerValue = TCNT3;
	distance = timerValue;
	
}

void wait(int ms) {
	for (int i = 0; i < ms; i++) {
		_delay_ms(1);
	}
}

void wait_micro(int us) {
	for (int i = 0; i < us; i++) {
		_delay_us(1);
	}
}

void lcd_command(unsigned char data) {
	PORTC = data & 0xF0; //hoge nubble
	lcd_strobe_lcd_e();		//wait 1 ms
	
	PORTC = 0x04;
	PORTC = (data & 0x0F) << 4;
	
	lcd_strobe_lcd_e();
	PORTC = 0x00;
}

void lcd_writeChar(unsigned char data) {
	PORTC = data & 0xF0;
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
	
	PORTC = (data & 0x0F) <<4;
	PORTC |= (1<<LCD_RS);
	lcd_strobe_lcd_e();
	
}

void lcd_strobe_lcd_e(void) {
	PORTC |= (1<<LCD_E);
	_delay_ms(1);
	PORTC &= ~(1<<LCD_E);
	_delay_ms(1);
}

void init_4bits_mode(void) {
	
	DDRC = 0xFF;
	PORTC = 0x00;
	
	PORTC = 0x20;
	lcd_strobe_lcd_e();
	
	PORTC = 0x20;
	lcd_strobe_lcd_e();
	
	PORTC = 0x20;
	lcd_strobe_lcd_e();
	
	PORTC = 0x80;
	lcd_strobe_lcd_e();
	
	PORTC = 0x00;
	lcd_strobe_lcd_e();
	PORTC = 0xF0;
	lcd_strobe_lcd_e();
	
	PORTC = 0x00;
	lcd_strobe_lcd_e();
	PORTC = 0x60;
	lcd_strobe_lcd_e();
	
	lcd_command(0x01);
}

void init_buzzer(){
	DDRF |= BIT(1); // Set the buzzer pin
	
	TCCR1B |= (1 << WGM12); // Configure timer 1 for CTC mode
	TIMSK |= (1 << OCIE1A); // Enable CTC interrupt
	OCR1A = delay_count; // Set initial delay value
	
	TCCR1B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu / 64
}

void init_ultrasoon(){
	DDRD = 0b00000010; // SET D0 AS INPUT (ECHO) AND D1 AS OUTPUT (TRIG)
	
	TCCR3A = 0;
	TCCR3B = 0;
	TCNT3 = 0;
	
	TCCR3B |= (1 << CS30);
}

void init_interrupts(){
	// INIT Interrupt Hardware
	EICRA |= 0b00000010; // INT0 falling edge
	EIMSK |= 0x01; // Enable INT0

	// enable global interrupt system
	sei();
}

void init_leds(){
	DDRB |= BIT(7);
	DDRB |= BIT(4);
	
	TCCR2 |= (1 << WGM20) | (1 << COM21) | (1 << CS20);
	TCCR0 |= (1 << WGM00) | (1 << COM01) | (1 << CS00);
}

void display_text(char *str) {
	for (;*str; str++) {
		lcd_writeChar(*str);
	}
}

void set_cursor(int position){
	unsigned char p = 0x80 + position;
	lcd_command(p);
}


void send_pulse(){
	// Start Timer 3
	TCCR3B |= (1 << CS30);
	TCNT3 = 0;
	
	PORTD |= BIT(1);
	wait_micro(10); // Send a pulse of minimal timer period 10us, this will make the Ultrasonic module to send a burst of data.
	PORTD ^= BIT(1);
}

void control_rgb_color(){
	if (distance < MIN_BUZZER_THRESHOLD) {
		valueRed = 255;
		valueGreen = 0;
		} else if (distance >= MAX_BUZZER_THRESHOLD) {
		valueRed = 0;
		valueGreen = 255;
		} else {
		float ratio = (float)(distance - MIN_BUZZER_THRESHOLD) / (float)(MAX_BUZZER_THRESHOLD - MIN_BUZZER_THRESHOLD);
		valueRed = 255 * (1 - ratio);
		valueGreen = 255 * ratio;
	}
	
	OCR2 = valueRed;
	OCR0 = valueGreen;
}


int main(void)
{	
	init_leds();
	init_interrupts();
	
	init_4bits_mode();
	char buffer[20];
	
	init_buzzer();
	init_ultrasoon();

	while (1){
		send_pulse();
		wait(250);
		
		if (distance > 0){
			lcd_command(0x01);
			sprintf(buffer, "%d", distance);
			display_text(buffer);
		}
		
		control_rgb_color();

	}
}

