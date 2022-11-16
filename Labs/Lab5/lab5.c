/* 
Lab5
Section: 2
Side: B
*/
/*
Lab 5 is designed to have the car move forward or backwards up an incline
depending on the position of SS2. SS1 controls the user input for the 
gains. The car reads the data from the accelerometer to determine the x
and y location of the car and moves based off of that location.
*/	

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
void SMB0_Init();
void Read_Accelerometer();
unsigned char read_AD_input(unsigned char pin_number);
signed int set_ServoPW();
signed int error_height(void);
void DC_Speed(void);
void user_input(void);
void Servo_speed(void);




//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

__sbit __at 0xB7 SS1;	//Slide Switch at Port 3 Pin 7
__sbit __at 0xB6 SS2;	//Slide Switch 2 at Port 3 Pin 6
__sbit __at 0xB3 BI1;	//BiLED1 at Port 3 Pin 3
__sbit __at 0xB1 BI2;	//BiLED2 at Port 3 Pin 1
__sbit __at 0x96 POT;	//Slide Switch at Port 1 Pin 6


//********PW for servo*****************
__xdata unsigned int PW_CENTER = 2775;
__xdata unsigned int PW_MIN = 1859;
__xdata unsigned int PW_MAX = 3271;
__xdata unsigned int loop;


//*********Counts****************
unsigned int h_count, r_count = 0;
unsigned int Counts;


//****Compass and Range**************
unsigned int actual_heading;
unsigned int new_heading, new_range;
signed int PW_DC;
long temp_servo_pw;		
unsigned char AD_value;	//Conversion value

char direction;

unsigned char input;

//****** gains ***********************
unsigned int ks;		//steering gain
unsigned int kdx;	//drive motor gains in x
unsigned int kdy;	//drive motor gains in y

