/*
 * DiscreteRMS.h
 *
 *  Created on: Jun 14, 2016
 *      Author: robertirwin
 */

#ifndef DISCRETERMS_H_
#define DISCRETERMS_H_

#include 	<stdio.h>
#include	<math.h>
#include 	<stdint.h>

typedef struct RMS{
	float rms_value;
	float curr_sum;
	float ref;
	float steps;
	float offset;
	float N;
	float temp_diff;
	float temp_sum;
	float  btv;
}RMS;

/* Name:		RMS_Init
 * Description:	Initializes critical information regarding RMS calculations, including
 * 				the reference voltage for ADCs, the steps of the ADC, any DC offset that
 * 				should be excluded from the calculation, and the number of samples per period (N).
 * 	NOTE:		Can be used on microprocessors to calculate accurate RMS values.
 * 				Set REF to the reference voltage of the system, steps to
 * 				2^n where n is the precision of the ADC, and offset to the DC offset
 * 				of the system. Otherwise set REF to 1, steps to 2, and offset to 0.
 */
void RMS_Init(RMS *rms, float ref, float steps, float offset, float N );

/* Name: 		calculateRMS
 * Description: Takes the pointer to current RMS value, the pointer to the
 * 				most current sample, and the pointer to the sample
 * 				exactly one period prior to the current sample. Adds
 * 				the square of the current value to the current sum and
 * 				subtracts the square of the previous sample from sum.
 */
void calculateRMS(RMS * rms, uint16_t * curr_val, uint16_t * prev_val );


typedef struct PowerCalc{
	float AVG_pow;
	float Q_pow;
	float curr_sum;
	float ref;
	float steps;
	float offset;
	float N;
	float inst_pow;
	float prev_inst_pow;
	float  btv;
}PowerCalc;

/* Name:		PowerCalc_Init
 * Description:	Initializes critical information regarding Power calculations, including
 * 				the reference voltage for ADCs, the steps of the ADC, any DC offset that
 * 				should be excluded from the calculation, and the number of samples per period (N).
 * 	NOTE:		Can be used on microprocessors to calculate accurate Power values.
 * 				Set REF to the reference voltage of the system, steps to
 * 				2^n where n is the precision of the ADC, and offset to the DC offset
 * 				of the system. Otherwise set REF to 1, steps to 2, and offset to 0.
 */
void PowerCalc_Init(PowerCalc *pc, float ref, float steps, float offset, float N );

/* Name: 		Calculate_AVGPower
 * Description:	Calculates average power over a period defined by N samples which is a member of the
 * 				PowerCalc structure. Inputs are the most recent voltage and current values (ints),
 * 				the last voltage and current values (ints) from the previous
 * 				period as well as the pointer to the PowerCalc structure. The average power is stored
 * 				as a member in the PowerCalc structure.
 */
void Calculate_AVGPower(PowerCalc *pc, uint16_t * cvolt, uint16_t * ccurr, uint16_t * pvolt, uint16_t * pcurr);

/* Name:		Calculate_QPower
 * Description: Calculates reactive power. Inputs are the pointer to the PowerCalc structure, pointer to
 * 				the rms current and pointer to the rms voltage.
 */
void Calculate_QPower(PowerCalc *pc, float * irms, float * vrms);


#endif /* DISCRETERMS_H_ */
