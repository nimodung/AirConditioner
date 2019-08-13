/*
 * Timer.c
 *
 * Created: 2019-04-10 오전 11:04:02
 *  Author: user
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h> // interrupt 사용하기 위함
//#include "Dot_matrix.h"
#include "FND4digit.h"
#include "Timer.h"
//#include "DHT11.h"
//#include "Uart.h"

//Dot_matrix
/*
extern char dotmatrix_row[8]; //Dot_matrix.c에서 선언한 dotmatrix_row 배열 변수에 접근할 수 있게된다.
extern const char KIM[8];
extern const char HEE[8];
extern const char RAM[8];
extern char name[3][8];
*/
//FND4digit
extern char FND4digit_digit[4];
extern char FND[4];

extern signed char desire_temp;

volatile char ADC_flag, DESIRE_flag, temper_print_flag;		//전역 변수는 초기화 안하면 0으로 초기화된다
volatile int msec = 0, desire_msec = 0;				
volatile char i = 0, sec = 0, min = 0, desire_count;							//volatile : 워킹레지스터에 선언하지말고 RAM에다가 할당하라는 의미(휘발성)
																			//전역변수 선언시에 많이 사용

//isr은 가급적 빨리 실행해야 되기 때문에 딜레이 x, 함수호출x ...
ISR(TIMER0_COMPA_vect)						//interrupt 걸리면 실행하는 내용 //Interrupt Service Routine
{	
						
	//static int msec = 0;					//static -> 처음 할당될때 메모리 받아서 반납하지 않음
	//static char i = 0, sec = 0, min = 0;
	//if(start_flag) 
	
	msec++;
	if(DESIRE_flag) desire_msec++;
	
	if(msec >= 1000)
	{
		ADC_flag = 1;
		msec = 0;
		sec++; //fnd 출력이 바뀌는 시점
		if(sec >= 60)
		{
			sec = 0;
			
			min++;
			if(min >= 60) min = 0;
		}
	
		//if(w_flag)
		
		//FND_clock(sec, min);
	}
	
		
	if(DESIRE_flag)
	{
		//temper_print_flag = 1;
		//FND_out(desire_temp);
		if(desire_msec >= 2000) {
			temper_print_flag = 0;
			DESIRE_flag = 0;
			desire_msec = 0;
		}
	}	
		
	
	
/*	
	if(!(msec % 10)) 
	{
		if(w_flag)
			time_flag = 1;
	}
	
	if(clear_flag)
	{
		clear_flag = 0;
		FND_update_time(msec, sec);
		msec = 0;
		sec = 0;
		min = 0;
	}
*/
	//FND를 실제로 출력시키는 
	i++;
 	
 	if(i >=4) i = 0;
	
	//끄고 데이터 주고 끄고
	FND_COM_PORT &= 0b00001111; //상위 4비트 끄고
	FND_shift_out(FND[i]);
	FND_COM_PORT |= FND4digit_digit[i]; //< FND_shift_out보다 먼저하면 이전 데이터가 출력돼서 정확한 출력이 나오지 않음
	//FND_DATA_PORT = FND[i];
	
	

	//dot_matrix 출력	
	/* dot_matrix 출력
		dot_matrix_comm_low_PORT &= 0b11110000;
		dot_matrix_comm_high_PORT &= 0b11110000;
		dot_matrix_comm_high_PORT |= 1 << (i-4);
		dot_matrix_data_PORT = dotmatrix_row[i];
	*/
	
	//if(!(msec % 500))						//아두이노 우노 상의 led 출력 //인터럽트 잘 걸리는지 확인용
	//	PORTB ^= (1<<PORTB5);				// (1<<PORTB5) == 0b00100000 //토글 켜져있으면 끄고 꺼져있으면 켠다
} 

int Timer_main(void)
{
	
	while(1)
	{ 
	}
	
	return 0;
}



void Timer0_init(void)
{
	//타이머 초기화
	TCCR0A |= 1 << WGM01;					//0b00000010;  //CTC Mode
	TCCR0B |= (1 << CS00 | 1 << CS01);		//0b00000011;  //64분주 사용(하위 3bit : 011)
	OCR0A = 249;							//비교할 값 => (16,000,000 / 64) / 1000 = 250 (0 ~ "249")			//1ms 주기
	TIMSK0 |= 1 << OCIE0A;                  //0b00000010; //어떤 interrupt 걸리게 할건지 정하는	// OC0A 비교 매치 인터럽트 활성화
	
	return;
}

void Timer0_HC_SR04_init(void)
{
	TCCR0A |= 1 << WGM01;					//CTC 모드
	TCCR0B |= (1 << CS02) | (1 << CS00);	//1024분주
	OCR0A = 255;							//
	
	return;
}

