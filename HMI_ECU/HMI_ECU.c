/*
 ================================================================================================
 Name        : MC1.c
 Author      : Kerolos Sameh
 Description : MC1 application code
 ================================================================================================
 */

#include"keypad.h"
#include"lcd.h"
#include"uart.h"
#include<util/delay.h>
#include"Timer.h"
#include<avr/io.h>

#define SIZE_OF_PASSWORD              5
#define FIRST_PASSWORD_READY          0x10
#define FIRST_PASSWORSD_CONFIRMATION  0x60
#define ACTIVE_OPENING_DOOR_FEATURES  0x20
#define RECIEVE_OLD_PASSWORD          0x30
#define CHANGE_PASSWORD_READY         0x40
#define WRONG_PASSWORD                0x50
#define PASSWORD_NOT_MATCH            0x70
#define PASSWORD_MATCH                0x80
#define FIRST_PASSWORD_HAS_WRITTEN    0x90
#define MC1_READY                     0x91
#define PASSWORD_MISMATCHING_FOR_THREE_TIMES  3



static void GET_password(uint8 *password, uint8 size);
static uint8 get_operation(void);
static void LCD_showStrings(uint8 matching);
/*******************************************************************************
 *                          global variables                                   *
 *******************************************************************************/
uint8 g_string = 0; // this variable to show which string will be displayed on the LCD after a certain action
uint16 g_tick = 0;// indicate that the interrupt has been happened
uint8 seconds = 0;
/*******************************************************************************
 *                          call back functions                                *
 *******************************************************************************/
/*
 * these call back functions are executed when an interrupt happen in any timer register
 * the system enter any function then increases the number of ticks until it reaches
 * a specific value  (number of interrupts per second)
 */
/*
 * NOTE: you must aware of the number of interrupts per seconds in each function
 * by setting the configuration of the timer that will hold
 */
void Timer0_func(void) {
	g_tick++;
	if (g_tick == 4000) {
		seconds++;
		g_tick = 0;//to return counting again
	}
}
void Timer1_func(void) {
	g_tick++;
	if (g_tick == 500) {
		seconds++;
		g_tick = 0;
	}
}
void Timer2_func(void) {
	g_tick++;
	if (g_tick == 3921) {
		seconds++;
		g_tick = 0;
	}
}

