/*
 * DC_MOTOR.h
 *
 *  Created on: Oct 9, 2023
 *      Author: Kerolossamehel-shaip
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_
typedef enum
{
	OFF,CW,ACW
}DcMotor_State;
void DcMotor_Init(void);
void DcMotor_Rotate(DcMotor_State state,uint8 speed);

#endif /* DC_MOTOR_H_ */
