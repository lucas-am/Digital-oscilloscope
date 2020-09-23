/*
 * uart.c
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "uart.h"
#include "global.h"

/*!
 * \fn insere_item
 * \brief insere item no buffer circular
 */
uint8_t insere_item (tipo_buffer b, char d) {
	short indice_t = (buffer_transmissor.head+1)%SIZE;
	short indice_r = (buffer_receptor.head+1)%SIZE;
	
	switch (b) {
	case RECEPTOR:
		if(indice_r==buffer_receptor.tail)return 0;		//se o buffer estiver cheio não faz nada
		buffer_receptor.data[buffer_receptor.head]=d;	//insere o parâmetro d no buffer
		buffer_receptor.head=indice_r;					//incrementa head
		break;
	case TRANSMISSOR:
		if(indice_t==buffer_transmissor.tail)return 0;		//se o buffer estiver cheio não faz nada	
		buffer_transmissor.data[buffer_transmissor.head]=d;	//insere o parâmetro d no buffer
		buffer_transmissor.head=indice_t;					//incrementa head
		UART0_C2 |= UART_C2_TIE_MASK; 						//habilita a interrupção do transmissor
		break;
	}
	return 1;
}

/*!
 * \fn remove_item
 * \brief remove item no buffer circular
 */
char remove_item (tipo_buffer b) {
	char d;
	
	switch (b) {
	case RECEPTOR:
		if(buffer_receptor.head != buffer_receptor.tail){		//se o buffer não estiver vazio
			d = buffer_receptor.data[buffer_receptor.tail];		//le o que está no buffer e guarda em d
			buffer_receptor.tail=(buffer_receptor.tail+1)%SIZE;	//incremeta tail
			
			if(buffer_receptor.head == buffer_receptor.tail){	//Se o buffer estiver vazio após operação
						buffer_receptor.head = 0;				//reseta head
						buffer_receptor.tail = 0;				//reseta tail
			}
		}
		else{
			return 0xff;
		}
		break;
	case TRANSMISSOR:
		if(buffer_transmissor.head != buffer_transmissor.tail){			//se o buffer não estiver vazio
			d = buffer_transmissor.data[buffer_transmissor.tail];		//le o que está no buffer e guarda em d
			buffer_transmissor.tail=(buffer_transmissor.tail+1)%SIZE;	//incremeta tail
			
			if(buffer_transmissor.head == buffer_transmissor.tail){		//Se o buffer estiver vazio após operação
						buffer_transmissor.head = 0;					//reseta head
						buffer_transmissor.tail = 0;					//reseta tail
						UART0_C2 &= ~UART_C2_TIE_MASK;					//Desabilita interrupção do transmissor
			}
		}
		else{
			return 0xff;
		}
	break;
	   		
	}
	return d;
}

/*!
 * \fn UART0_IRQHandler (void)
 * \brief Rotina de serviço para UART0 (Vetor 28)
 */
void UART0_IRQHandler(void) {
	char d;
	
	if (UART0_S1 & UART0_S1_RDRF_MASK){
		d = UART0_D;

		insere_item(RECEPTOR, d);	//Insere o dado no buffer do receptor	
	}

	else if (UART0_S1 & UART0_S1_TDRE_MASK) {
	    d = remove_item(TRANSMISSOR);
		if (d != 0xff)
			UART0_D = d;
	}	
}

/*!
 * \fn initUART (void)
 * \brief Inicializa os pinos conectados a UART0 e ao OpenSDA.
 */
void initUART(void) {
	/*!
	 * Configura&ccedil;&atilde;o de SIM
         */
	SIM_CLKDIV1 &= (uint32_t)~(uint32_t)(SIM_CLKDIV1_OUTDIV4(0x7));  ////< bus_clock = clock/1
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(0x1);	///< configura a fonte de relógio (20.971520MHz)
	SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;	 
	                                         ///< no reset, o valor é zero (FLL)
	                                         ///< mas não deixa de ser uma boa
	                                         ///< prática de inicializar explicitamente
	
	/*!
	 * Configura&ccedil;&atilde;o dos pinos que servem UART0 
         */
	PORTA_PCR1 |= PORT_PCR_MUX(0x2);         ///< UART0_RX
	PORTA_PCR2 |= PORT_PCR_MUX(0x2);         ///< UART0_TX
		
	/*!
	 * Configura&ccedil;&atilde;o do m&oacute;dulo UART0 
         */
	UART0_C1 &= ~(UART0_C1_PE_MASK |			
			UART0_C1_ILT_MASK |
			UART0_C1_WAKE_MASK |                  
			UART0_C1_M_MASK |                       
			UART0_C1_DOZEEN_MASK |
			UART0_C1_LOOPS_MASK );
	UART0_C2 &= ~(UART_C2_RE_MASK |            
				UART_C2_TE_MASK);
	UART0_BDH &= ~(UART_BDH_SBNS_MASK |                       
				UART_BDH_RXEDGIE_MASK | 
				UART_BDH_LBKDIE_MASK);
	UART0_C4 |= UART0_C4_OSR(0xF);                
	/*!
	 * baud rate 19200 (Se&ccedil;&atilde;o 8.3.2 em 
	 * ftp://ftp.dca.fee.unicamp.br/pub/docs/ea871/ARM/KLQRUG.pdf)
	 */
	UART0_BDH |= UART_BDH_SBR(0x00);                             
	UART0_BDL |= UART_BDL_SBR(0x44);             
	
	UART0_S1 |= (UART0_S1_PF_MASK |              ///< Registradores de estado: w1c
				UART0_S1_FE_MASK |
				UART0_S1_NF_MASK |
				UART0_S1_OR_MASK |
				UART0_S1_IDLE_MASK);
		
	UART0_C2 |= (UART_C2_RIE_MASK |  
		UART_C2_RE_MASK |        
		UART_C2_TE_MASK);	             			          
}
