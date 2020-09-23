/*
 * medidas.c
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "global.h"
#include "medidas.h"
#include <math.h>

/*!
 * \fn maximo ()
 * \brief retorna valor maximo do sinal.
 */
float maximo(){
	float val=0;
	float aux=0;
	int i;
	
	for(i=0;i<SIZEVEC;i++){
		aux = data[i];
		if(aux > val){
			val = aux;
		}
	}
return val;
}

/*!
 * \fn minimo ()
 * \brief retorna valor minimo sinal.
 */
float minimo(){
	float val=3300;
	float aux=0;
	int i;
	
	for(i=0;i<SIZEVEC;i++){
		aux = data[i];
		if(aux < val){
			val = aux;
		}
	}
return val;
}

/*!
 * \fn pico_pico ()
 * \brief retorna valor pico_pico do sinal.
 */
float pico_pico(){
	float pico, max, min;
	max = maximo();
	min = minimo();
	pico = max-min;
return pico;	
}

/*!
 * \fn medio ()
 * \brief retorna valor medio do sinal.
 */
float medio(){
	float med=0;	
	int i;
	for(i=0; i<SIZEVEC; i++){
		med = med + data[i];
	}
	float media =  med/SIZEVEC;
return media;	
}

/*!
 * \fn eficaz ()
 * \brief retorna valor eficaz do sinal.
 */
float eficaz(){
	float med=0;
	int i;
	for(i=0; i<SIZEVEC; i++){
		med = med + (data[i]*data[i]);
	}
    float media = sqrt(med/SIZEVEC);
	return media;	
}