int main(void) {
	/*******************************************************************************
	 *                             local variables                                 *
	 *******************************************************************************/
	uint8 i; // counter
	uint8 operation; // to store the value returned from get_opration function.
	/* this variable to check if the old password matches
	 *  the new password and take some actions that displays on LCD
	 * */
	uint8 matching;
	uint8 counter = 0; // to count the times of failed password when change password needed
	uint8 Flag; //  this flag to check if the EEPROM has a starting password or not
	uint8 DataPassword[SIZE_OF_PASSWORD]; // for storing  password elements in it.
	/*******************************************************************************
	 *                 configuration of init functions for the drivers              *
	 *******************************************************************************/
	UART_ConfigType Config_UART={bit8,Disabled,bit1,9600};

	Timer_ConfigType Config_TIMER0 = { 0, 250, F_CPU_8, CTC_8BIT };
	Timer_ConfigType Config_TIMER1 = {0, 2000, F_CPU_8, CTC_16BIT};
	Timer_ConfigType Config_TIMER2 = {0, 0, F_CPU_8, NORMAL };
	UART_init(&Config_UART); // configure the UART to be ready for recieve and send
	LCD_init();// configure the lcd to be ready for display a messages
	SREG |= (1 << 7);// enable interrupt global bit for timer driver
	TIMER0_setCallBack(Timer0_func);
	TIMER1_setCallBack(Timer1_func);
	TIMER2_setCallBack(Timer2_func);

	/*******************************************************************************
	 *                 application instructions                                    *
	 *******************************************************************************/
	while(UART_recieveByte()!=MC1_READY); // waiting for MC2 to be ready for starting
	 Flag = UART_recieveByte(); // getting the value of the flag from MC2 to make the compare

	if (Flag != FIRST_PASSWORD_HAS_WRITTEN) {
		LCD_displayString("welcome to the ");
		LCD_displayStringRowColumn(1, 0, "system");
		_delay_ms(1000);
		while (1) {
			g_string = 0;// to show ENTER PASSWORD
			GET_password(DataPassword, SIZE_OF_PASSWORD); //getting the first password
			g_string = 1;//to show re_enter PASSWORD
			UART_sendByte(FIRST_PASSWORD_READY); // warning MC2 to be ready for receiving the password
			for (i = 0; i < SIZE_OF_PASSWORD; i++) {
				UART_sendByte(DataPassword[i]); // sending the array of password digit by digit by the UART in a for loop
			}
			GET_password(DataPassword, SIZE_OF_PASSWORD);//getting the re_password
			UART_sendByte(FIRST_PASSWORSD_CONFIRMATION);// warning MC2 to be ready for receiving the re_password
			for (i = 0; i < SIZE_OF_PASSWORD; i++) {
				UART_sendByte(DataPassword[i]);// sending the array of password digit by digit by the UART in a for loop
			}
			_delay_ms(200);
			matching = UART_recieveByte(); // getting the result of matching the two passwords from MC2 by the UART
			LCD_showStrings(matching); // send this value to this function to do some actions on the LCD
			if (matching == PASSWORD_MATCH)
				break; // terminate the while loop
		}
	}
	else{
		/*
		 * do nothing
		 */
	}


	while (1) {
		operation = get_operation();
		if (operation == '+') {
			while(counter < PASSWORD_MISMATCHING_FOR_THREE_TIMES){
			g_string = 0; // to show Enter password
			GET_password(DataPassword, SIZE_OF_PASSWORD);//entering a password to send to MC2
			UART_sendByte(operation);// sending the operation to MC2
			for (i = 0; i < SIZE_OF_PASSWORD; i++) {
				UART_sendByte(DataPassword[i]); // for loop to send the password Byte by Byte to MC2
			}
			_delay_ms(500);
			matching = UART_recieveByte();// getting the result of matching the two passwords from MC2 by the UART
			LCD_showStrings(matching);// send this value to this function to do some actions on the LCD
			if (matching == PASSWORD_MATCH) {
				TIMER0_init(&Config_TIMER0); // configure timer 0 to start counting
				LCD_displayString("Door is");
				LCD_displayStringRowColumn(1, 1, "Unlocking”");
				while (seconds != 15); // wait until the counter reaches 15 seconds
				LCD_displayStringRowColumn(1, 1, "Unlocked” ");
				while (seconds != 18);// wait until the counter reaches 17 seconds
				LCD_displayStringRowColumn(1, 1, "locking”");
				while (seconds != 33);// wait until the counter reaches 32 seconds
				LCD_displayStringRowColumn(1, 1, "Locked");
				while (seconds != 34);// wait until the counter reaches 33 seconds
				TIMER0_deinit(); // stop the timer by deactivate the clock and the control register
				seconds = 0; // return the seconds to 0 to re-use it
				counter = 0;
				break;
				} else {
					counter++;// this means that the user fails to enter the old password
				}

			}


		} else if (operation == '-') {
			while (counter < PASSWORD_MISMATCHING_FOR_THREE_TIMES) {
				g_string = 2; //to show enter the old password
				GET_password(DataPassword, SIZE_OF_PASSWORD);//entering the old password to send to MC2
				UART_sendByte(operation);// sending the operation to MC2
				for (i = 0; i < SIZE_OF_PASSWORD; i++) {
					UART_sendByte(DataPassword[i]);// for loop to send the old password Byte by Byte to MC2
				}
			//	_delay_ms(500);
				matching = UART_recieveByte();// getting the result of matching the two passwords from MC2 by the UART
				LCD_showStrings(matching);// send this value to this function to do some actions on the LCD
				if (matching == PASSWORD_MATCH) {
					g_string = 3;//to show Enter the new password
					GET_password(DataPassword, SIZE_OF_PASSWORD);//getting the password that wills end to MC2 to store in the EEPROM
					UART_sendByte(CHANGE_PASSWORD_READY);// warning MC2 to be ready for receiving the new password
					for (i = 0; i < SIZE_OF_PASSWORD; i++) {
						UART_sendByte(DataPassword[i]);// for loop to send the new password Byte by Byte to MC2
					}
					_delay_ms(500);
					LCD_clearScreen();
					LCD_displayString("PASSWORD IS SET");
					_delay_ms(1000);
					counter = 0;
					break; // terminated the while loop because the user enter the password corrctly

				} else {
					counter++; // this means that the user fails to enter the old password

				}
			}

		}
		if (counter == PASSWORD_MISMATCHING_FOR_THREE_TIMES) {
			UART_sendByte(WRONG_PASSWORD); // warning MC2 that the user enter the password three times incorrectly
			TIMER1_init(&Config_TIMER1); // configure timer 1 to start counting
			LCD_clearScreen();
			LCD_displayString("system is off");
			while (seconds != 60)
				; // wait until the timer reaches 60 seconds
			counter = 0;
			TIMER1_deinit(); // stop the timer by deactivate the clock and the control register
			seconds = 0; // return the seconds to 0 to re-use it
		}

	}

}
/************************************************************************************
 * Function Name: Get_password
 * Parameters (in): *Password , size
 * Parameters (out): None
 * Return value: void
 * Description: responsible to enter the digits of the password in an array
 **********************************************************************************/

