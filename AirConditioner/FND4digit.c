/*
 * FND4digit.c
 *
 * Created: 2019-04-04 오후 4:01:50
 *  Author: user
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#include "Timer.h"
#include "FND4digit.h"

char FND4digit_font[10] = {
	~(1 << FND_a | 1 << FND_b | 1 << FND_c | 1 << FND_d | 1 << FND_e | 1 << FND_f), //0
	~(1 << FND_c | 1 << FND_b), //1
	~(1 << FND_a | 1 << FND_b | 1 << FND_g | 1 << FND_e | 1 << FND_d), //2
	~(1 << FND_a | 1 << FND_b | 1 << FND_g | 1 << FND_c | 1 << FND_d), //3
	~(1 << FND_c | 1 << FND_b | 1 << FND_g | 1 << FND_f), //4
	~(1 << FND_a | 1 << FND_f | 1 << FND_g | 1 << FND_d | 1 << FND_d | 1 << FND_c), //5
	~(1 << FND_a | 1 << FND_f | 1 << FND_e | 1 << FND_d | 1 << FND_d | 1 << FND_g | 1 << FND_c), //6
	~(1 << FND_a | 1 << FND_b | 1 << FND_c), //7
	~(1 << FND_a | 1 << FND_b | 1 << FND_c | 1 << FND_d | 1 << FND_e | 1 << FND_f | 1 << FND_g), //8
	~(1 << FND_a | 1 << FND_b | 1 << FND_c | 1 << FND_d | 1 << FND_f | 1 << FND_g), //9
};


char FND4digit_digit[4] = {
	1 << digit1, 1 << digit2, 1 << digit3, 1 << digit4
};

char FND[4];

void FND4digit_main()
{
	/*
	unsigned int number = 5000;
	//int k;
	char sec = 0, min = 0; //~255까지 저장 -> msec는 1000까지 증가시킬 것이기때문에 int로 선언 => 변수 타입 중요
	int msec = 0;
	char long_key_flag = 1, start_flag = 0, w_flag = 0;

	FND4digit_init();
	
	FND_COM_DDR &= 0b11001111; //특정비트만 클리어 -> 해당 비트만 0 주고 엔드연산
	FND_update_time(msec, sec);
	*/
	
	/*FND4digit_init_shiftR();
	Timer0_init();*/
	
	//FND_COM_PORT = 0b10000000;
	while(1)
	{
		/*
		//number = PINB; //입력을 읽을때 사용
		//number = PINB & 0b00010000; //4번째 비트만 필요하니까 
		if(long_key_flag) //long_key_flag가 1 일때 if문 실행
		{
			if(!(PINB & 0b00010000)) //스위치를 눌렀을 때 -> 연산 결과가 0일때 //pinb = 00000000 -> pinb & 00010000 = 0 의 논리 반전 => 참
			{   //KEY1  //(PINB & 0b00010000) == 0
				//if(start_flag) start_flag = 0;
				//else start_flag = 1;
				
				start_flag = 1; //if(!start_flag) start_flag = 1;
				
				if(w_flag) w_flag = 0;
				else w_flag = 1;
				
				long_key_flag = 0;
			}
			else if(!(PINB & 0b00100000)) 
			{ //KEY2
				FND_update_time(msec, sec);
				start_flag = 0;
				msec = 0;
				sec = min = 0;
				w_flag = 0;
				long_key_flag = 0;
			}
		}
		else //long_key_flag 가 1일 때
		{
			if((PINB & 0b00010000) && (PINB & 0b00100000)) //스위치를 안누를 때 -> 연산 결과가 16 //pinb = 00010000
			{  
				long_key_flag = 1;
			}
			
		}
		
		//k = number;
		if(start_flag)
		{
			msec += 8;
			if(msec >= 1000)
			{
				msec = 0;
				sec++;
				if(sec >= 60)
				{
					sec = 0;
					min++;
					if(min >= 60)
					{
						min = 0;
					}
				}
			}
			if((w_flag))
			{
				FND_update_time(msec, sec);
			}
		}
		
		//FND_update_value(min*100 +sec);
		//FND_update_value(number);
		//FND4digit_test();
		
		for(int i = 0; i < 4; i++)
		{
			FND_COM_PORT = FND4digit_digit[i];
			FND_DATA_PORT = FND[i];
			//k = k/10;
			_delay_ms(2);
		}*/
		//FND4digit_shiftR_test();
		//FND_shift_out(FND4digit_font[7]);
		//_delay_ms(1000);
		//FND_shift_out(FND4digit_font[1]);
		//_delay_ms(1000);
		
	}
	
	return;
}

