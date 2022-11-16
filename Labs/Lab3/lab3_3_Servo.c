/* 
Lab3_3_Servo
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
unsigned int ReadCompass();
void SMB0_Init();
signed int error_servo();
unsigned in_range();
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

__sbit __at 0xB7 SS2;	//Slide Switch at Port 3 Pin 7

unsigned int h_count, r_count, new_range, range;
unsigned int new_heading;
unsigned int count;
unsigned int desired_heading = 900;
unsigned int PW_CENTER = 2765, PW_MIN = 1659, PW_MAX = 3871;	//Neutral, min, and max PW limit
unsigned int SERVO_PW, actual_heading;
signed int temp_servo_pw;

	



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
	SMB0_Init();

	PCA0CP0 = 0xFFFF - PW_CENTER;
	count = 0;			//clear count
	printf("Start");
   while(1)
	{
		unsigned int heading;

		

		if(!SS2) //If the slide switch is activated
		{
			if(new_heading == 1)		//enough overflows for a new heading
	 		{
				//printf("Heading\n\r");
				heading = ReadCompass();
				actual_heading = heading;
				printf("Actual Heading: %i\n\r", heading);

				if(in_range() == 0)
				{
					printf("Failed");
				}
				else
				{
					error_servo();
					//Check if Pulse width is witin limits
					if(temp_servo_pw >= PW_MIN && temp_servo_pw <=PW_MAX)
					{
						//while(temp_servo_pw != desired_heading)
						//{
							//printf("Turning");

							PCA0CPL0 = 0xFFFF - temp_servo_pw;
							PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
							new_heading = 0;
					//	}

					}
					else if(temp_servo_pw <= PW_MIN)
					{

						temp_servo_pw = PW_MIN;
						PCA0CPL0 = 0xFFFF - temp_servo_pw;
						PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
						new_heading = 0;



					}
					else if(temp_servo_pw >= PW_MAX)
					{

						temp_servo_pw = PW_MAX;
						PCA0CPL0 = 0xFFFF - temp_servo_pw;
						PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
						new_heading = 0;

					}
					

					//Infinte loop for desired heading

				}
			}		
		}
		else
		{
			PCA0CP0 = 0xFFFF - PW_CENTER;	//Set wheels bck to neutral position

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
    P1MDOUT |= 0x0D;	//set output pin for CEX0 or CEX2 in push-pull mode
	P3MDOUT &= 0x1F;	//set inputs to open drain for port 3
	P3 |= 0xE0;			//set inputs to high impedance
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
		count++;
		CF = 0;
		//printf("Rcount: %i \n\r", r_count);
		h_count++;
		//printf("Hcount: %i \n\r", h_count);
		if(h_count >= 2)
		{
			new_heading = 1; //2 overflows is about 40 ms
			h_count = 0;

		}


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


//Function for the Compass
unsigned int ReadCompass()
{
	unsigned char addr = 0xC0 ;	//the address of the sensor
	unsigned char Data[2];		//Data is an array with a length of 3
	unsigned int heading;		//the heading returned in degrees between 0 and 3599

	//printf("Start Ping\n\r");
	i2c_read_data(addr,2,Data,2);	//read two byte, starting at register at 4
	//printf("Read\n\r");
	heading = (((unsigned int)Data[0] << 8) | Data[1]); //Combine the two values
//	printf("Ping\n\r");
	return heading;

}

//Function for the accounted error while steering
signed int error_servo()
{
		signed int error;

		temp_servo_pw = PW_CENTER;
		error = desired_heading - actual_heading;
		printf("error: %i", error);

		if(error >= 1800)
		{
			error = error - 3600;
			temp_servo_pw = ((5 * error)/12) +PW_CENTER;
			printf("temp: %i ", temp_servo_pw);
			return temp_servo_pw;
		}
		else if(error <= -1800)
		{
			error = error + 3600;
			temp_servo_pw = ((5*error)/12) +PW_CENTER;
			printf("temp: %i ", temp_servo_pw);
			return temp_servo_pw;

		}
	
		else //if(error > -1800 && error <0)
		{
			//error = error + 1800;
			temp_servo_pw =((5*error)/12) +PW_CENTER;
			printf("temp: %i ", temp_servo_pw);
			return temp_servo_pw; 

		} 

}

unsigned int in_range()
{
	if(desired_heading > 2700 && desired_heading < 3600)
	{
		if(actual_heading >0 && actual_heading < 900)
		{
			return 0;
		}
		return 1;
	}
	if(actual_heading > 2700 && actual_heading < 3600)
	{
		if(desired_heading >0 && desired_heading < 900)	
		{
			return 0;

		}
		return 1;
	}
	
	else
		return 1;

}






