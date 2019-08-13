/*
 * Keypad.h
 *
 * Created: 2019-04-17 오전 11:05:04
 *  Author: user
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_


#define Keypad_PORT_DDR DDRD
#define Keypad_PORT PORTD
#define Keypad_PIN_DDR DDRB
#define Keypad_PIN_PORT PORTB
#define Keypad_PIN PINB

void Keypad_init(void);
void User_Keypad_init(void); //에어컨 키패드 초기화
char Keyscan(void);
char Keyscan_sub(void);


#endif /* KEYPAD_H_ */