void FND4digit_init(void)
{
	FND_COM_DDR |= 0b00001111; //하위 4 비트만 출력으로 설정
	FND_COM_PORT &= 0b11110000; //꺼짐으로 초기화
	FND_DATA_DDR |= 0b11111111; //포트를 출력(1)으로 설정
	FND_DATA_PORT |= 0b11111111; //LED를 전부 끄는걸로 초기화(VCC에 연결했기때문에 1:꺼짐 0:켜짐)
	return;
}

void FND4digit_init_shiftR(void)
{
	FND_COM_DDR |= 0b11110000;  //출력 설정
	FND_COM_PORT &= 0b00001111; //fnd 끄기
	FND_DATA_DDR |= 1 << RClk | 1 << SRClk | 1 << SER; //595 3bit 연결 // 3, 4,5번 연결
	FND_DATA_PORT |= 1 << RClk; // 0b00010000; //RClk : 1 //RClk을 상승 edge로 둬서 다른 데이터가 들어가지 않게
	FND_update_value(0);
	return;
}

void FND_shift_out(char data)
{
	FND_DATA_PORT &= ~(1 << RClk); 
	for(int i = 0; i < 8; i++) //data 한개씩 넣는걸 8번
	{
		FND_DATA_PORT &= ~(1 << SRClk);
		FND_DATA_PORT &= ~(1 << SER); //일단 0으로 떨어트려
		FND_DATA_PORT |= ((data >> i) & 1) << SER; //받아온 data 값을 0 아니면 1
		FND_DATA_PORT |= 1 << SRClk;//0b00100000; //상승 edge
	}
	FND_DATA_PORT |= 1 << RClk; //RClk 상승 edge
	return;  
}

void FND4digit_test(void)
{
	for(int j = 0; j < 4; j++)
	{
		FND_COM_PORT = FND4digit_digit[j];
	
		for(int i = 0; i < 10; i++)
		{
			FND_DATA_PORT = FND4digit_font[i];
			_delay_ms(400);
		}
	}
	return;
}

void FND4digit_shiftR_test(void)
{
	for(int j = 0; j < 4; j++)
	{
		FND_COM_PORT = FND4digit_digit[j];
		
		for(int i = 0; i < 10; i++)
		{
			FND_shift_out(FND4digit_font[i]);
			_delay_ms(400);
		}
	}
	return;
}

void FND_update_value(int number)
{
	FND[0] = FND4digit_font[number % 10];
	FND[1] = FND4digit_font[number /10 % 10];
	FND[2] = FND4digit_font[number /100 % 10] ;
	FND[3] = FND4digit_font[number /1000 % 10];
	return;
}

void FND_update_time(int msec, char sec)
{
	FND[0] = FND4digit_font[msec /10 % 10];
	FND[1] = FND4digit_font[msec /100 % 10];
	FND[2] = FND4digit_font[sec % 10] & (~(1<< FND_p));
	FND[3] = FND4digit_font[sec /10 % 10];
	return;
}

void FND_clock(char sec, char min)
{
	FND[0] = FND4digit_font[sec % 10];
	FND[1] = FND4digit_font[sec /10 % 10];
	FND[2] = FND4digit_font[min % 10] & (~(1<< FND_p));
	FND[3] = FND4digit_font[min /10 % 10];
	return;
}