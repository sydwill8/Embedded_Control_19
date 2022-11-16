/* Sample code for Lab 3.1. This code provides a basic start. */
#include <c8051_SDCC.h>
#include <stdio.h>
#include <stdlib.h>
#include <i2c.h>
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void Port_Init(void);
void PCA_Init (void);
void Interrupt_Init(void);
void XBR0_Init();
void Steering_Servo(void);
void PCA_ISR ( void ) __interrupt 9;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
unsigned int PW_CENTER = 2765;	//Neutral PW limit
unsigned int PW_MIN = 1659;		//Left PW limit
unsigned int PW_MAX = 3871;		//Right PW limit
unsigned int Servo_PW;				//Initial PW
unsigned int count;


//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)
{
    // initialize board
    Sys_Init();
    putchar(' '); //the quotes in this line may not format correctly
    Port_Init();
    XBR0_Init();
    PCA_Init();
    Interrupt_Init();

    //print beginning message
    printf("Embedded Control Pulsewidth Calibration\n");
    // set the PCA output to a neutral setting and wait 1 second
	Servo_PW = PW_CENTER;
	PCA0CP0 = 0xFFFF - PW_CENTER;
	while(count < 2); //wait 1 second
    
    //__________________________________________
    //__________________________________________
   while(1)
        Steering_Servo();
}

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Set up ports for input and output
//
void Port_Init()
{
    P1MDOUT |= 0x0D;  //set output pin for CEX0 or CEX2 in push-pull mode
}

//-----------------------------------------------------------------------------
// XBR0_Init
//-----------------------------------------------------------------------------
//
// Set up the crossbar
//
void XBR0_Init()
{
    XBR0 = 0x27;  //configure crossbar as directed in the laboratory

}

//-----------------------------------------------------------------------------
// PCA_Init
//-----------------------------------------------------------------------------
//
// Set up Programmable Counter Array
//
void PCA_Init(void)
{
 	  PCA0MD = 0x81;	//Enable CF and SYSCLK/12
	  PCA0CPM0 = 0xC2;	//Enabled CCM0
	  PCA0CN = 0x40;	//Enable PCA counter

	 // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}

//-----------------------------------------------------------------------------
// Interrupt_Init
//-----------------------------------------------------------------------------
//
// Set up the PCA overflow interrupts
//
void Interrupt_Init()
{
    EA = 1;			//Enable global interrupts
	EIE1 |= 0x08;	//Enable PCA interrupt

	//Enable interrupts and PCA interrupts specifically
}

//-----------------------------------------------------------------------------
// PCA_ISR
//-----------------------------------------------------------------------------
//
// Interrupt Service Routine for Programmable Counter Array Overflow Interrupt
//
void PCA_ISR ( void ) __interrupt 9
{

	if(CF)
	{
		count++;
		CF = 0;			//Clears flag overflow
		PCA0 = 28672;	//Start count for 20 ms period

	}

	PCA0CN &= 0x40;		//Keeps timer on when on and off when off

    // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}

void Steering_Servo()
{
    char input;
    //wait for a key to be pressed
    input = getchar();
    if(input == 'r')		// single character input to turn car right
    {

       
        if(Servo_PW < PW_MAX)		// check if greater than pulsewidth maximum
        {
			Servo_PW = Servo_PW + 10;	//increases the pulsewidth by 10

		}
		else if(Servo_PW >= PW_MAX)
		{
			Servo_PW = PW_MAX;			//Prevents PW from going over Max PW

		}
    }
    else if(input == 'l')				// single character input to turn car left
    {
		
        // ___________________________
        if(Servo_PW > PW_MIN)			//check if less than pulsewidth minimum
        {
			Servo_PW = Servo_PW - 10;	//decreases the steering pulsewidth by 10
 
		}
		else if((Servo_PW <= PW_MIN))
		{
			Servo_PW = PW_MIN;			//Prevents PW from going over Min PW

		}
    }
    printf("Servo_PW: %u\n", Servo_PW);
    PCA0CPL0 = 0xFFFF - Servo_PW;
	PCA0CPH0 = (0xFFFF - Servo_PW) >> 8; 

}
