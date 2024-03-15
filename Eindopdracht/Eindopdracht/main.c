
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

#define LCD_E 3
#define LCD_RS 2
#define BIT(x) (1 << (x))

void lcd_strobe_lcd_e(void);
void init_4bits_mode(void);
void set_cursor(int position);

void wait(int ms) {
	for (int i = 0; i < ms; i++) {
		_delay_ms(1);
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
	DDRF = 0x00;
	DDRB = 0xFF;
	DDRG = 0xFF;
	
	ADMUX = 0b11100001;
	
	ADCSRA = 0b10000110;
}

int main(void)
{
	DDRB = 0x01;
	//init_4bits_mode();
	//setupADC();
	
	//char buffer[20];
	
	PORTB = 0xFF;
	
    while (1) 
    {
		//ADCSRA |= (1<<ADSC);
		//
		//while (ADCSRA & (1<<ADSC));
		//
		//lcd_command(0x01);
		//display_text("Temperatuur:");
		//set_cursor(40);
		//sprintf(buffer, "%d", (ADCH));
		//display_text(buffer);
		//PORTB = ADCH;
		//
		//set_cursor(0);
		//
		//
		//wait(1000);
		

    }
}

