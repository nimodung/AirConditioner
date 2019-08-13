/*
 * Keypad.c
 *
 * Created: 2019-04-16 오후 12:06:32
 *  Author: Kim Hee Ram
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdio.h>

/*#include "Uart.h"*/
#include "Keypad.h"

int Keypad_main(void)
{
	/*char long_key_flag = 0;
	int number = 0;
	Keypad_init();
	//UART0_init(9600);*/
	
	while(1)
	{
		/*//number++;
		//TX0_char('\r');
		//TX0_4Digit(number);
		
		if(long_key_flag) 
		{
			if(Keyscan() != 'A') 
			{
				if(Keyscan() != 'B') TX0_char(Keyscan());
				if(Keyscan() == 'B') TX0_string("Hello World\n");
				long_key_flag = 0;
			}
		}
		else 
		{
			if(Keyscan() == 'A') //스위치를 안누를 때 -> 연산 결과가 16 //pinb = 00010000
			{
				long_key_flag = 1;
			}
		}
	*/
	}
	return 0;
}

void Keypad_init(void)
{
	Keypad_PORT_DDR |= 0b11110000; //포트D 상위 4bit 출력으로 설정
	Keypad_PORT |= 0b11110000; // 상위 4bit 끄고 시작(풀업저항)
	Keypad_PIN_DDR &= 0b11110000; //포트B 하위 4bit 입력으로 설정
	Keypad_PIN_PORT |= 0b00001111;
	//MCUCR Register : bit 4번 PUD : Pull-up Disable //내부적으로 풀업저항 사용 가능 //initial value : 0
	//DDRxn : 0 (입력 설정), PORTxn : 1, PUD : 0 => pull-up yes
	
	return;
}

void User_Keypad_init(void)
{
	
	Keypad_PIN_DDR &= 0b11110000; //포트B 하위 4bit 입력으로 설정
	Keypad_PIN_PORT |= 0b00001111;
	
	
	return;
}

char Keyscan_sub(void)
{
	if(!(Keypad_PIN & 0b00000001)) return 1; //키가 눌렸을때
	else if(!(Keypad_PIN & 0b00000010)) return 2;
	else if(!(Keypad_PIN & 0b00000100)) return 3;
	else if(!(Keypad_PIN & 0b00001000)) return 4;
	else return 0;
}

char Keyscan(void)
{
	for(int i = 0; i < 4; i++)
	{
		Keypad_PORT |= 0b11110000;
		Keypad_PORT &= ~(0b10000000 >> i); //clear &=
		_delay_ms(1);
		if(Keyscan_sub()) return 'A' + Keyscan_sub() + (i*4);
	}
	
	return 'A'; //아무것도 눌리지 않았음
}

