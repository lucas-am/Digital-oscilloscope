/*
 * global.h
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define GPIO_PIN(x)  ((1)<<(x)) ///< obtem o bit do pino x
#define MAX 0xffff              ///< maximum conversion extent (16 bits)
#define VREFH 3305              ///< tens&atilde;o de refer&ecirc;ncia alta (mV)
#define VREFL 0                 ///< tens&atilde;o de refer&ecirc;ncia baixa (mV)
#define SIZE 30                 ///< Tamanho do buffer circular
#define SIZEVEC 1000            ///< Tamanho do vetor de dados
#define LINHAS 41               ///< Quantidade de linhas da tela de impressão
#define COLUNAS 150             ///< Quantidade de colunas da tela de impressão

uint8_t read;
unsigned int config[3]; // [0]= intervalo de tempo entre amostragens, [1]= amplitude minima, [2]= amplitude maxima
char strg[10];
float data[SIZEVEC];
unsigned int value;

#endif /* GLOBAL_H_ */
