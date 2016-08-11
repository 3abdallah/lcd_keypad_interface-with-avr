/*
 * start2.c
 *
 * Created: 7/12/2016 5:05:18 PM
 * Author : abdalla
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include<util/delay.h>
#include<avr\interrupt.h>
#include<stdlib.h>
#define key_PRT PORTD //keypad PORTD
#define key_DDR DDRD // keypad data direct register
#define key_PIN PIND // keypad reading input signal
#define lcd_DPRT PORTA //data port 
#define lcd_DDDR DDRA  //data direct register 
#define lcd_CPRT PORTB  //data control port port
#define lcd_CDDR DDRB   //data control direct register 
#define lcd_RS   0//pin for rs
#define lcd_RW   1//pin for rw
#define lcd_E    2//pin for enable pin
int sec=0,min=0,hou=0;//global variables 
char co[16];//array used to convert numbers into strings 
unsigned char keypad [4][4] = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};//the structure array to initialize the keypad to choose which character pressed
/*to send a command you should do three things 
one :you should make the rs pin (register select)to be zero
two :you should make the rw pin (read/write register)to be zero
three:you should enable the E pin to latch information and disable it again to make it latch the informatin
*/
void lcd_command(unsigned char cmd)// a routine used to send commands to the lcd
{
	lcd_DPRT=(cmd  & 0xF0);
	lcd_CPRT&=~(1<<lcd_RS);
	lcd_CPRT&=~(1<<lcd_RW);
	lcd_CPRT|=(1<<lcd_E);
	_delay_us(1);
	lcd_CPRT&=~(1<<lcd_E);
	_delay_us(100);
	lcd_DPRT=(cmd<<4);
	lcd_CPRT|=(1<<lcd_E);
	_delay_us(1);
	lcd_CPRT&=~(1<<lcd_E);
	_delay_us(100);
}
/*to send a data you should do three things
one :you should make the rs pin (register select)to be one
two :you should make the rw pin (read/write register)to be zero
three:you should enable the E pin to latch information and disable it again to make it latch the informatin
*/
void lcd_data(unsigned char data)//a routine used to send data to be written at the lcd
{
	lcd_DPRT=(data & 0xF0);
	lcd_CPRT|=(1<<lcd_RS);
	lcd_CPRT&=~(1<<lcd_RW);
	lcd_CPRT|=(1<<lcd_E);
	_delay_us(1);
	lcd_CPRT&=~(1<<lcd_E);
	_delay_us(100);
	lcd_DPRT=(data<<4);
	lcd_CPRT|=(1<<lcd_E);
	_delay_us(1);
	lcd_CPRT&=~(1<<lcd_E);
	_delay_us(100);
	
	
}
void lcd_init()// a routine to initialize the lcd with a 4 bit mode
{
	lcd_DDDR=0xFF;//data port is output
	lcd_CDDR=0xFF;//control port is output
	lcd_CPRT&=~(1<<lcd_E);//disable the E pin
	lcd_command(0x33);//a command for 4 bit mode
	lcd_command(0x32);// a command for 4 bit mode
	lcd_command(0x28);// 5*7 matrix for 4 bit mode
	_delay_us(2000);
	lcd_command(0x0E);//lcd on cursor on 
	lcd_command(0x01);//clear the lcd
	_delay_us(2000);
	lcd_command(0x06);//shift the cursor to the right
}
void gotoxy(unsigned char x,unsigned char y )// a routine to move around the lcd
{
	unsigned char address[]={0x80,0xC0,0x94,0xD4};
		lcd_command(address[y-1]+(x-1));
		_delay_us(100);
}
void lcd_print( char * str)//this a routine to send a display a string on lcd..
{
	unsigned i =0;
	while(str[i]!=0)
	{
		lcd_data(str[i]);
		i++;
	}
}
void conv(int a)//this routine is used to display numbers on lcd..
{
	itoa(a,co,10);
	lcd_print(co);
}
void lcd_blink()//this routine is used to blink th entire lcd..
{
	lcd_command(0x08);
	_delay_ms(300);
	lcd_command(0x0C);
	_delay_ms(300);
}
int main(void)
{
	uint8_t colloc=0,rowloc=0,count=0;//this variables colloc=column location ..rowloc=rowlocation..count=for clear the second line of the lcd
	key_DDR=0xF0;//make the rows as input and columns as output..
	key_PRT=0x0F;//enable the pull up resistor for rows an grounded all columns
	lcd_init();
	gotoxy(1,1);
	lcd_print("press a key:");
    while (1) 
    {
		do //aloop to ensure that all buttons realesed
		{
			key_PRT&=0x0F;
			rowloc=(key_PIN & 0x0F);
		} while (rowloc!=0x0F);
		do // a loop to check any pressed button
		{
			do 
			{
				_delay_ms(50);//a short delay for the debouncing of the keys
				rowloc=(key_PIN&0x0F);
			} while (rowloc==0x0F);
			_delay_ms(50);
			rowloc=(key_PIN & 0x0F);
		} while (rowloc==0x0F);
		while (1)// aloop to detect which column the key pressed located in by ground evry column one by one and chec the input reading..
		{
			key_PRT=0xEF;
			_delay_us(10);
			rowloc=(key_PIN & 0x0F);
			if (rowloc!=0x0F)
			{
				colloc=0;
				count++;
				break;
			}
			key_PRT=0xDF;
			_delay_us(10);
			rowloc=(key_PIN&0x0F);
			if (rowloc!=0x0F)
			{
				colloc=1;
				count++;
				break;
			}
			key_PRT=0xBF;
			_delay_us(10);
			rowloc=(key_PIN&0x0F);
			if (rowloc!=0x0F)
			{
				colloc=2;
				count++;
				break;
			}
			key_PRT=0x7F;
			_delay_us(10);
			rowloc=(key_PIN&0x0F);
				colloc=3;
				count++;
				break;
		}
		if (count>16)// if condition to clear the second line of the lcd when its full in
		{
			lcd_command(0x01);
			_delay_us(2000);
			gotoxy(1,1);
			lcd_print("print a key:");
			count=1;
		}
		if (rowloc==0x0E)// to check if the key pressed in the key in row 1
		{
			gotoxy(count,2);
			lcd_data(keypad[0][colloc]);
		}
		else if (rowloc==0x0D)// to check if the key pressed in the key in row 2
		{
			gotoxy(count,2);
			lcd_data(keypad[1][colloc]);
		}
		else if (rowloc==0x0B)// to check if the key pressed in the key in row 3
		{
			gotoxy(count,2);
			lcd_data(keypad[2][colloc]);
		}
		else// to check if the key pressed in the key in row 4
		{
			gotoxy(count,2);
			lcd_data(keypad[3][colloc]);
		}
		
	}
}