static void GET_password(uint8 *password, uint8 size) {
	/*
	 * the password must consist of 5 digits then the user must enter 13
	 * if the 6th digit is not enter 13 . the system will repeat itself until
	 * pressing enter 13.
	 * the user must enter 0-9 numbers only
	 */
	_delay_ms(500);
	uint8 i = 0;
	LCD_clearScreen();
	if (g_string == 0) {
		LCD_displayString("Enter password");
	} else if (g_string == 1) {
		LCD_displayString("re_Enter Pass");
	} else if (g_string == 2) {
		LCD_displayString("Enter OLD Pass");
	} else if (g_string == 3) {
		LCD_displayString("Enter NEW Pass");
	}

	LCD_moveCursor(1, 1);
	while (i <= size) {

		password[i] = KEYPAD_getPressedKey(); // get a number from keypad
		if (i == 5 && KEYPAD_getPressedKey() != 13)
			continue;
		else if (i == 5 && KEYPAD_getPressedKey() == 13)
			break;
		if (password[i] >= 0 && password[i] <= 9) {
			LCD_intgerToString(password[i]);
			_delay_ms(500);
			LCD_displayStringRowColumn(1, i + 1, "*"); // write * for every seccessed pressed key after showing the digit

		} else {
			continue;

		}
		i++;
	}
	//_delay_ms(500);
}
/************************************************************************************
 * Function Name: get_operation
 * Parameters (in): none
 * Parameters (out): operation (uint8)
 * Return value: (uint8)
 * Description: responsible to get a certain operation just + or -
 **********************************************************************************/

static uint8 get_operation(void) {
	uint8 Operation;
	while (1) {
		LCD_clearScreen();
		LCD_displayString("+ Open the door");
		LCD_displayStringRowColumn(1, 0, "- Change Password");
		Operation = KEYPAD_getPressedKey();// the system will wait until pressed a key
		if (Operation == '+' || Operation == '-') {
			break;
		} else { // if the user does not enter a right operation the system will repeated again
			LCD_clearScreen();
			LCD_displayString("ERROR Selection");
			_delay_ms(700);
		}
	}
	LCD_clearScreen();
	return Operation;
}
/************************************************************************************
 * Function Name: LCD_showStrings
 * Parameters (in): matching
 * Parameters (out): None
 * Return value: void
 * Description: responsible to display some strings on the LCD due to a certain action
 * is happened in MC2
 **********************************************************************************/

static void LCD_showStrings(uint8 matching) {
	LCD_clearScreen();
	if (matching == PASSWORD_NOT_MATCH && g_string == 1) {
		LCD_displayString("WRONG CONFIGURATION");
		LCD_displayStringRowColumn(1, 0, "please try again");
		_delay_ms(1500);
	} else if (matching == PASSWORD_MATCH && g_string == 1) {
		LCD_displayString("PASSWORD IS SET");
		_delay_ms(1500);
	} else if (matching == PASSWORD_NOT_MATCH && g_string == 0) {
		LCD_displayString("PASS NOT MATCH");
		_delay_ms(1500);
	} else if (matching == PASSWORD_MATCH && g_string == 2) {
		g_string = 3;
		LCD_displayString("PASS IS MATCHING");
		_delay_ms(1500);
	} else if (matching == PASSWORD_NOT_MATCH && g_string == 2) {
		g_string = 3;
		LCD_displayString("PASS NOT MATCH");
		_delay_ms(1500);
	}
}
