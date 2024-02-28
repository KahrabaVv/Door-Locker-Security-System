/*
 * Buzzer.h
 *
 *  Created on: Oct 24, 2023
 *      Author: Kerolossamehel-shaip
 */

#ifndef BUZZER_H_
#define BUZZER_H_


#define BUZZER_OUTPUT_PORT  PORTC_ID
#define BUZZER_OUTPUT_PIN   PIN3_ID

void Buzzer_init(void);
void Buzzer_off(void);
void Buzzer_on(void);

#endif /* BUZZER_H_ */
