/*
	Section: 2
	Side: B
	Date: 5/30/19

	File name: lab2.c
	Description: 

*/

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void ADC_Init(void);
void Port_Init(void);
void Timer_Init(void);
void Interrupt_Init(void);
void Timer0_ISR(void) __interrupt 1;
void mode_select(void);
void mode_1(void);
void mode_2(void);
void mode_3(void);
void Button_Pushed(void);
unsigned char read_AD_input(unsigned char pin_number);



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

//Port 2
__sbit __at 0xA0 BI1;		//BILED 1 at Port 2, Pin 0
__sbit __at 0xA1 BI2;		//BILED 2 at Port 2, Pin 1
__sbit __at 0xA2 LED_top;	//Top LED at Port 2, Pin 2
__sbit __at 0xA3 LED_bot;	//Bottom LED at Port 2, Pin 3
__sbit __at 0xA4 LED_left;	//Left LED at Port 2, Pin 4
__sbit __at 0xA5 LED_right;	//Right LED at Port 2, Pin 5

//Port 1
__sbit __at 0x90 PB_top;	//Top Pushbutton at Port 1, Pin 0
__sbit __at 0x91 PB_bot;	//Bottom Pushbutton at Port 1, Pin 1
__sbit __at 0x93 PB_left;	//Left Pushbutton at Port 1, Pin 2
__sbit __at 0x92 PB_right;	//Right Pushbutton at Port 1, Pin 3
__sbit __at 0x94 SS1;		//Slide Switch 1 at Port 1, Pin 4
__sbit __at 0x95 SS2;		//Slide Switch 2 at Port 1, Pin 5
__sbit __at 0x96 Aln_Pot;	//Potentiometer at Port 1, Pin 6

unsigned char AD_value;
unsigned char input;
int result;
int Counts;
int mode;
int sequence;
int analog_count;
int analog_overflow;
int debounce_count;
int LED_left_count;
int LED_right_count;

//***************
void main(void)
{
	
	
	Sys_Init();      // System Initialization
	putchar(' ');    // the quote fonts may not copy correctly into SiLabs IDE
	Port_Init(); 
	ADC_Init();
	Interrupt_Init();
	Timer_Init();	//Initialize Timer 0
	printf("Start \r\n");
    while (1) 
    {

	//Mode select for three modes

	TR0 = 1; //Turn on Timer
	mode_select();
	TR0 = 0;

	/*	printf("enter key to read A/D input \r\n");
		input = getchar();

		// add code necessary to complete the homework

		AD_value = read_AD_input(0x06); //Read the A/D value on P1.6


		result= (AD_value/(0.5*255))*(REF0CN*1000);


		

		printf("The A/D value is %i \n\r", AD_value);	// print statement as required by homework
		printf("The input voltage is %i \n\r", result  );	// print statement as required by homework */

    }
	//TR0 = 0; //Turn off Timer
}


//
// add the initialization code needed for the ADC1
//
void ADC_Init(void)
{
	REF0CN = 0x03; //Sets internal reference at 2.4V
	 
	ADC1CN = 0x80; //Enable A/D converter

	ADC1CF |= 0x01; //Set gain to 1


}
//
// function that completes an A/D conversion
//
unsigned char read_AD_input(unsigned char pin_number)
{
	AMX1SL = pin_number; //Set P1.6 as the analog input for ADC1
	ADC1CN = ADC1CN & ~0x20; //Clear the "Conversation Completed" flag

	ADC1CN = ADC1CN | 0x10; //Initiate A/D conversion

	while ((ADC1CN & 0x20) == 0x00); //Wait for conversion to complete

	return ADC1;
}

//
// add Port initialization code
//
void Port_Init(void)
{
	P1MDIN &= ~0x40; //
	P1MDIN |= ~0x2F; //Configure anlog input and output port
	P1MDOUT &= ~0x7F; //Set pins to open drain or push-pul
	P1 |= 0x7F; //Set high impendances to the inputs

	P2MDOUT |= 0x2F; //Configure Port 2 inputs and outputs


}

