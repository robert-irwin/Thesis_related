/*
 * DiscreteRMS.c
 *
 *  Created on: Jun 14, 2016
 *      Author: robertirwin
 */

#include "DiscreteRms.h"
#include <math.h>
#include <stdint.h>

/* Name:		RMS_Init
 * Description:	Initializes critical information regarding RMS calculations, including
 * 				the reference voltage for ADCs, the steps of the ADC, any DC offset that
 * 				should be excluded from the calculation, and the number of samples per period (N).
 * 	NOTE:		Can be used on microprocessors to calculate accurate RMS values.
 * 				Set REF to the reference voltage of the system, steps to
 * 				2^n where n is the precision of the ADC, and offset to the DC offset
 * 				of the system. Otherwise set REF to 1, steps to 2, and offset to 0.
 */
void RMS_Init(RMS *rms, float ref, float steps, float offset, float N )
{
	rms->curr_sum = 0.0;
	rms->rms_value = 0.0;
	rms->ref = ref;
	rms->steps = steps;
	rms->offset = offset;
	rms->N = N;
	rms->btv = (rms->ref)/(rms->steps);
	rms->temp_sum = 0.0;
	rms->temp_diff = 0.0;

}

/* Name: 		calculateRMS
 * Description: Takes the pointer to current RMS value, the pointer to the
 * 				most current sample, and the pointer to the sample
 * 				exactly one period prior to the current sample. Adds
 * 				the square of the current value to the current sum and
 * 				subtracts the square of the previous sample from sum.
 */
void calculateRMS(RMS * rms, uint16_t * curr_val, uint16_t * prev_val )
{
	rms->temp_sum = (((float) *curr_val-rms->offset) + ((float) *prev_val-rms->offset)) * (rms->btv);
	rms->temp_diff = (((float) *curr_val-rms->offset) - ((float) *prev_val-rms->offset)) * (rms->btv);
	rms->curr_sum += (rms->temp_sum * rms->temp_diff);
	rms->rms_value = sqrt(rms->curr_sum/rms->N);
}

/* Name:		PowerCalc_Init
 * Description:	Initializes critical information regarding Power calculations, including
 * 				the reference voltage for ADCs, the steps of the ADC, any DC offset that
 * 				should be excluded from the calculation, and the number of samples per period (N).
 * 	NOTE:		Can be used on microprocessors to calculate accurate Power values.
 * 				Set REF to the reference voltage of the system, steps to
 * 				2^n where n is the precision of the ADC, and offset to the DC offset
 * 				of the system. Otherwise set REF to 1, steps to 2, and offset to 0.
 */
void PowerCalc_Init(PowerCalc *pc, float ref, float steps, float offset, float N )
{
	pc->curr_sum = 0.0;
	pc->AVG_pow = 0.0;
	pc->Q_pow = 0.0;
	pc->ref = ref;
	pc->steps = steps;
	pc->offset = offset;
	pc->N = N;
	pc->btv = (pc->ref)/(pc->steps);
	pc->inst_pow = 0.0;
	pc->prev_inst_pow = 0.0;
}

/* Name: 		Calculate_AVGPower
 * Description:	Calculates average power over a period defined by N samples which is a member of the
 * 				PowerCalc structure. Inputs are the most recent voltage and current values (ints),
 * 				the last voltage and current values (ints) from the previous
 * 				period as well as the pointer to the PowerCalc structure. The average power is stored
 * 				as a member in the PowerCalc structure.
 */
void Calculate_AVGPower(PowerCalc *pc, uint16_t * cvolt, uint16_t * ccurr, uint16_t * pvolt, uint16_t * pcurr)
{
	pc->inst_pow = ((( (float) *cvolt - pc->offset) * pc->btv) * (( (float) *ccurr - pc->offset) * pc->btv));
	pc->prev_inst_pow = ((( (float) *pvolt - pc->offset) * pc->btv) * (( (float) *pcurr - pc->offset) * pc->btv));
	pc->curr_sum += pc->inst_pow - pc->prev_inst_pow;
	pc->AVG_pow = pc->curr_sum/pc->N;

}

/* Name:		Calculate_QPower
 * Description: Calculates reactive power. Inputs are the pointer to the PowerCalc structure, pointer to
 * 				the rms current and pointer to the rms voltage.
 */
void Calculate_QPower(PowerCalc *pc, float * irms, float * vrms)
{
	double theta = acos(pc->AVG_pow/((*vrms) * (*irms)));
	pc->Q_pow = (*vrms) * (*irms) * sin(theta);
}
