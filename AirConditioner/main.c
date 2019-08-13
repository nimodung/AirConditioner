/*
 * AirConditoner.c
 *
 * Created: 2019-05-15 오후 4:39:42
 * Author : Kim Hee Ram
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#include "ADC.h"
#include "FND4digit.h"
#include "Keypad.h"
#include "Timer.h"

#define COOLING 1
#define HEATING 2

#define MODE 1
#define UP 2
#define DOWN 3
#define WIND 4

#define MAX_TEMP 30
#define MIN_TEMP 17

#define CONTROL_DDR DDRB
#define CONTROL_PORT PORTB
#define COOLER 5 //PORTD 5 //냉방기 ON BOARD
#define HEATER 4 //PORTD4 //난방기 RED LED

void FND_out(signed char temp); //fnd 출력 함수
void Contoler_init(void);
void Air_Vol(void);
void Heater_On(void);
void Heater_Off(void); 
void Cooler_On(void);
void Cooler_Off(void);

extern char FND[4];
extern char FND4digit_font[10];

extern volatile char ADC_flag, DESIRE_flag, temper_print_flag;

signed char room_temp = 0, desire_temp = 20; //현재 온도, 희망 온도 
char mode = COOLING; //1 : 냉방, 2 : 난방

int main(void)
{
	char long_key_flag = 0;
	char wind_speed = 1;
	
	//초기화
	Contoler_init();
	Timer0_init();
	Timer2_init_fast_PWM_outA_Motor();
	ADC_init();
	FND4digit_init_shiftR();
	User_Keypad_init();
   
	sei();
	
	//room_temp = Volt_to_temperature(ADC_converting_value(0));
	FND_out(room_temp);
	
	while (1) 
    {		//가변저항 : 현재 온도 측정
		/*
		roomTemp = Volt_to_temperature(ADC_converting_value(0));
		
		_delay_ms(1000);
		*/
		
		//키 입력
		if(ADC_flag) {
			ADC_flag = 0;
			
			if(DESIRE_flag) {
				FND_out(desire_temp); 	
			}
			else 
			{
				room_temp = Volt_to_temperature(ADC_converting_value(0)); //온도 측정 //1초에 한번씩
				FND_out(room_temp);
			}
		}
		
		if(mode == COOLING) {
			Heater_Off(); //COOLING MODE 에서는 HEATER 꺼주기
			if(desire_temp < room_temp) Cooler_On(); //희망 온도가 현재 온도보다 높으면 COOLING MODE 가동
			else Cooler_Off();
		}
		else //HEATING MODE
		{
			Cooler_Off(); //HEATING MODE 에서는 COOLER 꺼주기
			if(desire_temp > room_temp) Heater_On();
			else Heater_Off();
		}
		
		if(long_key_flag)
		{
			if(Keyscan_sub()) 
			{
				_delay_us(200); 
				switch(Keyscan_sub()) { // chattering 잡기
					case MODE :
						if(mode == COOLING) mode = HEATING;
						else mode = COOLING;
						
						if(DESIRE_flag) FND_out(desire_temp);
						else FND_out(room_temp);
						
						long_key_flag = 0;
						break;
					case UP :
						
						desire_temp++;
						if(desire_temp > MAX_TEMP) desire_temp = MAX_TEMP;
						DESIRE_flag = 1;
						FND_out(desire_temp);
						long_key_flag = 0;
						break;
					case DOWN :
						desire_temp--;
						if(desire_temp < MIN_TEMP) desire_temp = MIN_TEMP;
						DESIRE_flag = 1;
						FND_out(desire_temp);
						long_key_flag = 0;
						break;
					case WIND :
						Air_Vol();
						/*
						wind_speed++;
						if(wind_speed > 4) wind_speed = 0;
						if(wind_speed != 4) FND[2] = FND4digit_font[wind_speed]; 
						switch(wind_speed) {
							case 0 :
								OCR2B = 0;
								break;
							case 1 :
								OCR2B = 170;
								break;
							case 2 :
								OCR2B = 210;
								break;
							case 3 :
								OCR2B = 250;
								break;
						}
						*/
						long_key_flag = 0;
						break;
					
				}
				
				//FND_out(room_temp);
			}
		}
		else
		{
			if(!Keyscan_sub()) //0이 return 되면 키 입력이 없다
			{
				long_key_flag = 1;
			}
		}
		
    }
}

void FND_out(signed char temp){
	
	if(mode == COOLING) {
		if(temp >= 0) {
			FND_update_value(temp);
			FND[2] = 255; 
			FND[3] = FND4digit_font[COOLING];
		}
		else{
			//temp = temp * -1; //양수 값으로 변경
			FND_update_value(temp * -1);
			FND[2] = ~(1 << FND_g); 
			FND[3] = FND4digit_font[COOLING];
		}
		
	}
	else {
		if(temp >= 0) {
			FND_update_value(temp);
			FND[2] = 255;
			FND[3] = FND4digit_font[HEATING];
		}
		else{
			//temp = temp * -1; //양수 값으로 변경
			FND_update_value(temp * -1);
			FND[2] = ~(1 << FND_g);
			FND[3] = FND4digit_font[HEATING];
		}
	}
	
	return;
}

 void Air_Vol(void){
	 
	 if(OCR2B == 0) {
		 OCR2B = 130; //꺼져있는 상태면 미풍으로
		 FND[2] = FND4digit_font[1]; 
	 }
	 else if(OCR2B == 130) 
	 {
		 OCR2B = 170; //약풍
		 FND[2] = FND4digit_font[2];
	 }
	 else if(OCR2B == 170) 
	 {
		 OCR2B = 240; //강풍
		 FND[2] = FND4digit_font[3]; 
	 }
	 else if(OCR2B == 240) 
	 {
		 OCR2B = 0; //끄기
		 FND[2] = FND4digit_font[0]; 
	 }
	
 }
 
 void Contoler_init(void) {
	 CONTROL_DDR |= 1 << COOLER | 1 << HEATER; //PORT 4, 5를 출력으로 설정
	 CONTROL_PORT &= ~(1 << COOLER | 1 << HEATER); //LED 끄기
	 return;
 }
 
void Heater_On(void) {
	 CONTROL_PORT |= 1 << HEATER;
	 return;
}
 
 void Heater_Off(void) {
	  CONTROL_PORT &= ~(1 << HEATER);
	  return;
}

void Cooler_On(void) {
	CONTROL_PORT |= 1 << COOLER;
	return;
}

void Cooler_Off(void) {
	CONTROL_PORT &= ~(1 << COOLER);
	return;
}