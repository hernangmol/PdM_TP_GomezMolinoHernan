/*============================================================================
 * Programación de microcontroladores
 * Trabajo práctico
 * Archivo: miApp_UART.h
 * Fecha 18/08/2021
 * Alumno: Hernán Gomez Molino
 *===========================================================================*/

#ifndef MIS_PROGS_TP_PDM_INC_MIAPP_UART_H_
#define MIS_PROGS_TP_PDM_INC_MIAPP_UART_H_

// declaración de tipos de datos públicos
//---------------------------------------------------------------
typedef enum
{
	TERMOMETRO,
	CALEF_OFF,
	CALEF_ON,
	REFRIG_OFF,
	REFRIG_ON
} modo_t;

void UART_clearScreen(void);
void UART_printHeader(void);
int UART_consRefresh(modo_t , int, int, int );
void UART_getCmd (modo_t*,int*,int*);

#endif /* MIS_PROGS_TP_PDM_INC_MIAPP_UART_H_ */
