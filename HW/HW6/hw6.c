/*
	Section: 2
	Side: B
	Date: 5/30/19

	Assignment description:
		(for example, track button presses longer that 10s in one minute)

	File name: hw6.c
	Description: Basic template for Homework 6
*/

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>
#include <i2c.h>


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);		//Port Initialization
void Timer_Init(void);     	//Initialize Timer 0 
void Interrupt_Init(void); 	//Initialize interrupts
void Timer0_ISR(void) __interrupt 1;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

int counts;				//overflow tracking variable
int time_counts;		//count for time
int button_count;		//count for button
int one_second;
int debounce_count;
int ones_counter;
unsigned int value;
unsigned char input;	//input variable
// add additional global variables if needed

 __sbit __at 0xA2 PB;	// Pushbutton on Port 2 pin 2



//***************
void main(void)
{
	Sys_Init();      // System Initialization
	putchar(' ');    // the quote fonts may not copy correctly into SiLabs IDE
//	Port_Init();
//	Interrupt_Init();
//	Timer_Init();    // Initialize Timer 0 

	printf("Start \n\r");

	
 	
    while (1) /* the following loop contains the button pressing/tracking code */
    {
		value = kpd_input(0);
		
	/*	printf("Enter a keyboard character to begin \r\n");
		input = getchar();
		printf("Push the button as many times as you like in 20 seconds \r\n");
		// add necessary code
		
		button_count = 0;	//clear counts
		//time_count = 0;
		counts = 0;			// timer counts
		TR0 = 1;			// start Timer 0
		
		one_second = 2700;
			//
		time_counts = 0;
		ones_counter = 0;


		while (counts < 54000) //20 seconds
		{
			
			if (!PB)		//if button is pressed
			{
					// time counts set to measure the count of button pressed		
				
				debounce_count = 0;	//debounce count cleared

				//while (debounce_count < 54); //wait
				

				if (time_counts > one_second)
				{
					ones_counter++;
					printf("Time Counts: %i \n\r", time_counts);
					time_counts = 0;
				}

				while (PB);
				
				debounce_count = 0;

				button_count++;		//incrememnt the debounce count

				printf("Score: %i \n\r", button_count);

				while (debounce_count <54); //wait
				

					 

			}	
				



		}
		
		//end of while loop with 20 seconds
		printf("The number of presses is %i \n\r", button_count);
		printf("Number of presses longer than 1s is %i \n\r", ones_counter); 
		TR0 = 0;
		*/

    } 
}

//***************

/*void Port_Init(void)
{
	P2MDOUT &= 0xFD;
	P2 |= 0x04;
}

//***************

void Interrupt_Init(void)
{
	IE |= 0x02;      // enable Timer0 Interrupt request
	EA = 1;       // enable global interrupts
}

//***************
void Timer_Init(void)
{
	
	CKCON |= 0x08;  // CKCON as needed
	TMOD &= 0xFC;   // TMOD as needed
//	TMOD |= _____;   // TMOD as needed
	TR0 = 0;         // Stop Timer0
	TL0 = 0;         // Clear low byte of register T0
	TH0 = 0;         // Clear high byte of register T0

}


//***************
void Timer0_ISR(void) __interrupt 1
{
	// add interrupt code here, in this homework, the code will increment the 
	// global variable 'counts'

	counts++;
	time_counts++;
	debounce_count++;

}

