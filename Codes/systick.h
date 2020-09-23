/*
 * systick.h
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "global.h"

#ifndef SYSTICK_H_
#define SYSTICK_H_

/*!
 * \fn InitSysTick (void)
 * \brief Inicializa o temporizador SysTick.
 */
void InitSysTick (void); 

/*!
 * \fn enableNVIC (void)
 * \brief Habilita interrupÃ§Ãµes via NVIC.
 */
void enableNVIC(void); 

#endif /* SYSTICK_H_ */
