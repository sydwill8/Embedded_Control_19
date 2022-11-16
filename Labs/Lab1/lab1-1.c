/* 
    Section:2
    Date: 5/23/19
    File name: Lab1-1 Part2
    Program description:This is the part 2 for the group member assignment in Lab 1-1.
*/
/*
  This program is incomplete. Part of the code is provided as an example. You 
  need to modify the code, adding code to satisfy the stated requirements. Blank 
  lines have also been provided at some locations, indicating an incomplete line.
*/
#include <c8051_SDCC.h> // include files. This file is available online on LMS
#include <stdio.h>

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);  // Initialize ports for input and output
void Set_outputs(void);// function to set output bits

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
__sbit __at 0xB6 LED0; // LED0, associated with Port 3 Pin 6
__sbit __at 0xB3 BI1; // BILED0, associated with Port 3, Pin 3
__sbit __at 0xB4 BI2; // BILED1, associated with Port 3, Pin 4
__sbit __at 0xB7 BUZ; // Buzzer, associated with Port 3, Pin 7
__sbit __at 0xA0 SS;   // Slide switch, associated with Port 2, Pin 0
__sbit __at 0xB0 PB1;  // Push button 1, associated with Port 3, Pin 0
__sbit __at 0xB1 PB2; // Push button 2, associated with Port 3, Pin 1


//***************
// Main program

void main(void)
{
    Sys_Init();        // System Initialization
    putchar(' ');      // the quote fonts may not copy correctly into SiLabs IDE
    Port_Init();       // Initialize ports 2 and 3 

    while (1)          // infinite loop 
    {
        // main program manages the function calls

        Set_outputs();
    }
}


//***************
/* Port_Init - Initializes Ports 2 and 3 in the desired modes for input and output */

void Port_Init(void)
{
    // Port 3
P3MDOUT |= 0xD8; // set Port 3 output pins to push-pull mode (fill in the blank)
P3MDOUT &= 0xFC; // set Port 3 input pins to open drain mode (fill in the blank)
P3 |= 0x03; // set Port 3 input pins to high impedance state (fill in the blank)

    // Port 2
    // configure Port 2 as needed
P2MDOUT &= 0xFE;
P2 |= 0x01;
//
}



//***************
// Set outputs:
//    The following code is incomplete, lighting an LED depending 
//    on the state of the slide switch.



void Set_outputs(void)
{
    if (SS)        // if Slide Switch  not activated (Off position)
    {
        BI1 = 0;  // turn on BI LED1 to REd 
		BI2 = 1;  // BI LED2 (RED) is off
		BUZ = 1; // Buzzer is off
		LED0 = 1;   // LED is off
        printf("\r Slide switch is off    \n");
    }
    else            // if Slide Switch is not activated (Off position)
    {
		if (PB1 && PB2) // if pushbuttons are inactivated (not pushed)
		{
			BI1 = 1; // BILED1 is off
			BUZ = 1;
			LED0 = 1;
			printf("\r No buttons are pushed  \n");
		}

		else if (!PB1) // if pushbutton 1 is pushed (activated)
		{
			if (!PB2)
			{
				BI2 = 0; // turn on BILED2 Green
				BI1 = 1;
				BUZ = 1; // turn off Buzzer
				LED0 = 1;
				printf("\r Both pushbuttons are pushed  \n");
			}
			else
			{

				BUZ = 0; // Buzzer is turned on
				BI1 = 1;
				BI2 = 1;
				LED0 = 1; 
				printf("\r Pushbutton 1 is pushed  \n");
			}

		}
		else if (!PB2) //if pushbutton 2 is pushed (activated)
		{
			
			LED0 = 0;   // turn on LED0 
			BI1 = 1;
			BI2 = 1;
			BUZ = 1;
        	printf("\r Pushbutton 2 is pushed   \n");

		}
		//else if (!(PB1 && PB2))// if both push buttons are pushed (activated)
		//{
			//BI2 = 0; // turn on BILED2
			//printf("\r Both pushbuttons are pushed  \n");
		//}	
    }
}

