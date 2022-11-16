/* 
Includes Lab3_3_Servo
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
void ADC_Init(void);
unsigned int ReadCompass();
void SMB0_Init();
signed int error_servo();
unsigned in_range();
unsigned char read_AD_input(unsigned char pin_number);
unsigned int ReadRanger(void);
signed int error_height(void);
void DC_Speed(void);
void Motor_Direction(void);
void user_input(void);
void Servo_speed(void);


//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

__sbit __at 0xB7 SS1;	//Slide Switch at Port 3 Pin 7
__sbit __at 0xB3 BI1;	//BiLED1 at Port 3 Pin 3
__sbit __at 0xB1 BI2;	//BiLED2 at Port 3 Pin 1
__sbit __at 0x96 POT;	//Slide Switch at Port 1 Pin 6





unsigned int h_count, r_count, new_range, range;
unsigned int new_heading;
unsigned int Counts;
char direction;
unsigned int desired_heading, desired_heading_s;
unsigned int PW_CENTER = 2775, PW_MIN = 1859, PW_MAX = 3271;	//Neutral, min, and max PW limit
unsigned int SERVO_PW, actual_heading,	actual_height, neutral_height;
signed int PW_DC;
signed int temp_servo_pw,error;
unsigned char AD_value;	//Conversion value
unsigned char input;
unsigned char gain = 1;




//-----------------------------------------------------------------------------
// Main Function
//-----------------------------------------------------------------------------
void main(void)
{
	int loop ;
	
    // initialize board
    Sys_Init();
    putchar(' '); //the quotes in this line may not format correctly
    Port_Init();
    XBR0_Init();
    PCA_Init();
    Interrupt_Init();
	SMB0_Init();
	ADC_Init();
	DC_Speed();

	PCA0CP0 = 0xFFFF - PW_CENTER;
	PCA0CP2 = 0xFFFF - 2800;
	Counts = 0;			//clear count

	printf("Start");

	loop = 0;			//Condition for the first run of 3 seconds
  	while(1)
	{
		
		
		
		if (SS1) // When the Slide switch is off
		{
			BI1 = 0;
			BI2 = 1;
			PCA0CP0 = 0xFFFF - PW_CENTER;	//Set wheels bck to neutral position
			PW_DC = 2800;
			PCA0CPL2 = 0xFFFF - PW_DC;
			PCA0CPH2 =(0xFFFF - PW_DC) >>8;
			printf("off\n\r");
			user_input();
			loop = 0;
		}
		else
		{
			
			
			while (!SS1)		//Slide Switch is activated
			{
				if( loop == 0)
				{
					
					BI1 = 1;	//Red Off
					BI2 = 0;	//Green On
				
			
					Counts = 0;

					direction = 'f';	//Sets direction to forward
					DC_Speed();

					//Drive Forward for 3 seconds
					Counts = 0;
					while(Counts<170)	//Wait for 3 seconds, accounts for the 20 ms call
					{
					
						DC_Speed();
						desired_heading = 900;
						temp_servo_pw = error_servo();
						Servo_speed();
					}
					loop = 1;
				}
				error_servo();
				Servo_speed();
				
				DC_Speed();
				Counts = 0;
				range = ReadRanger();
				while(range == -1 || range == 2816 )	//Ignore ranger values of -1 or 2816
				{
					range = ReadRanger();
				}


				printf("Range = %i\n\r", range);
				error_servo();
				Servo_speed();
				if (range <= 20)						//Ranger detects an object within 20 cm
				{
					error_servo();
					Servo_speed(); 
					Motor_Direction();					//Adjust Direction
				
					
				}
				range = ReadRanger();
				while(range == -1 || range == 2816 )
				{
					range = ReadRanger();
				}
				error_servo();
				Servo_speed(); 
				
			}
		}
	}
        
}


//****************************Initialization*******************************

//-----------------------------------------------------------------------------
// Port_Init
//-----------------------------------------------------------------------------
//
// Set up ports for input and output
//
void Port_Init()
{
    P1MDOUT |= 0x0D;	//set output pin for CEX0 or CEX2 in push-pull mode
	P1MDIN &= 0xBF;
	P1 |= 0x40;
	P3MDOUT &= 0x1F;	//set inputs to open drain for port 3
	P3MDOUT |= 0x0A;
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
	  PCA0CPM0 = PCA0CPM2 = PCA0CPM3 = 0xC2;	//Enabled CCM0,CCM2, CCM3
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


//Initilization of SMB

void SMB0_Init()
{
	SMB0CR = 0x93;	//set SCL to 100KHz
	ENSMB = 1;		//bit 6 of SMB0CN, enable the SMBus

}


//Initialization code for the ADC1
void ADC_Init(void)
{
	REF0CN = 0x03; //Sets internal reference at 2.4V
	 
	ADC1CN = 0x80; //Enable A/D converter

	ADC1CF |= 0x01; //Set gain to 1


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
		Counts++;			//Incremented the counts
		r_count++;
		
	
		h_count++;
	
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
		CF = 0;
		PCA0=28672;			//Set PCA0

	}

	PCA0CN &= 0x40;		//Keeps timer on when on and off when off

}






//Function that completes an A/D Conversion
unsigned char read_AD_input(unsigned char pin_number)
{
	AMX1SL = pin_number; //Set P1.6 as the analog input for ADC1
	ADC1CN = ADC1CN & ~0x20; //Clear the "Conversation Completed" flag

	ADC1CN = ADC1CN | 0x10; //Initiate A/D conversion

	while ((ADC1CN & 0x20) == 0x00); //Wait for conversion to complete

	return ADC1;
}


//*******************************Compass*****************************

//Function for the Compass
unsigned int ReadCompass()
{
	unsigned char addr = 0xC0 ;	//the address of the sensor
	unsigned char Data[2];		//Data is an array with a length of 3
	unsigned int heading;		//the heading returned in degrees between 0 and 3599

	i2c_read_data(addr,2,Data,2);	//read two byte, starting at register at 4
	
	heading = (((unsigned int)Data[0] << 8) | Data[1]); //Combine the two values
	printf("Current Direction = %i \n\r", heading);
	return heading;

}

//Function for the accounted error while steering
signed int error_servo()
{
		signed int errorS;
		temp_servo_pw = PW_CENTER;
		actual_heading = ReadCompass();
		errorS = desired_heading - actual_heading;	
		printf("Direction error: %i \n\r", errorS);

		//Following statements adjust the calculated PW based on the error
		if(error >= 1800)
		{
			errorS = errorS - 3600;
			temp_servo_pw = ((errorS) * gain) +PW_CENTER;
			printf("Steering PW: %i \n\r", temp_servo_pw);
			return temp_servo_pw;
		}
		else if(errorS <= -1800)
		{
			errorS = errorS + 3600;
			temp_servo_pw = ((errorS*gain)) +PW_CENTER;
			printf("Steering PW: %i\n \r", temp_servo_pw);
			return temp_servo_pw;

		}
	
		else 
		{
			
			temp_servo_pw =((errorS) * gain) +PW_CENTER;
			printf("Steering PW: %i \n\r", temp_servo_pw);
			return temp_servo_pw; 

		} 
}

//Check Headings
unsigned int in_range()
{
	if(desired_heading > 2700 && desired_heading < 3600)
	{
		if(actual_heading >0 && actual_heading < 900)
		{
			printf("1st 0");
			return 0;
		}
		return 1;
	}
	if(actual_heading > 2700 && actual_heading < 3600)
	{
		if(desired_heading >0 && desired_heading < 900)	
		{
			printf("Second 0");
			return 0;
			

		}
		return 1;
	}
	
	else
		return 1;

}




//Adjust Steering
void Servo_speed(void)
{
	unsigned int heading;

			if(new_heading == 1)		//enough overflows for a new heading
	 		{
			
				heading = ReadCompass();	//Read from the compass
				actual_heading = heading;
			
				if(in_range() == 0)
				{
					printf("Failed");
				}
				else
				{
					//Check if Pulse width is witin limits and adjust the steering
					if(temp_servo_pw >= PW_MIN && temp_servo_pw <=PW_MAX)
					{
						
							
							PCA0CPL0 = 0xFFFF - temp_servo_pw;		
							PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
							new_heading = 0;
				

					}
					else if(temp_servo_pw <= PW_MIN)
					{
						
						temp_servo_pw = PW_MIN;
						if(direction == 'r')
							temp_servo_pw = PW_MAX;
						PCA0CPL0 = 0xFFFF - temp_servo_pw;
						PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
						new_heading = 0;



					}
					else if(temp_servo_pw >= PW_MAX)
					{

						temp_servo_pw = PW_MAX;
						if(direction == 'r')
							temp_servo_pw = PW_MIN;
						PCA0CPL0 = 0xFFFF - temp_servo_pw;
						PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
						new_heading = 0;

					}
					

			

				}
			}
			
	

}


//User input
void user_input(void)
{
	
	lcd_clear();
	while(Counts < 50);		//wait for 1 second
	desired_heading = kpd_input(0);		//user input for deseired heading
	desired_heading_s = desired_heading;	//second variable for desired heading
	lcd_clear();			//clear the screen

	printf("Desired Heading: \n %c, = Hex %X \n\r", desired_heading, desired_heading);
	gain = kpd_input(0);		//user input for the gain
	
	printf("Gain: \n %c, = Hex %X \n\r ", gain, gain);
	lcd_clear();

}




//********************************DC Motor***************************

//Ranger Data
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

//Error calculation for the motor
signed int error_height(void)
{
	error =0;
	error = neutral_height-actual_height;
	return error;


}

//Controls the Speed for motor either forward or reverse
void DC_Speed(void)
{
	AD_value = read_AD_input(0x06);
	printf("AD_Value: %i \n\r", AD_value);
	if (direction == 'f')
	{
	//	direction = 'f';
		BI2 =0;
		BI1 =1;
		PW_DC = gain*(-160)/51*AD_value+2800;	//Calculated Pulse width for DC motor
		desired_heading = 900;
		error_servo();							
		Servo_speed();
		actual_heading = ReadCompass();
		if (PW_DC < 2000)
			PW_DC = 2000;
		printf("Driver PW forward: %d\n\r", PW_DC);
		PCA0CPL2 = 0xFFFF - PW_DC;
		PCA0CPH2 =(0xFFFF - PW_DC) >>8;
		;

	}
	else if (direction == 'r')
	{

		BI2=1;
		BI1=0;
		PW_DC = gain*658/255*AD_value+2800;
		desired_heading = 900;
		error_servo();
		
		Servo_speed();
		if(PW_DC > 3500)
			PW_DC = 3500;
		printf("Driver PW reverse: %d\n\r", PW_DC);
		PCA0CPL2 = 0xFFFF - PW_DC;
		PCA0CPH2 =(0xFFFF - PW_DC) >>8;
	}

}
// motor control 
void Motor_Direction(void)
{
	unsigned int temp;
	if(direction == 'r')
	{
		direction = 'f';	//Direction is forward
		
		
		Counts = 0;
			while(Counts < 150)	//wait 3 seconds
			{
				desired_heading = 900;
				error_servo();
				Servo_speed();
				DC_Speed();

				//desired_heading = desired_heading_s;
			}

	}
	else if( direction == 'f')	
	{
		direction = 'r';		//Direction is reverse
		desired_heading  = 900;
		
		error_servo();
		Servo_speed();
		Counts = 0;
		while(Counts < 150)		//wait 3 seconds
		{
			if(Counts == 75)
			{
				temp = ReadRanger();
				
				actual_heading = ReadCompass();
				error_servo();

					temp_servo_pw = PW_CENTER;
					PCA0CPL0 = 0xFFFF - temp_servo_pw;
					PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;
			

					
			}
			printf("Direction: %i \n\r", desired_heading);
			actual_heading = ReadCompass();
			error_servo();
		
				temp_servo_pw = PW_CENTER;
				PCA0CPL0 = 0xFFFF - temp_servo_pw;
				PCA0CPH0 = (0xFFFF - temp_servo_pw) >> 8;


			DC_Speed();

		}

	}

}