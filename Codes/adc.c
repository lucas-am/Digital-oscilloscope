/*
 * adc.c

 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "adc.h"
#include "global.h"

/*!
 * \fn ADC0_IRQHandler (void)
 * \brief Rotina de serviço para ADC0 (Vetor 31)
 */
void ADC0_IRQHandler (void) {
	
	if(( ADC0_SC1A & ADC_SC1_COCO_MASK ) == ADC_SC1_COCO_MASK)
	{
		value = ADC0_RA;              ///< ler o registrador do conversor (limpa COCO)
		read = 1;
	}
}

/*!
 * \fn calibraADC (void)
 * \brief calibra o ADC.
 */
void calibraADC (void) {
	uint16_t tmp=0;
	
	SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;      ///< habilita clock do m&oacute;dulo ADC0                             
	/*! Configura&ccedil;&atilde;o do rel&oacute;gio
	 * Os bits de ADC0_CFG1 s&atilde;o resetados em 0: 
	 * ADLSMP=0 (short sample time) 
	 */ 
	ADC0_CFG1 &= (uint32_t)~(uint32_t)(ADC_CFG1_ADLPC_MASK); ///< opera&ccedil;&atilde;o normal
	ADC0_CFG1 |= ADC_CFG1_ADIV(0x3) |             ///< ADCK = (source clock)/8
	            ADC_CFG1_ADLSMP_MASK |            ///< intervalo de amostragem longo
	            ADC_CFG1_MODE(0x3) |             ///< convers&atilde;o em 16 bits
	            ADC_CFG1_ADICLK(0x1);            ///< (source clock) = (bus clock)/2
	/*! Configura&ccedil;&atilde;o dos ciclos adicionais 
	 * Os bits de ADC0_CFG2 s&atilde;o resetados em 0: MUXSEL = 0 (canais ADxxa)  
	 */
	ADC0_CFG2 &= (uint32_t)~(uint32_t)(
	             ADC_CFG2_ADACKEN_MASK |     ///< desabilita a sa&iacute;da do sinal de clock      
	             ADC_CFG2_ADLSTS(0x3));     ///< 24 (20 extra+4) ADCK ciclos por amostra
	ADC0_CFG2 |=  ADC_CFG2_ADHSC_MASK;       ///< sequ&ecirc;ncia de convers&atilde;o em alta velocidade com 2 ciclos adicionais
	/*! Os bits de ADC0_SC2 s&atilde;o resetados em 0: ACFE=0 (desabilitada fun&ccedil;&atilde;o de compara&ccedil;&atilde;o) */
	ADC0_SC2 &= ADC_SC2_REFSEL(0x0);    ///< Tens&otilde;es de refer&ecirc; VREFH e VREFL                                
	  
	/*! Os bits de ADC0_SC3 s&atilde;o resetados em 0 */
	ADC0_SC3 |= ADC_SC3_AVGE_MASK | ///< habilita fun&ccedil;&atilde;o em hardware
				ADC_SC3_AVGS(0x3);  ///< n&uacute;mero de amostras por m&eacute;dia = 32                
	ADC0_SC3 |= (uint32_t)(ADC_SC3_CAL_MASK);  ///< inicializa a calibra&ccedil;&atilde;o	

	while (ADC0_SC3 & (uint32_t)(ADC_SC3_CAL_MASK))  {}  ///< aguarda a calibra&ccedil;&atilde;o

	/*! Gere as corre&ccedil;&otilde;es de erros de ganhos, conforme o procedimento
	 *  na Se&ccedil;&atilde;o 28.4.6 
	 */
	tmp = ADC0_CLP0 + ADC0_CLP1 + ADC0_CLP2 + ADC0_CLP3 + ADC0_CLP4 + ADC0_CLPS; ///< soma PS
	tmp = tmp/2;                            ///< divide por 2
	tmp |= (uint16_t)(0x8000);              ///< set o bit mais significativo
	ADC0_PG = tmp;                          ///< plus-side gain
	/*! Somente &uacute;til para modos diferenciais */
	tmp = ADC0_CLM0 + ADC0_CLM1 + ADC0_CLM2 + ADC0_CLM3 + ADC0_CLM4 + ADC0_CLMS; ///< soma PS
	tmp = tmp/2;                            ///< divide por 2
	tmp |= (uint16_t)(0x8000);              ///< set o bit mais significativo
	ADC0_MG = tmp;	                        ///< minus side gain
}

/*!
 * \fn configuraADC (void)
 * \brief configura o ADC.
 */
void configuraADC(void) {
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;     ///< habilita clock do m&oacute;dulo PORTB
	/*! Pino que serve um canal do m&oacute;dulo ADC0 */
	PORTB_PCR0 &= (uint32_t)~(uint32_t)((PORT_PCR_ISF_MASK |        ///< baixa bandeira
			         PORT_PCR_MUX(0x7)));                            ///< fun&ccedil;&atildeo ADC0       
	/*! 
	 * Configura&ccedil&atilde;o do rel&oacute;gio: convers&atilde;o em 16 bits,
	 * frequ&ecirc;ncia ADCK = (bus clock)/2*8
	 */ 
	ADC0_CFG1 &= (uint32_t)~(uint32_t)(
	          ADC_CFG1_ADLSMP_MASK ) ;   ///< intervalo de amostragem curto
	ADC0_CFG1 |= ADC_CFG1_ADLPC_MASK;    ///< opera&ccedil;&atilde;o low-power
	/*! 
	 * Configura&ccedil;&atilde;o da velocidade: MUXSEL=0 (canais ADxxa), ADACKEN=0 (desabilitada
	 * sa&iacute;da ass&iacute;ncrona, ADLSTS=00 (24 ciclos)
	 */
	ADC0_CFG2 &= (uint32_t)~(uint32_t)(
			  ADC_CFG2_ADHSC_MASK);      ///< sequ&ecirc;ncia de convers&atilde;o normal      
	/*! 
	 * Configura o modo de convers&atilde;o: ADCO=0 (modo não cont&iacute;nuo), AVGE=0 
	 */
	ADC0_SC3 &= (uint32_t)~(uint32_t)(
				ADC_SC3_AVGS(0x3));    ///<  n&uacute;mero de amostras por m&eacute;dia = 4     
	ADC0_SC3 |= ADC_SC3_CALF_MASK;      ///< baixa a bandeira de "calibration failed"
      
	/*! ADC0_SC1A: COCO=0, DIFF=0 (medida unipolar) e ADCH=0b11111 (canais desabilitados) no reset */
	ADC0_SC1A |= ADC_SC1_AIEN_MASK;           ///< habilita interrup&ccedil;&atilde;o
}