void Interrupt_Init(void)
{
	IE |= 0x02;	//Enable global interrupts using bit masking
	EA = 1; 	//Enable Timer 0 Interrupt request using sbit variable
	

}

void Timer_Init(void)
{
	CKCON |= 0x08;	//Timer 0
	TMOD &= 0xF0;	//
	TMOD |= 0x01;	//Timer0 mode setting
	TR0 = 0;		//Stops Timer
	TMR0 = 0; //Clear high and low byte of Timer 0

}



//Function to select modes
void mode_select(void)
{
	if (!SS1) //Slide Switch 1 is Activated
	{
		if(!SS2) //Slide Switch 2 is Activated
		{
			mode = 3;
			printf("Mode is %i \n\r", mode);
			mode_3(); //Call mode 3 function when SS1 and SS2 is Activated
			 
		}
		else
		{
			mode = 2;
			printf("Mode is %i \n\r", mode);
			mode_2(); //Call mode 2 function when SS2 is not activated

		}
	}
	else
	{
		if(!SS2) //Slide Switch 2 is activated
		{
			mode = 1;
			printf("Mode is %i\n\r", mode);
			mode_1(); //Call mode 1 function when SS2 is Activated
			
			//break

		}

	}

}

//Mode 1 function
void mode_1(void)
{

	
	analog_count = 0; //clear the count of analog input	
	LED_top = 1;	//Top Led Off
	LED_bot = 1;	//Bottom Led Off
	LED_left = 1;	//Left Led Off
	LED_right = 1;	//Right Led Off
	
	BI1 = 1;		//Turn Off BiLed 1
	BI2 = 1;		//Turn Off BiLed 2
	
	sequence = 1;
	
	while(sequence == 1)
	{
		AD_value = read_AD_input(0x06); //Read the A/D value on P1.6
		result= (AD_value/(255))*(REF0CN); //check the input voltage
		
		analog_overflow =(result+0.8)/0.009496; //Calcuate the seconds for the anaalog input voltage		
		//printf("Analog Overflow: %i \n\r", analog_overflow);

		//printf("Sequence: %i \n\r", sequence);

		LED_top = 0;
		analog_count = 0;
		while(analog_count < analog_overflow); //wait
		LED_top = 1;

		LED_right = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_right = 1;

		LED_bot = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_bot = 1;
		
		LED_left = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_left = 1;
		

		LED_top = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_top = 1;


		LED_left = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_left = 1;

		LED_bot = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_bot = 1;
		
		LED_right = 0;
		analog_count = 0; //Clear the analog count
		while(analog_count < analog_overflow); //wait
		LED_right = 1;

		sequence++;
	}	



	while(sequence == 2)
	{
		AD_value = read_AD_input(0x06); //Read the A/D value on P1.6
		result= (AD_value/(255))*(REF0CN); //check the input voltage
		
		analog_overflow =(result+0.8)/0.009496; //Calcuate the seconds for the anaalog input voltage			


		LED_top = 0;
		analog_count = 0;
		while(analog_count < analog_overflow); //wait
		LED_top = 1;

		
		LED_bot = 0;
		analog_count = 0;
		while(analog_count < analog_overflow); //wait
		LED_bot = 1;

		LED_left = 0;
		analog_count = 0;
		while(analog_count < analog_overflow); //wait
		LED_left = 1;

		LED_right = 0;
		analog_count = 0;
		while(analog_count < analog_overflow); //wait
		LED_right = 1;


		LED_left = 0;
		analog_count = 0;
		while(analog_count < analog_overflow); //wait
		LED_left = 1;


		LED_bot = 0;
		while(analog_count < analog_overflow); //wait
		LED_bot = 1;

		LED_top = 0;
		while(analog_count < analog_overflow); //wait
		LED_top = 1;
		
		//printf("Sequence: %i \n\r", sequence);
		sequence++;
		
	}



	while(sequence == 3)
	{
		AD_value = read_AD_input(0x06); //Read the A/D value on P1.6
		result= (AD_value/(255))*(REF0CN); //check the input voltage

		
		analog_overflow =(result+0.8)/0.009496; //Calcuate the seconds for the anaalog input voltage	
		


		LED_left = 0;
		while(analog_count < analog_overflow); //wait
		LED_left = 1;
		

		LED_right = 0;
		while(analog_count < analog_overflow); //wait
		LED_right = 1;
	
		LED_left = 0;
		while(analog_count < analog_overflow); //wait
		LED_left = 1;

		LED_right = 0;
		while(analog_count < analog_overflow); //wait
		LED_right = 1;
		//printf("Sequence: %i \n\r", sequence);
		sequence++;

	}

	

}


