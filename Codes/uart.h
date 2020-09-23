/*
 * uart.h
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "global.h"

#ifndef UART_H_
#define UART_H_

/*
 * Enumera&ccedil&atilde;o do tipo de buffer
*/
typedef enum _tipo_buffer {
	RECEPTOR,
	TRANSMISSOR
} tipo_buffer;

/*
 * Estrutura para representar o buffer UART
*/
struct _buffer_uart {
	short head;
	short tail;
	char data[SIZE];
} buffer_transmissor, buffer_receptor;

/*!
 * \fn insere_item
 * \brief insere item no buffer circular
 */
uint8_t insere_item (enum _tipo_buffer b, char d);

/*!
 * \fn remove_item
 * \brief remove item no buffer circular
 */
char remove_item (enum _tipo_buffer b);

/*!
 * \fn initUART (void)
 * \brief Inicializa os pinos conectados a UART0 e ao OpenSDA.
 */
void initUART(void);

#endif /* UART_H_ */
