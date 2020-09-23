/*
 * systick.c
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "systick.h"
#include "global.h"

/*!
 * \fn InitSysTick (void)
 * \brief Inicializa o temporizador SysTick.
 */
void InitSysTick (void) {
   /*!
    * core clock: 20.971520MHz
    */
    SYST_RVR = SysTick_RVR_RELOAD(4194);     	//!Configura interrupção a cada 10us
    SYST_CVR = SysTick_CVR_CURRENT(2097);               
    SYST_CSR |= (SysTick_CSR_CLKSOURCE_MASK);
    SYST_CSR &= ~(SysTick_CSR_ENABLE_MASK | SysTick_CSR_TICKINT_MASK);
}

/*!
 * \fn enableNVIC (void)
 * \brief Habilita interrupÃƒÂ§ÃƒÂµes via NVIC.
 */
void enableNVIC(void) {

	NVIC_IPR3 = (uint32_t)((NVIC_IPR3 & (uint32_t)~(uint32_t)( ///< NVIC_IPR3: PRI_15=0x80
	               NVIC_IP_PRI_15(0x7F)
	              )) | (uint32_t)(
	               NVIC_IP_PRI_15(0x40)
	              ));                                  
	NVIC_ISER |= NVIC_ISER_SETENA(GPIO_PIN(31-16));   ///< Habilita interrup&ccedil;&atilde;o ADC0                               
	NVIC_ICPR |= NVIC_ICPR_CLRPEND(GPIO_PIN(31-16));  ///< Limpa as poss&iacute;veis pend&ecirc;ncias do ADC0    
	
    NVIC_ISER |= GPIO_PIN (12);          //!Habilita interrupção da UART
    NVIC_ICPR |= GPIO_PIN (12);          
    NVIC_IPR7 |= NVIC_IP_PRI_12(0x80);   //!Configura Prioridade 2 a interrupção do UART0
    UART0_C2 &= ~UART_C2_TIE_MASK;
}
