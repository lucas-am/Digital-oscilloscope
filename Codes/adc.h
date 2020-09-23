/*
 * adc.h
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "global.h"

#ifndef ADC_H_
#define ADC_H_

/*!
 * \fn ADC0_IRQHandler (void)
 * \brief Rotina de serviço para ADC0 (Vetor 31)
 */
void ADC0_IRQHandler (void);

/*!
 * \fn calibraADC (void)
 * \brief calibra o ADC.
 */
void calibraADC (void);

/*!
 * \fn configuraADC (void)
 * \brief configura o ADC.
 */
void configuraADC(void);


#endif /* ADC_H_ */