void Timer0_init_CTC_outA(void) //A 출력을 쓰기위한 초기화 함수
{
	DDRD |= 1 << PORTD6;
	TCCR0A |= 1<< WGM01 | 1 <<COM0A0;
	TCCR0B |= 1 <<  CS00 | 1 <<CS01;
	
	OCR0A = 30; //4000Hz
	//OCR0A = 63; // 2000Hz
	//OCR0A = 124;							//(250 / 2) - 1 //한 주기를 1msec
	//OCR0A = 249;							//한 주기 2msec //500Hz
	//250Hz를 만들고싶다면? => OCR0A를 늘리면 오버플로우(8bit) => 분주를 늘린다
	//CS0x (분주)랑 OCR값을 이용해서 주기, 주파수 조정
	TIMSK0 |= 1 << OCIE0A;                  //0b00000010; //어떤 interrupt 걸리게 할건지 정하는	// OC0A 비교 매치 인터럽트 활성화
	
	return;
}

void Timer1_init_CTC_outA(void) //A출력을 쓰기위한 초기화 함수
{
	DDRB |= 1 << PORTB1;
	TCCR1A |= 1 << COM1A0; //TOGGLE
	TCCR1B |= 1 << WGM12 | 1 <<  CS11;  //CTC 모드//64분주
	OCR1A = 0; // 소리 끄기
	
	//OCR0A = 30; //4000Hz
	//OCR0A = 63; // 2000Hz
	//OCR0A = 124;							//(250 / 2) - 1 //한 주기를 1msec
	//OCR0A = 249;							//한 주기 2msec //500Hz
	//250Hz를 만들고싶다면? => OCR0A를 늘리면 오버플로우(8bit) => 분주를 늘린다
	//CS0x (분주)랑 OCR값을 이용해서 주기, 주파수 조정
	//TIMSK0 |= 1 << OCIE0A;                  //0b00000010; //어떤 interrupt 걸리게 할건지 정하는	// OC0A 비교 매치 인터럽트 활성화
	
	return;
}

void Timer1_init_fast_PWM_outA(void){
	DDRB |= 1 << PORTB1 | 1<< PORTB2; //outA, 출력
	//MODE : 14, WGM13 : 1, WGM12 : 1, WGM11 : 1, WGM10 : 0 
	// Fast PWM //TOP : ICR1, Update of OCR1x at: BOTTOM, TOV1 Flag Set on : TOP 
	TCCR1A |= (1 << WGM11) | (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0); //COM1A1 : 1, COM1A0 : 0  //OCR값이 커지면 펄스폭이 길어지고
											//Clear OC1A/OC1B on Compare Match, set Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at BOTTOM (non-inverting mode)
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);  //CS12 :0, CS11: 1, CS10: 0 //clkI/O/8 (From prescaler) //8분주
	OCR1A = 0; //BOTTOM
	OCR1B = 0;
	ICR1 = 255; //1차이가 크지 않기때문에 보통 9999대신 10000을 쓴다//19999; //0부터 2만개를 COUNT 하겠다는 의미 
}

void Timer2_init_fast_PWM_outA(void){
	DDRD |= 1 << PORTD3; 
	
	TCCR2A |= (1 << COM2A1) | (1 << COM2A0) | (1 << WGM21) | (1 << WGM20); 
	TCCR2B |= (1 << CS20); //모터 구동 시에는 CS20  
	
	OCR2A = 0; //BOTTOM
	OCR2B = 0;
	//따로 ICR 없음 //0~255까지가 100프로
}

void Timer1_init_fast_PWM_outA_SurvoMotor(void){
	DDRB |= 1 << PORTB1 | 1<< PORTB2; 
	TCCR1A |= (1 << WGM11) | (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0);
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS12);  
	
	OCR1A = 0; 
	OCR1B = 0;
	ICR1 = 255;
}

void Timer2_init_fast_PWM_outA_Motor(void){
	DDRD |= 1 << DDD3; //DDD <- 레지스터의 비트 이름 //PORTDx랑 define 값이 같음
	
	TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
	 //COM2B1 , COM2B0 : 11 이면 Set OC2B on Compare Match, clear OC2B at BOTTOM, (inverting mode). 
	 //->match가 되면 1로 되는것. ocr값을 작게 주면 high가 길다. 반대라서 헷갈리니까 10으로 준것
	TCCR2B |= (1 << CS20); //모터 구동 시에는 CS20
	
	//OCR2A = 0; //BOTTOM
	OCR2B = 130;
	//따로 ICR 없음 //0~255까지가 100프로
}