//Function for mode 2
void mode_2(void)
{
	int rounds;
	int push_left;
	int push_right;
	rounds = 0; //Beginning number of rounds
	
	
	TR0 = 0;
	LED_left = 0; //Turn on Left LED
	LED_right= 1; //Turn off Right LED
	LED_top = 1; //Turn off top LED
	LED_bot= 1; //Turn off bot LED

	push_left = 0;
	push_right = 0;

	LED_left_count = 0; //Set the count of the left LED to 0
	LED_right_count = 0; //Set the count of the right LED to 0

	Play:

		while(rounds <3)
		{
			AD_value = read_AD_input(0x06); //Read the A/D value on P1.6
			result= (AD_value/(255))*(REF0CN); //check the input voltage
			analog_overflow =(result+0.6)/0.00178; //Calcuate the seconds for the anaalog input voltage	

			//printf("Rounds: %i \n\r", rounds);
			TR0 = 1;
			BI1 = 1;
			BI2 = 1;
		
				while(LED_left_count < analog_overflow)
				{
					LED_right = 1;
					LED_left = 0;
					
					while(debounce_count < 12); //wait
					if (!PB_left)
					{
						
						while(PB_left);
						debounce_count = 0;	
						while(debounce_count < 12); //wait

						push_left++; //Increment the pushes of left pushbutton
						//printf("Left Count: %i \n\r", push_left);
						while(debounce_count < 12);

					}
		
					else if(!PB_right) //right pushbutton is activated
					{	
						push_right = 0;
						

					
						while(PB_right);
						debounce_count = 0;	
						BI1 = 0;
						BI2 = 1;
						while(debounce_count < 12); //wait
						
						printf("Left person wins \n\r");	
						push_left = 0;
						rounds = 0;				

						goto Play;
					}
			
		
				
			
		
			} //Exit when left LED is off

				while(LED_left_count < (2* analog_overflow))
				{
					LED_left = 1;
					LED_right = 0;

					//while(debounce_count < 12); //wait
					if(!PB_right)
					{
					

						while(PB_right);
						
					
						debounce_count = 0;
							
						while(debounce_count < 12); //wait
						
						push_right++; //Increment the pushes of left pushbutton
						//printf("Right Count: %i \n\r", push_right);
						while(debounce_count < 12);
					}
		
					else if(!PB_left) //left pushbutton is activated
					{	
						push_left = 0; //Clear score of left person
					
						BI1 = 1;
						BI2 = 0;
						while(PB_left);
						debounce_count = 0;	
						while(debounce_count < 12); //wait
						
						printf("Right person wins\n\r");
						push_right = 0;
						rounds = 0;
						goto Play;
					}
				}
				TR0 = 0;
				rounds++;
				LED_left_count = 0;
			

		}

		Score:


			//Prints the winner of mode 2 with most amounts of pushes
			if(push_left > push_right)
			{
				BI1 = 0;
				BI2 = 1;
				printf("Left person wins \n\r");
				push_left = 0;
				push_right = 0;
			}

			else if(push_right > push_left)
			{ 
				BI1 = 1;
				BI2 = 0;
				printf("Right person wins\n\r");
				push_left = 0;
				push_right = 0;
			}
			else
			{
				printf("Both players win \n\r");
				push_left = 0;
				push_right = 0;

			}
	
		//break

}

