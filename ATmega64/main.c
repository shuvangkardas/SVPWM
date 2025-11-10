/*
 * SVM.c
 *
 * Created: 5/1/2018 8:38:19 PM
 * Author : Shuvangkar Chandra Das (shuvangkarcdas@gmail.com)
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
//defines
#define F_CPU           8000000UL
#define F_timer		    1000000ul
#define F_ref		    10
#define Mod_Index		0.9
#define sqrt3			1.7320
#define sqrt2			1.4142
#define	PI				3.1415
#define	Ts				0.000833
#define point			24

// look up variable

volatile unsigned char n_sample=0;
float sin_forward[]= {0,0.258,0.5,0.707,0,0.258,0.5,0.707,0,0.258,0.5,0.707,0,0.258,0.5,0.707,0,0.258,0.5,0.707,0,0.258,0.5,0.707};
float sin_reverse[]= {0.866,0.707,0.5,0.258,0.866,0.707,0.5,0.258,0.866,0.707,0.5,0.258,0.866,0.707,0.5,0.258,0.866,0.707,0.5,0.258,0.866,0.707,0.5,0.258};



//function prototype

//void set_up_timer(int ICR_value);
//void find_sector_and_update_PWM_ontime(unsigned char sample_num,float a_Ts);

int main(void)
{
	DDRB |= (1<<PINB0)|(1<<PB1);
	//PORTB =(1<<PINB0);
	int sampling_frequency= F_ref*point;
	int ICR_value=(int)(0.5*(F_timer/(sampling_frequency)));//multiply by 0.5 cause phase correct pwm,top value =0.5*total sample time
	int a_Ts=(int)ICR_value*Mod_Index ;
	set_up_timer(ICR_value);
	
    /*  application code */
    while (1) 
    {
		
		find_sector_and_update_PWM_ontime(n_sample,a_Ts);
		//PORTB ^=(1<<PINB0);
    }
}
void set_up_timer(int ICR_value)
{
	DDRB |= (1<<PINB0)|(1<<PINB5)|(1<<PINB6)|(1<<PINB7);			// OC1A & OC1B,OC1C - PWM1 & PWM2 &PWM3
	TCCR1A |=(1<<COM1A1)|(1<<COM1A0)|(1<<COM1B1)|(1<<COM1B0)|(1<<COM1C1)|(1<<COM1C0)|(1<<WGM11);//mode 10 phase correct pwm,3 output oc1a,oc1b,oc1c
	TCCR1B |=(1<<WGM13)|(1<<CS11);//clk/8
	TIMSK |=(1<<TOIE1);
	ICR1 =ICR_value;
	OCR1A=0;
	OCR1B=0;
	OCR1C=0;
	sei();
	
}
void  find_sector_and_update_PWM_ontime(unsigned char sample_num,int a_Ts)
{
	PORTB |= (1<<PB1)|(1<<PB0);
	uint8_t   SECTOR = (int)(sample_num /4);//24 samples is 6 sectors,so 4 samples per sector
	uint16_t Tr = (int)(a_Ts*sin_reverse[sample_num]);
	uint16_t Tl = (int)(a_Ts*sin_forward[sample_num]);
	uint16_t T0 = ICR1-Tr-Tl;
	uint16_t Tr_Plus_Tl=Tr+Tl;
	uint16_t half_T0= 0.5*T0;
	PORTB &=~(1<<PB0);
	
	
	switch(SECTOR)
	{
		case 0:
		OCR1A=(Tr_Plus_Tl +half_T0);
		OCR1B=(Tl+half_T0);
		OCR1C=(half_T0);
		break;

		case 1:
		OCR1A=(Tr +half_T0);
		OCR1B=(Tr_Plus_Tl+half_T0);
		OCR1C=(half_T0);
		break;
		
		case 2:
		OCR1A=(half_T0);
		OCR1B=(Tr_Plus_Tl+half_T0);
		OCR1C=(Tl+half_T0);
		break;
		
		case 3:
		OCR1A=(half_T0);
		OCR1B=(Tr+half_T0);
		OCR1C=(Tr_Plus_Tl+half_T0);
		break;

		case 4:
		OCR1A=(Tl +half_T0);
		OCR1B=(half_T0);
		OCR1C=(Tr_Plus_Tl+half_T0);
		break;

		case 5:
		OCR1A=(Tr_Plus_Tl +half_T0);
		OCR1B=(half_T0);
		OCR1C=(Tr+half_T0);
		break;

		default: break;
	}

	PORTB &=~(1<<PB1);
	
	
}
ISR(TIMER1_OVF_vect)
{
	n_sample++;
	
	if(n_sample==24)
	{
		n_sample=0;
	}
	//PORTB ^=(1<<PORTB0);
}
