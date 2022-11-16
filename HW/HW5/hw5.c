/*
	Section: 2
	Side: B
	Date: 5/28/19

	Size of counter: 13bit  (indicated your assigned counter size, delete the others)

	Trigger: SYSCLK/12   (indicate your assigned trigger, delete the other)

	File name: hw5.c
	Description: Basic template for Homework 5
		This program waits 3 seconds after a keyboard input and then prints the
  		number of overflows that occurred.
*/

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Timer_Init(void);     // Initialize Timer 0 
void Interrupt_Init(void); //Initialize interrupts
void Timer0_ISR(void) __interrupt 1;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

unsigned char input;
int counts = 0;    	//replace xxxxx with the variable type, uncomment as well


//***************
void main(void)
{
	Sys_Init();      // System Initialization
	putchar(' ');    
	Interrupt_Init();
	Timer_Init();    // Initialize Timer 0 

    while (1) /* the following loop waits until a key is pressed on the
			     keyboard and prints the number of overflows that occurr
				 in two time intervals */
    {
		printf("Hit any key on the keyboard to start \r\n");
		input = getchar();

		counts = 0;
		// add code to clear the high and low byte of Timer0 and start Timer0
			
		TMR0 = 0;
		TR0 = 1;

	
		// add code for a ???? second delay
		while((counts<674));
		
		printf("\n\r3 seconds have passed. ");  	//print that the time has elapsed

		printf("Number of overflows = %i \n\r", counts);	// print number of overflows using count variable
	
		
		// add code for a ???? second delay
		while(counts<1124);

		printf("5 seconds have passed. ");  	//print that the time has elapsed

		printf("Number of overflows = %i \n\r", counts);	// print number of overflows using count variable
		

		TR0 = 0;
		
    }
}

//***************

void Interrupt_Init(void)
{
	ET0 = 1 ;      // enable Timer0 Interrupt request using sbit variable 
	EA = 1 ;       // enable global interrupts using bit masking
}
//***************
void Timer_Init(void)
{
	
	CKCON &= 0xFC;  // Timer0 uses SYSCLK or SYSCLK/12
	TMOD &= 0xF0;   // clear the 4 least significant bits
	TMOD |= 0x00;   // Timer0 mode setting
	TR0 = 0;         // Stop Timer0
	TL0 = 0x00;         // Clear low byte of register T0
	TH0 = 0x00;         // Clear high byte of register T0
}


//***************
void Timer0_ISR(void) __interrupt 1
{
	// add interrupt code here, in this homework, the code will increment the 
	// global variable 'counts'
	counts++;
}

