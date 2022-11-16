/* 
Homework: 10
Section: 2
Side: B
*/
	

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
void PCA_ISR ( void ) __interrupt 9;
void SMB0_Init(void);
unsigned int ReadRanger(void);
signed int error_height(void);

void Drive_motor(unsigned int range);
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
unsigned int r_count;
unsigned int new_range;
unsigned int Counts;
unsigned int l_count;
unsigned int light;
unsigned int new_light;
unsigned int PW_CENTER = 2700;
unsigned int PW_MIN = 2002;
unsigned int PW_MAX = 3458;
unsigned int PW;
unsigned int actual_height;
unsigned int neutral_height = 45;
signed int error;
__sbit __at 0xB6 SS;


//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)
{	
    // initialize board

    Sys_Init();
	putchar(' ');
    Port_Init();
    XBR0_Init();
    PCA_Init();
	SMB0_Init();
    Interrupt_Init();
	

	Counts = 0;
	PW = PW_CENTER;
	PCA0CP2 = 0xFFFF - PW_CENTER;
	while(Counts < 20);
	
    while(1)
	{
		if(new_range ==1 )			//enough overflows for a new range
		{
			actual_height = ReadRanger();
			new_range = 0;
		}
	
		error = error_height();
		if (!SS)
		{
			Drive_motor(actual_height);
			printf("Actual Height: %u, PW: %u\r\n", actual_height, PW);
			
		}
		else
		{
			PCA0CP2 = 0xFFFF - PW_CENTER;
		}
	//	;
	}
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
	P3MDOUT &= 0x1F;
	P3 |= 0xE0;
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
	  PCA0CPM0 = PCA0CPM2 = PCA0CPM3 = 0xC2;	//Enabled CCM0
	  PCA0CN = 0x40;	//Enable PCA counter

	 // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}`

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
		Counts++;
		r_count++;
		if(r_count >= 4)
		{

			new_range = 1;	//4 overflows is about 80 ms
			r_count = 0;	//Start count for 20 ms period
		}
		CF=0;
		PCA0=28672;

	}

	PCA0CN &= 0x40;		//Keeps timer on when on and off when off

    // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}




//Initilization of SMB

void SMB0_Init()
{
	SMB0CR = 0x93;	//set SCL to 100KHz
	ENSMB = 1;		//bit 6 of SMB0CN, enable the SMBus

}

//Following function reads the data for the ultrasonic ranger
unsigned int ReadRanger(void)
{
	unsigned char Data[2];
	unsigned int range = 0;
	unsigned char addr = 0xE0;			//the address of the ranger is 0xE0
	i2c_read_data(addr, 2, Data,2);		//read two bytes, starting at register 2
	range = (((unsigned int)Data[0] << 8) | Data[1]);
	Data[0] = 0x51;						//data zero is in inches
	i2c_write_data(addr,0, Data, 1);	//write one byte of data to reg	
	return range;

}
signed int error_height(void)
{
	error =0;
	error = neutral_height-actual_height;
	return error;

}
// motor control 
void Drive_motor(unsigned int range)
{

    if(range <= 10)  // single character input to increase the pulsewidth
    {
		PW = PW_MAX;
    }
    else if(range > 40 && range < 50)  // single character input to decrease the pulsewidth
    {
		PW = PW_CENTER;     // set PW to the minimum value
    }
	else if(range >80)
	{
		PW = PW_MIN;
	}
	else
	{
		PW = 21* error +2680;
	}
    PCA0CPL2 = 0xFFFF - PW;
	PCA0CPH2 =(0xFFFF - PW) >>8;
}