//Function for Mode 3
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
	LED_bot =1;
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
		printf ("rad %u \r\n",rad);
		
		
		
		switch (rad) // check the random number, select the start led
		{
			case 0:
			//	printf("switch0 \r\n");
				TR0=1;
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_top ==0 && !PB_top)
						goto CCW0;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
					P1_score++;
					if(LED_right ==0 && !PB_right)
						goto CCW0;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
					P1_score++;		
					if(LED_bot ==0 && !PB_bot)
						goto CCW0;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
					P1_score++;
					if(LED_left ==0 && !PB_left)
						goto CCW0;
				}
				CCW0:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_top ==0 && !PB_top)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
						P1_score++;
						if(LED_right ==0 && !PB_right)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
						P1_score++;		
						if(LED_bot ==0 && !PB_bot)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
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
					LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_right ==0 && !PB_right)
						goto CCW1;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
					P1_score++;
					if(LED_bot ==0 && !PB_bot)
						goto CCW1;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
					P1_score++;		
					if(LED_left ==0 && !PB_left)
						goto CCW1;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
					P1_score++;
					if(LED_top ==0 && !PB_top)
						goto CCW1;
				}
				CCW1:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_right ==0 && !PB_right)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
						P1_score++;
						if(LED_top ==0 && !PB_top)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
						P1_score++;		
						if(LED_left ==0 && !PB_left)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
						P1_score++;
						if(LED_bot ==0 && !PB_bot)
							goto end;
					}
				goto end;
			case 2:
			//	printf("switch2 \r\n");
				TR0=1;

				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_bot ==0 && !PB_bot)
						goto CCW2;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
					P1_score++;
					if(LED_left ==0 && !PB_left)
						goto CCW2;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
					P1_score++;		
					if(LED_top ==0 && !PB_top)
						goto CCW2;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
					P1_score++;
					if(LED_right ==0 && !PB_right)
						goto CCW2;
				}
				CCW2:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_bot ==0 && !PB_bot)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
						P1_score++;
						if(LED_right ==0 && !PB_right)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
						P1_score++;		
						if(LED_top ==0 && !PB_top)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
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
					LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
					P1_score++;
				//	printf("LED_T, count: %u\r\n", Counts);
					if(LED_left ==0 && !PB_left)
						goto CCW3;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
					P1_score++;
					if(LED_top ==0 && !PB_top)
						goto CCW3;
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
					P1_score++;		
					if(LED_right ==0 && !PB_right)
						goto CCW3;		
				}
				Counts =0;
				while(Counts < d_count/4)
				{
					LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
					P1_score++;
					if(LED_bot ==0 && !PB_bot)
						goto CCW3;
				}
				CCW3:
				//	printf("CCW");
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =1; LED_left =0;
						P1_score++;
					//	printf("LED_T, count: %u\r\n", Counts);
						if(LED_left ==0 && !PB_left)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 1; LED_bot =0; LED_left =1;
						P1_score++;
						if(LED_bot ==0 && !PB_bot)
							goto end;
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =1; LED_right = 0; LED_bot =1; LED_left =1;
						P1_score++;		
						if(LED_right ==0 && !PB_right)
							goto end;		
					}
					Counts =0;
					while(Counts < d_count/4)
					{
						LED_top =0; LED_right = 1; LED_bot =1; LED_left =1;
						P1_score++;
						if(LED_top ==0 && !PB_top)
							goto end;
					}
				goto end;
		}
	}
	if (P1_score > P2_score) // compare score, player with lease score wins.
	{
		printf("Player 2 wins!\n\r");
		BI1=1;
		BI2=0;	
	}
	else if (P2_score > P1_score)
	{
		printf("Player 1 wins!\n\r");
		BI1=0;
		BI2=1;	
	}



}






void Timer0_ISR(void)__interrupt 1
{

	Counts++; //Increments the overflow count
	analog_count++; //Increment the count of the analog timer
	//printf("Analog Count: %i \n\r", analog_count);
	debounce_count++; //Increment debounce count for pushbutton
	LED_left_count++; //Increment the count of the left LED
	//printf("LEft Count: %i \n\r", LED_left_count );
	LED_right_count++; //Increment the count of the right LED
	//printf("Right Count: %i \n\r",LED_right_count );
}











