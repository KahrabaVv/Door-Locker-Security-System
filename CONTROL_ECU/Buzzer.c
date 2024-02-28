/*
 * Buzzer.c
 *
 *  Created on: Oct 24, 2023
 *      Author: Kerolossamehel-shaip
 */
#include "Buzzer.h"
#include <avr/io.h>
#include "std_types.h"
#include "gpio.h"
void Buzzer_init(void){
	GPIO_setupPinDirection(BUZZER_OUTPUT_PORT,BUZZER_OUTPUT_PIN, PIN_OUTPUT);
	GPIO_writePin(BUZZER_OUTPUT_PORT,BUZZER_OUTPUT_PIN, LOGIC_LOW);
}
void Buzzer_off(void){
	GPIO_writePin(BUZZER_OUTPUT_PORT,BUZZER_OUTPUT_PIN, LOGIC_LOW);     // Set pin 5 in PORTC with value 0 (BUZZER OFF)
}
void Buzzer_on(void){
	GPIO_writePin(BUZZER_OUTPUT_PORT,BUZZER_OUTPUT_PIN, LOGIC_HIGH);       // Set pin 5 in PORTC with value 1 (BUZZER ON)}
}
