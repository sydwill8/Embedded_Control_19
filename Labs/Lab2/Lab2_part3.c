/*	
*/

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);
void ADC_Init(void);
void Timer_Init(void);
void Interrupt_Init(void); 
void Timer0_ISR(void) __interrupt 1;

unsigned char read_AD_input(unsigned char pin_number);
void mode_select(void);
//void delay_count(unsigned char PB);

void mode_1(void);
void mode_2(void);
void mode_3(void);



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

__sbit __at 0xA0 BI1;
__sbit __at 0xA1 BI2;
__sbit __at 0xA2 LED_top;
__sbit __at 0xA3 LED_bottom;
__sbit __at 0xA4 LED_left;
__sbit __at 0xA5 LED_right;

__sbit __at 0x90 PB_top;
__sbit __at 0x91 PB_bottom;
__sbit __at 0x92 PB_right;
__sbit __at 0x93 PB_left;
__sbit __at 0x94 SS2;
__sbit __at 0x95 SS1;
__sbit __at 0x96 AIn_Pot;

int Counts;

//***************
void main(void)
{
	Sys_Init();      // System Initialization
	putchar(' ');    // the quote fonts may not copy correctly into SiLabs IDE
	Port_Init(); 
	ADC_Init();
	printf("Start \r\n");
	Timer_Init();
	Interrupt_Init();
//	while(1)
		mode_select();
    
}

//
// add Port initialization code
//
void Port_Init(void)
{
	P1MDIN &= ~0x40;
	P1MDIN |= ~0x2F;
	P1MDOUT &= ~0x7F;
	P1 |= 0x7F;
	
	P2MDOUT|= 0x2F;
}
//
// add the initialization code needed for the ADC1
//
void ADC_Init(void)
{
	REF0CN = 0x03;
	ADC1CN = 0x80;
	ADC1CF |= 0x01;
}
//
// 
//
void Timer_Init(void)
{
	CKCON |= 0x08;
	TMOD &= 0xF0;
	TMOD|= 0x01;
	TR0 = 0;
	TMR0 = 0;

}
//
// 
//
void Interrupt_Init(void)
{
	IE |= 0x02;
	EA =1;
}
//
// 
//
void Timer0_ISR(void) __interrupt 1
{
	TF0 =0;
	Counts++;
}
//
// function that completes an A/D conversion
//
unsigned char read_AD_input(unsigned char pin_number)
{
	AMX1SL = pin_number;
	ADC1CN &= ~0x20;
	ADC1CN |= 0x10;
	while((ADC1CN & 0x20) == 0x00); 
	return ADC1;
}
//
//
//
void mode_select(void)
{
	if(!SS1)
	{
		if(!SS2)
		{	
			printf("mode 3\r\n");
			mode_3();
			
		}
		else 
		{
			printf("mode 2\r\n");
			mode_2();
		}
	}
	else
	{	
		printf("mode 1\r\n");
		mode_1();
	}
	
}
//
void mode_1(void)
{

}

void mode_2(void)
{

}

void mode_3(void)
{	
	//declare local variable
	unsigned char AD;
	unsigned char d_count;
	unsigned char P1_score;
	unsigned char P2_score;

	int i;
	int rad;
	// turn all the output off
	BI1 =1;
	BI2 =1;
	LED_top =1;
	LED_bottom =1;
	LED_left =1;
	LED_right =1;
	//
	P1_score =0;
	P2_score =0;
	i =0;
//	printf("mode 3 start \r\n");
	while(i<5)	// loop 5 times
	{
	//	printf("i: %u \r\n",i);
		AD=read_AD_input(6);
	//	printf ("AD read %u \r\n",AD)	;
		d_count = AD+84; 
	//	printf("d_c %u \r\n", d_count);
		rad = (rand()%4); // generate random number between 0 and 3
	//	printf ("rad %u \r\n",rad);
		
		
		
		switch (rad) // check the random number, select the start led
		{
			case 0:
			//	printf("switch0 \r\n");
				TR0=1;
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_top ==0 && !PB_top)
						goto CCW0;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
					P1_score++;
					if(LED_right ==0 && !PB_right)
						goto CCW0;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
					P1_score++;		
					if(LED_bottom ==0 && !PB_bottom)
						goto CCW0;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
					P1_score++;
					if(LED_left ==0 && !PB_left)
						goto CCW0;
				}
				CCW0:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_top ==0 && !PB_top)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
						P1_score++;
						if(LED_right ==0 && !PB_right)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
						P1_score++;		
						if(LED_bottom ==0 && !PB_bottom)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
						P1_score++;
						if(LED_left ==0 && !PB_left)
							goto end;
					}
				goto end;
				end:
					i++;
					break;
		    case 1:
			//	printf("switch1 \r\n");
				TR0=1;
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_right ==0 && !PB_right)
						goto CCW1;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
					P1_score++;
					if(LED_bottom ==0 && !PB_bottom)
						goto CCW1;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
					P1_score++;		
					if(LED_left ==0 && !PB_left)
						goto CCW1;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
					P1_score++;
					if(LED_top ==0 && !PB_top)
						goto CCW1;
				}
				CCW1:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_right ==0 && !PB_right)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
						P1_score++;
						if(LED_top ==0 && !PB_top)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
						P1_score++;		
						if(LED_left ==0 && !PB_left)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
						P1_score++;
						if(LED_bottom ==0 && !PB_bottom)
							goto end;
					}
				goto end;
			case 2:
			//	printf("switch2 \r\n");
				TR0=1;

				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_bottom ==0 && !PB_bottom)
						goto CCW2;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
					P1_score++;
					if(LED_left ==0 && !PB_left)
						goto CCW2;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
					P1_score++;		
					if(LED_top ==0 && !PB_top)
						goto CCW2;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
					P1_score++;
					if(LED_right ==0 && !PB_right)
						goto CCW2;
				}
				CCW2:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_bottom ==0 && !PB_bottom)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
						P1_score++;
						if(LED_right ==0 && !PB_right)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
						P1_score++;		
						if(LED_top ==0 && !PB_top)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
						P1_score++;
						if(LED_left ==0 && !PB_left)
							goto end;
					}
				goto end;
			case 3:
				//printf("switch3 \r\n");
				TR0=1;

				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_left ==0 && !PB_left)
						goto CCW3;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
					P1_score++;
					if(LED_top ==0 && !PB_top)
						goto CCW3;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
					P1_score++;		
					if(LED_right ==0 && !PB_right)
						goto CCW3;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
					P1_score++;
					if(LED_bottom ==0 && !PB_bottom)
						goto CCW3;
				}
				CCW3:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =1; LED_left =0;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_left ==0 && !PB_left)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bottom =0; LED_left =1;
						P1_score++;
						if(LED_bottom ==0 && !PB_bottom)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bottom =1; LED_left =1;
						P1_score++;		
						if(LED_right ==0 && !PB_right)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bottom =1; LED_left =1;
						P1_score++;
						if(LED_top ==0 && !PB_top)
							goto end;
					}
				goto end;
		}
	}
	if (P1_score > P2_score) // compare score, player with lease score wins.
	{
		printf("Player 1 scoer: %u, Player 2 scoer: %u, Player 2 win!",P1_score, P2_score);
		BI1=1;
		BI2=0;	
	}
	else if (P2_score > P1_score)
	{
		printf("Player 1 scoer: %u, Player 2 scoer: %u, Player 1 win!",P1_score, P2_score);
		BI1=0;
		BI2=1;	
	}

}
//
//
//
