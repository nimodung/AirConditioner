/*
 * ADC.c
 *
 * Created: 2019-04-24 오전 10:01:04
 *  Author: Kim Hee Ram
 */ 
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
//#include "Uart.h"
#include "ADC.h"

#define DATA_AVR_COUNT 10

int ADC_main(void)
{
	int value = 0;
	int distance = 0;
	ADC_init();
	_delay_us(200); //ADC 준비 딜레이
	
	//UART0_init(9600);
	
	while(1)
	{
		//가변 저항
		/*
		value = ADC_converting_value(0);
		//TX0_4Digit(value);
		//TX0_char('\n');
		printf("Potentionmeter : %d.%d%d \t", value/100%10, value/10%10, value%10); 
				
		//CDS
		value = ADC_converting_value(1);
		printf("CDS value : %d.%d%d \n", value/100%10, value/10%10, value%10);
		*/
		
		//적외선 거리 측정
	/*	value = ADC_converting_value(2); //전압에 *100 한 값
		distance = Volt_to_cm(value);
		printf("Distance : %d.%d%d \n", distance/100%10, distance/10%10, distance%10);
		
		_delay_ms(500);*/
		
	}
	
	return 0;
}

void ADC_init(void)
{
	ADMUX |= (1 << REFS0); //Voltage Reference Selection : AVCC with external capacitor at AREF pin
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //ADC enable, prescaler 128분주
	_delay_ms(200);
	return;
}

int ADC_converting_value(char channel)
{
	int return_value = 0;
	static int value = 0;
	static int runningavr_value_arr[10] = {0,};
	static char idx = 0;
	
	ADMUX &= 0b11110000; //Analog Channel Selection Bits 초기화
	ADMUX |= channel; //채널 설정
	_delay_us(200); //제대로 동작하는데 약간의 시간 필요
	
	//신호 처리//가변 저항값 오차 잡기' 
	
	//1. value 값 10개의 평균 값 출력 해주기
	
	/*value = 0;
	for(int i = 0; i < DATA_AVR_COUNT; i++) {
		ADCSRA |= (1 << ADSC); //Start Conversion //When the conversion is complete, it returns to zero
		while(!(ADCSRA  & (1 << ADIF))); //ADIF : conversing 이 완료되면 set //ADIF가 0인동안 기다려주기
	//value = 0;
	//ADC 값 value에 저장하기
		/ * //컴파일러 문제인듯 // 수식에는 문제 없으나 변경되는 값이 출력안됨
			value = ADCH;
			value <<= 8;
			value += ADCL;  //value |= ADCL; 
		* /
		value += ADCL + ADCH * 256;
	}
	value /= DATA_AVR_COUNT;
	
	*/
	//2. 기존 값에 *9 + 측정 값 의 평균 출력  
	//1번에 비해 1번만 값을 얻으면 되고, 1번은 크게 튀는 값이 한개라도 있으면 평균에 영향을 크게 미치는데 비해 2번은 그런게 없당
	//but, 측정값이 더디게 움직임
	//작은 값이면 무시가되어 정확도 낮아짐
	/*ADCSRA |= (1 << ADSC);
	while(!(ADCSRA  & (1 << ADIF)));
	value = (value * 9 + ADC) / 10;
	*/
	
	//3. running avr 
	//값이 지속적으로 변하는 경우 사용
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA  & (1 << ADIF)));
	runningavr_value_arr[idx] = ADCL + ADCH * 256; 
	idx++; 
	if(idx >= 10) idx = 0;
	value = 0;
	for(int i = 0; i < DATA_AVR_COUNT; i++) {
		value += runningavr_value_arr[i];
	}
	value /= DATA_AVR_COUNT;
	
	return_value = (value + 1) * 5 * 100.0 / 1024 + 3; //+3 : 오차 보정 //5 * 100 / 1024 : 0 ~ 5V 사이의 값 , 소숫점을 위해서 *100
	
	return return_value;
}

int Volt_to_cm(int value)  //SHARP 2Y0A21 적외선 거리센서
{
	int cm = 0;
	cm = 20.0 / ((value / 100.0) - 0.3); //데이터 시트 보고 대충 계산한 값 //제품마다 다르기때문에 완전 정확하지않음
	
	return cm;
}

int Volt_to_temperature(int value) 
{
	int temp = value * 11 / 50 - 30;
	return temp;
}