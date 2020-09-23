/*
 * medidas.h
 *
 *  Created on: Dec 1, 2016
 *      Author: ea871
 */

#include "derivative.h"
#include "global.h"
#include <math.h>

#ifndef MEDIDAS_H_
#define MEDIDAS_H_

/*!
 * \fn maximo ()
 * \brief retorna valor maximo do sinal.
 */
float maximo();

/*!
 * \fn minimo ()
 * \brief retorna valor minimo sinal.
 */
float minimo();

/*!
 * \fn pico_pico ()
 * \brief retorna valor pico_pico do sinal.
 */
float pico_pico();

/*!
 * \fn medio ()
 * \brief retorna valor medio do sinal.
 */
float medio();

/*!
 * \fn eficaz ()
 * \brief retorna valor eficaz do sinal.
 */
float eficaz();

#endif /* MEDIDAS_H_ */
