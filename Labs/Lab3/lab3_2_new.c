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
unsigned int ReadRanger();
unsigned int ReadCompass();
unsigned int ReadRanger_Light();
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
unsigned int h_count;
unsigned int r_count;
unsigned int new_range;
unsigned int range;
unsigned int new_heading;
unsigned int heading;
unsigned int l_count;
unsigned int light;
unsigned int new_light;




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

   while(1)
	{
		if(new_range)			//enough overflows for a new range
		{
			range = ReadRanger();
			printf("Range: %i\n\r", range);
			new_range = 0;

		}
		else if(new_heading)		//enough overflows for a new heading
	 	{
			heading = ReadCompass();
			printf("Heading: %i\n\r", heading);
			new_heading = 0;

		}
		else if(new_light)
		{
			light = ReadRanger_Light();
			printf("Light: %i\n\r", light);
			new_light = 0;

		}

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
		r_count++;
		h_count++;
		l_count++;
		if(r_count >= 4)
		{

			new_range = 1;	//4 overflows is about 80 ms
			r_count = 0;	//Start count for 20 ms period
		}
		if(h_count >= 2)
		{
			new_heading = 1; //2 overflows is about 40 ms
			h_count = 0;

		}
		if(l_count > 4)
		{
			new_light = 1; //at least 4 overflows is about 80ms
			l_count = 0;

		}


	}

	//PCA0CN &= 0x40;		//Keeps timer on when on and off when off

    // reference to the sample code in Example 4.5 -Pulse Width Modulation 
    // implemented using the PCA (Programmable Counter Array), p. 50 in Lab Manual.
}




//Initilization of SMB

void SMB_Init(void)
{
	SMB0CR = 0x93;	//set SCL to 100KHz
	ENSMB = 1;		//bit 6 of SMB0CN, enable the SMBus

}




//Following function reads the data for the ultrasonic ranger
unsigned int ReadRanger()
{
	unsigned char Data[2];
	unsigned int range = 0;
	unsigned char addr = 0xE0;			//the address of the ranger is 0xE0
	i2c_read_data(addr, 2, Data,2);		//read two bytes, starting at register 6
	range = (((unsigned int)Data[0] << 8) | Data[1]);
	Data[0] = 0x50;						//data zero is in inches
	i2c_write_data(addr,0, Data, 1);	//write one byte of data to reg	
	return range;

}

//Function for the Compass
unsigned int ReadCompass()
{
	unsigned char addr = 0xC0 ;	//the address of the sensor
	unsigned char Data[2];		//Data is an array with a length of 3
	unsigned int heading;		//the heading returned in degrees between 0 and 3599
	i2c_read_data(addr,2,Data,2);	//read two byte, starting at register at 4
	heading = (((unsigned int)Data[0] << 8) | Data[1]); //Combine the two values

	return heading;

}

//Function for the Light
unsigned int ReadRanger_Light()
{
	unsigned char Data[1];
	unsigned int light = 0;
	unsigned char addr = 0xE0;		//address of the ranger
	i2c_read_data(addr,1,Data,1);	//read one byte, starting at register 1
	light = Data[0];

	//Start Ping Data
	Data[0] = 0x51;					//write 0x51 to register 0 of the ranger
	i2c_write_data(addr,0,Data,1);	//write one byte of data to register 0 at address


	return light;

}






