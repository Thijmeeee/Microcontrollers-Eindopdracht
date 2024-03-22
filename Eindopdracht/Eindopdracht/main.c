
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

void lcd_strobe_lcd_e(void);
void init_4bits_mode(void);
void set_cursor(int position);

volatile int distance = 0;

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

void display_text(char *str) {
	for (;*str; str++) {
		lcd_writeChar(*str);
	}
}

void set_cursor(int position){
	unsigned char p = 0x80 + position;
	lcd_command(p);
}

void setupADC(){
	//DDRF &= ~(1<<PF0);
	
	// Ultrasoon: ECHO (D0) = INPUT | TRIG (D1) = OUTPUT
	DDRF = 0b00000010; // SET D0 AS INPUT AND D1 AS OUTPUT
	
	ADMUX = 0b01100000; // Voltage = AVCC with external capacitor at AREF pin | Last 4 bits are the port. Echo is on D0, so all 4 bits are 0
	
	ADCSRA = 0b11000110; // 7 bit ENABLES ADC | 6 bit starts the conversion | 5 bit sets it to single use, not an endless while-loop | 3&2 bit set the devision factor (prescaler) to 64.
}

		
ISR (INT0_vect){
    unsigned int timerValue = ICR1;
	distance = timerValue;
	TCNT1 = 0;
}


void send_pulse(){
	TCCR1B |= (1 << CS10);
	TCCR1A = 0;
	
	PORTD = BIT(1);
	wait_micro(10); // Send a pulse of minimal timer period 10us, this will make the Ultrasonic module to send a burst of data.
	PORTD = BIT_OFF(1);
}

int main(void)
{	
	DDRA = 0xFF;
	DDRD = 0b00000010; // SET D0 AS INPUT (ECHO) AND D1 AS OUTPUT (TRIG)
	
	// INIT Interrupt Hardware
	EICRA |= 0b00000011; // INT0 rising edge
	EIMSK |= 0x01; // Enable INT0

	// enable global interrupt system
	sei();
	
	init_4bits_mode();
	char buffer[20];
	
	//TCCR1A = 0b00001100; 
	TCCR1A = 0;
	TCCR1B = (1 << CS11);
	

	while (1){
		send_pulse();
		wait(1000);	
		lcd_command(0x01);
		sprintf(buffer, "%d", distance);
		display_text(buffer);
		
	}
	
	//setupADC();
	
	
	//lcd_command(0x01);
	//display_text("Nelus");
	
    //while (1) 
    //{
		//ADCSRA |= (1<<ADSC);
		////
		////while (ADCSRA & (1<<ADSC));
		//
		//PORTF = BIT(1);
		//wait(10);
		//PORTF = BIT_OFF(0);
		
		
		//
		
		//set_cursor(40);
		//sprintf(buffer, "%d", (ADCH));
		//display_text(buffer);
		//PORTB = ADCH;
		//
		//set_cursor(0);
		//
		//
		//wait(1000);
		

    //}
}

