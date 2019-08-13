/*
 * FND4digit.h
 *
 * Created: 2019-04-05 오후 2:52:22
 *  Author: user
 */ 


#ifndef FND4DIGIT_H_
#define FND4DIGIT_H_

#define FND_COM_DDR DDRD
#define FND_COM_PORT PORTD
#define FND_DATA_DDR DDRC
#define FND_DATA_PORT PORTC

#define FND_e 3
#define FND_d 2
#define FND_p 1
#define FND_c 0
#define FND_g 7
#define FND_a 6
#define FND_f 5
#define FND_b 4

#define digit4 7
#define digit3 6
#define digit2 5
#define digit1 4

#define RClk PORTC4
#define SRClk PORTC5
#define SER PORTC3

void FND4digit_main();
void FND4digit_init(void);
void FND4digit_test(void);
void FND_update_value(int number);
void FND_update_time(int msec, char sec);
void FND_clock(char sec, char min);
void FND4digit_init_shiftR(void);
void FND_shift_out(char data);
void FND4digit_shiftR_test(void);

#endif /* FND4DIGIT_H_ */