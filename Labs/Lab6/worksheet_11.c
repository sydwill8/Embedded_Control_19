/*
    Section: 2
    Date: 6/23/19
    File name: Worksheet 11
    Program description:
*/

#include <c8051_SDCC.h>
#include <stdio.h>
#include <i2c.h>

/* Global Variables */
/////////////////////////
/*
 * NOTE: The equations printed in old Worksheet 11 interpret (int) as (signed int) and
 * (long) as (signed long).  SDCC versions >= 3.6 assume 'unsigned', therefore, we
 * explicitly add 'signed' below.  These are the same equations as previously on the worksheet.
 */
/////////////////////////

unsigned int ReadCompass();
signed int error_servo();
void SMB0_Init();

unsigned int desired = 0;           // set this value
unsigned int actual = 0;            // set this value
unsigned int kp = 0;                // set this value
unsigned int kd = 0;                // set this value
signed int PW_CENTER = 2765;             // set this value
signed int previous_error = 0;      // set this value
signed int error = 0;               // set this value
signed long temp_servo_pw = 0;
unsigned int desired_heading = 1350,actual_heading;

void main()
{
    Sys_Init();
	SMB0_Init();
    putchar(' ');
    error = desired-actual;
/*//  two byte calculation
    temp_motorpw_2byte = pw_neut+kp*error+kd*(error-previous_error);
    printf("The two byte calculation of motorpw is %d \r\n",temp_motorpw_2byte);
//  equation form 1, long ints
    temp_motorpw_alg1 = pw_neut+kp*error+kd*(error-previous_error);
    printf("Algorithm 1, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg1);
//  equation form 2, long ints
    temp_motorpw_alg2 = pw_neut+(signed long)kp*error+kd*(error-previous_error);
    printf("Algorithm 2, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg2);
//  equation form 3, long ints
    temp_motorpw_alg3 = (signed long)( pw_neut + kp * (error) + kd * (error - previous_error));
    printf("Algorithm 3, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg3);
//  equation form 4, long ints
    temp_motorpw_alg4 = pw_neut+kp*(signed int)(error)+kd*(signed int)(error-previous_error);
    printf("Algorithm 4, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg4);
//  equation form 5, long ints
    temp_motorpw_alg5 = (signed long)pw_neut+(signed long)(kp*(error))+(signed long)(kd*(error-previous_error));
    printf("Algorithm 5, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg5);
//  equation form 6, long ints
    temp_motorpw_alg6 = (signed long)pw_neut+(signed long)kp*(signed long)error+(signed long)kd*(signed long)(error-previous_error);
    printf("Algorithm 6, four byte calculation of motorpw is %ld \r\n",temp_motorpw_alg6);*/
	ReadCompass();
	error_servo();

}

void SMB0_Init()
{
	SMB0CR = 0x93;	//set SCL to 100KHz
	ENSMB = 1;		//bit 6 of SMB0CN, enable the SMBus

}

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


signed int error_servo()
{
		unsigned int kp = 12, kd = 0;
		signed int errorS = 0, previous_error = 0;

		temp_servo_pw = PW_CENTER;
		actual_heading = ReadCompass();
		errorS = desired_heading - actual_heading;	
		printf("Direction error: %i \n\r", errorS);
		temp_servo_pw = PW_CENTER + (signed long)kp*errorS + kd*(errorS-previous_error);
		printf("Steering PW Before: %i \n\r", temp_servo_pw);

		//Following statements adjust the calculated PW based on the error
		if(error >= 1800)
		{
			
			errorS = errorS - 3600;
			temp_servo_pw = PW_CENTER + (signed long)kp*errorS + kd*(errorS-previous_error);
			printf("Steering PW: %i \n\r", temp_servo_pw);
			return temp_servo_pw;
		}
		else if(errorS <= -1800)
		{
			errorS = errorS + 3600;
			temp_servo_pw = PW_CENTER + (signed long)kp*errorS + kd*(errorS-previous_error);
			printf("Steering PW: %i\n \r", temp_servo_pw);
			return temp_servo_pw;

		}
	
		else 
		{
			
			temp_servo_pw = PW_CENTER + (signed long)kp*errorS + kd*(errorS-previous_error);
			printf("Steering PW: %i \n\r", temp_servo_pw);
			return temp_servo_pw; 

		} 
}




