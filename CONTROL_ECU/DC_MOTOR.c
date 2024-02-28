/*
 * DC_MOTOR.c
 *
 *  Created on: Oct 9, 2023
 *      Author: Kerolossamehel-shaip
 */
#include <avr/io.h>
#include "gpio.h"
#include "DC_MOTOR.h"
#include <util/delay.h>
#include "Timer0_PWM.h"
void DcMotor_Init(void){

	GPIO_setupPinDirection(PORTB_ID, PIN0_ID, PIN_OUTPUT); /* PC0 Output Pin */
	GPIO_setupPinDirection(PORTB_ID, PIN1_ID, PIN_OUTPUT); /* PC0 Output Pin */
	GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_LOW);
	GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_LOW);
}
void DcMotor_Rotate(DcMotor_State state,uint8 speed){

	if(state==OFF){
		GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_LOW);
		GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_LOW);}
	if(state==CW){
		GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_LOW);
		GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_HIGH);}
	if(state==ACW){
		GPIO_writePin(PORTB_ID, PIN0_ID, LOGIC_HIGH);
		GPIO_writePin(PORTB_ID, PIN1_ID, LOGIC_LOW);}

	PWM_Timer0_Start(speed);


}


;

