/*============================================================================
 * Programación de microcontroladores
 * Trabajo práctico
 * Archivo: SMTB_oneWire.h
 * Fecha 18/08/2021
 * Alumno: Hernán Gomez Molino
 *===========================================================================*/


#ifndef MIS_PROGS_TP_PDM_INC_SMTB_ONEWIRE_H_
#define MIS_PROGS_TP_PDM_INC_SMTB_ONEWIRE_H_

#include "sapi.h"

/*=====[Declaración de tipos de datos públicos]==============================*/

typedef struct
{
	gpioMap_t OWgpio;
	int OWport;
	int OWpin;
} OWbus_t;

/*=====[Declaración de funciones públicas]===================================*/

OWbus_t* OWinit(gpioMap_t);
int  OWpresence(int, int);
void OWcommand(uint8_t, uint8_t *, uint8_t, int, int);
int  OWreadScratch(uint8_t *, int, int);
int  OWreadTemperature(int, int);
int  OWreadROM(uint8_t *, int, int);

#endif /* MIS_PROGS_TP_PDM_INC_SMTB_ONEWIRE_H_ */