//****** accelerometer values **********
signed int gx,gy;		//averages for x and y




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
	ADC_Init();
	
	Accel_Init_C();

	PCA0CP0 = 0xFFFF - PW_CENTER;
	PCA0CP2 = 0xFFFF - 2800;
	Counts = 0;			//clear count
	loop = 0;

	printf("Start");

	
  	while(1)
	{
		if(SS1) // When the Slide switch 1 is off, user input is called
					{
						
						//printf("SS1 off");
						BI1 = 0;
						BI2 = 1;
						PCA0CP0 = 0xFFFF - PW_CENTER;	//Set wheels bck to neutral position
						PW_DC = 2775;
						PCA0CPL2 = 0xFFFF - PW_DC;
						PCA0CPH2 =(0xFFFF - PW_DC) >>8;
						printf("off\n\r");
						user_input();
						Counts = 0;
					}
		else
		{
		
		
			
			
			while (!SS2)		//Slide Switch2 is activated, car moves forward
			{
				if (loop == 0)
				{
					Counts = 0;
				}
				loop++;
				
				
				direction = 'f';
				kdy = ((read_AD_input(0x06))/(0.6*255)) *(REF0CN);
				
				
					
				BI1 = 1;	//Red Off
				BI2 = 0;	//Green On
				
			
				//Counts = 0;
				Read_Accelerometer();
				
				set_ServoPW();
				Servo_speed();
				DC_Speed();

				//Display accelerations and Gains every 400 ms
				if(Counts == 400)
				{
					printf(" X acceleration: %i \n\r", gx);
					printf(" Y acceleration: %i \n\r", gy);
					printf(" x_gain: %d, y_gain: %d, steering gain: %d \n\r", kdx, kdy, ks);
					printf("PW_DC: %i\n\r", PW_DC);
					printf("Servo: %ld\n\r", temp_servo_pw);
					Counts = 0;
				}
				
				if(SS1) // When the Slide switch 1 is off, user input is called
					{
						
						
						BI1 = 0;
						BI2 = 1;
						PCA0CP0 = 0xFFFF - PW_CENTER;	//Set wheels bck to neutral position
						PW_DC = 2750;
						PCA0CPL2 = 0xFFFF - PW_DC;
						PCA0CPH2 =(0xFFFF - PW_DC) >>8;
						
						user_input();
						loop = 0;
						
					}		
				
				

				
			
			}
			while(SS2)		//Slide switch 2 is not activated, car moves in reverse
			{
				if (loop == 0)
				{
					Counts = 0;
				}
				loop++;
				
				
				direction = 'r';	//Sets the direction to reverse
				kdy = ((read_AD_input(0x06))/(0.6*255)) *(REF0CN);	//Conversion of POT to acceptable gain
				
				BI1 = 1;	//Red Off
				BI2 = 0;	//Green On
				
	

				Read_Accelerometer();
					
				
			
				set_ServoPW();
				Servo_speed();
				DC_Speed();	
				//Display accelerations and Gains every 400 ms
				if(Counts == 400)
				{
					printf(" X acceleration: %i \n\r", gx);
					printf(" Y acceleration: %i \n\r", gy);
					printf(" x_gain: %d, y_gain: %d, steering gain: %d \n\r", kdx, kdy, ks);
					printf("PW_DC: %i\n\r", PW_DC);
					printf("Servo: %ld\n\r", temp_servo_pw);
					Counts = 0;
				}	
				
				if(SS1) // When the Slide switch 1 is off, user input is called
					{
						
						BI1 = 0;
						BI2 = 1;
						PCA0CP0 = 0xFFFF - PW_CENTER;	//Set wheels bck to neutral position
						PW_DC = 2750;					//Set DC PW to Center
						PCA0CPL2 = 0xFFFF - PW_DC;
						PCA0CPH2 =(0xFFFF - PW_DC) >>8;
						
						user_input();
						loop = 0;
						
					}
				
					

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
    P1MDOUT |= 0x05;	//set output pin for CEX0 or CEX2 in push-pull mode

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


//Function for the PW for the steering
signed int set_ServoPW()
{
		temp_servo_pw = PW_CENTER - (gx * ks) ;	//ks is the steering gain
		return temp_servo_pw;
		
}


//Adjust Steering
void Servo_speed(void)
{

			if(new_heading == 1)		//enough overflows for a new heading
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


//User input
void user_input(void)
{
		lcd_clear();
	
		lcd_print("Enter value for\nservo \n\r");
		ks = kpd_input(1);
		

		lcd_clear();
		lcd_print("Enter value for\ndrive x gain\n\r");
		kdx = kpd_input(1);
		printf("\n\rYour key was: %i",kdx);



		lcd_clear();
		lcd_print("POT controls value for drive y gain \n\r");
			

}



//********************************DC Motor***************************

//Controls the Speed for motor either forward or reverse
void DC_Speed(void)
{
	//AD Conversion value from port 6
	AD_value = read_AD_input(0x06);
	
	if (direction == 'f')
	{
		BI2 =0;
		BI1 =1;
		PW_DC = 2750 + kdy*gy - kdx * abs(gx);	//Calculated Pulse width for DC motor
		if(PW_DC> 2750)
		{
			PW_DC = (PW_DC -2750);	//Equation to have the car continuously drive forward
			PW_DC = 2750 - PW_DC;
	
			
		}
		//Set limit for DC Motor
		if(PW_DC < 2000)
		{
			PW_DC = 2000;
	
			
		}
		//Limits to account for error in accelerometer
		if(gx <=20 && gx >= -20)
			PW_DC = 2750;
		if(gy <=20 && gy >= -20)
			PW_DC = 2750;
		PCA0CPL2 = 0xFFFF - PW_DC;
		PCA0CPH2 =(0xFFFF - PW_DC) >>8;
		

	}
	else if (direction == 'r')
	{

		BI2=0;
		BI1=1;
		PW_DC = (2750 + kdy*gy - kdx * abs(gx));
		
		//Set limits for DC Motor
		if(PW_DC < 2600)
		{
			PW_DC = (2600 - PW_DC);	////Equation to have the car continuously drive backward
			PW_DC = 2600 + PW_DC;
	
		} 
		if(PW_DC > 3500)
		{
			PW_DC = 3500;
		
		}
		if(gx <=20 && gx >= -20)
			PW_DC = 2750;
		if(gy <=20 && gy >= -20)
			PW_DC = 2750;
		PCA0CPL2 = 0xFFFF - PW_DC;
		PCA0CPH2 =(0xFFFF - PW_DC) >>8;
	
			
		
	}

}



//****************************************Accelerometer**********************

//Reqads data from the Accelerometer
void Read_Accelerometer(void)
{
	
	signed long avg_gx = 0;
	signed long avg_gy = 0;
	unsigned char i;
	unsigned char Data[4];
	unsigned char addr = 0x3A;

	r_count = 0;
	for(i = 0; i<8; i++)
	{
		i2c_read_data(addr, 0x27, Data, 1);

		if((Data[0] & 0x03) != 0x03)
		{
			i2c_read_data(addr, 0x27, Data, 1);
		}
		else
		{
			i2c_read_data(addr, 0x28 | 0x80, Data, 4);
			avg_gx += ((Data[1] << 8 | Data[0]) >> 4);
			avg_gy += ((Data[3] << 8 | Data[2]) >> 4);
		}


		
	}
	//Takes the averages of the accelerometer data
	gx = avg_gx / 8;
	gy = avg_gy / 8;

}
			