/*
	Section: 2
	Side: B
	Date: 5/30/19

	Gain: 0.5
	Port pin: Input 1.6

	File name: hw7.c
	Description: 

*/

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void ADC_Init(void);
void Port_Init(void);
unsigned char read_AD_input(unsigned char pin_number);



//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

unsigned char AD_value;
unsigned char input;
int result;

//***************
void main(void)
{
	
	
	Sys_Init();      // System Initialization
	putchar(' ');    // the quote fonts may not copy correctly into SiLabs IDE
	Port_Init(); 
	ADC_Init();
	printf("Start \r\n");
    while (1) 
    {
		printf("enter key to read A/D input \r\n");
		input = getchar();

		// add code necessary to complete the homework

		AD_value = read_AD_input(0x06); //Read the A/D value on P1.6


		result= (AD_value/(0.5*255))*(REF0CN*1000);


		

		printf("The A/D value is %i \n\r", AD_value);	// print statement as required by homework
		printf("The input voltage is %i \n\r", result  );	// print statement as required by homework

    }
}


//
// add the initialization code needed for the ADC1
//
void ADC_Init(void)
{
	REF0CN = 0x03; //Sets internal reference at 2.4V
	 
	ADC1CN = 0x80; //Enable A/D converter

	ADC1CF &= ~0x03; //Set gain to 0.5


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
	P1MDIN &= ~0x40; //P1.6 is set to an analog input
	P1MDOUT &= ~0x40; //P1.6 set to open drain
	P1 |= 0x40; //Set high impendance to P1.6 



}

