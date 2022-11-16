/*
	Section: 2
	Side: B
	Date: 5/23/19

	Port bits/Digital I/O: 
		(for example: 1.0,1.1)

	File name: hw4.c
	Description: Basic template for Homework 4
		This program uses two digital inputs
*/

//-----------------------------------------------------------------------------
// Header Files
//-----------------------------------------------------------------------------

#include <c8051_SDCC.h>// include files. This file is available online
#include <stdio.h>


//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);	// digial port initialization 
void Check_Inputs(void);	// output after first character press

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
//
// edit sbit commands as appropriate to your assignment

__sbit __at 0xB0 PB1; // button 1 with Port 3, Pin 0
__sbit __at 0xB1 PB2; // button 2 with Port 3, Pin 1


//***************
void main(void)  		// 
{
	Sys_Init();      	// System Initialization
	putchar(' ');    
	Port_Init();		// port initialization function
	while(1)
	{
		printf("Press a key to check which buttons are pushed \r\n");
		getchar();
		Check_Inputs();
	}
}

//***************
// configure P2MDOUT or P3MDOUT


//
void Port_Init(void)
{ 

P3MDOUT &= 0xFC; 
P3 |= 0x01;

	//edit this function as appropriate to your assignment
}

//***************
// edit the arguments to the if/else if statements, as needed
// use sbit labels to output digital signals after first character press
//
void Check_Inputs(void)
{
	if (!PB1 && !PB2)			// both buttons pushed
	{
		printf("Both buttons pushed \r\n");
	}
	else if (!PB1 || !PB2)		// one button pushed
	{
		printf("Only one button pushed \r\n");
	}
	else if (PB1 && PB2)		// no buttons pushed
	{
		printf("No buttons pushed \r\n");
	}

}
