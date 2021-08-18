/*============================================================================
 * Programación de microcontroladores
 * Trabajo práctico
 * Archivo: miApp_UART.c
 * Fecha 18/08/2021
 * Alumno: Hernán Gomez Molino
 *===========================================================================*/

#include "miApp_UART.h"
#include "sapi.h"

/*=============================================================================
* FUNCIÓN: UART_clearScreen
* Que hace: Envía la secuencia de comandos ESC para limpiar la consola.
* PARÁMETROS:
* Que recibe: N/A
* Que devuelve: N/A
* Variables externas que modifca: N/A
*============================================================================*/

void UART_clearScreen(void)
{
	uartWriteByte(   UART_USB, 27    );   // ESC command
    uartWriteString( UART_USB, "[2J" );   // Clear screen command
    uartWriteByte(   UART_USB, 27    );   // ESC command
    uartWriteString( UART_USB, "[H"  );   // Cursor to home command
}

/*=============================================================================
* FUNCIÓN: UART_printHeader
* Que hace: Imprime en pantalla el encabezado común a todos los modos.
* PARÁMETROS:
* Que recibe: N/A
* Que devuelve: N/A
* Variables externas que modifca: N/A
*============================================================================*/

void UART_printHeader(void)
{
	printf("__________________________________\n\r");
	printf("Opciones:\n\r");
	printf("(T) Termómetro\n\r");
	printf("(C) Calefacción\n\r");
	printf("(R) Refrigeración\n\r");
	printf("(Axx) Limite superior - xx números\n\r");
	printf("(Bxx) Limite inferior - xx números\n\r");
	printf("__________________________________\n\r");
	printf("Estados:\n\r");
}

/*=============================================================================
* FUNCIÓN: UART_consRefresh
* Que hace: Calcula el CRC para comparar con el enviado por el dispositivo.
* PARÁMETROS:
* Que recibe: modo_t modo, el modo en que se encuentra el sistema
* 			  int tempActual, tempDisp1 y tempDisp2; temperaturas de trabajo
* Que devuelve: N/A
* Variables externas que modifca: N/A
*============================================================================*/

int UART_consRefresh(modo_t modo, int tempActual, int tempDisp1, int tempDisp2)
{
	switch(modo)
	{
		case TERMOMETRO:
			printf("Modo:-------------------TERMOMETRO\n\r");
			printf("Calefactor:----------------APAGADO\n\r");
			printf("Ventilador:----------------APAGADO\n\r");
			printf("Límite superior:----------------%d \n\r", tempDisp2);
			printf("Límite inferior:----------------%d \n\r", tempDisp1);
			printf("Temperatura :-------------------%d \n\r", tempActual);
			uartWriteByte(UART_USB, 27);   		// 					comando ESC
			uartWriteString(UART_USB, "[6A");   // 		  Reposiciona el cursor
			return(0);
			break;


		case CALEF_OFF:
			printf("Modo:------------------CALEFACCIÓN\n\r");
			printf("Calefactor:----------------APAGADO\n\r");
			printf("Ventilador:----------------APAGADO\n\r");
			printf("Límite superior:----------------%d \n\r", tempDisp2);
			printf("Límite inferior:----------------%d \n\r", tempDisp1);
			printf("Temperatura :-------------------%d \n\r", tempActual);
			uartWriteByte(UART_USB, 27);   		// 					comando ESC
			uartWriteString(UART_USB, "[6A");   // 		  Reposiciona el cursor
			return(0);
			break;

		case CALEF_ON:
			printf("Modo:------------------CALEFACCIÓN\n\r");
			printf("Calefactor:--------------ENCENDIDO\n\r");
			printf("Ventilador:----------------APAGADO\n\r");
			printf("Límite superior:----------------%d \n\r", tempDisp2);
			printf("Límite inferior:----------------%d \n\r", tempDisp1);
			printf("Temperatura :-------------------%d \n\r", tempActual);
			uartWriteByte(UART_USB, 27);   		// 					comando ESC
			uartWriteString(UART_USB, "[6A");   // 		  Reposiciona el cursor
			return(0);
			break;

		case REFRIG_OFF:
			printf("Modo:----------------REFRIGERACIÓN\n\r");
			printf("Calefactor:----------------APAGADO\n\r");
			printf("Ventilador:----------------APAGADO\n\r");
			printf("Límite superior:----------------%d \n\r", tempDisp2);
			printf("Límite inferior:----------------%d \n\r", tempDisp1);
			printf("Temperatura :-------------------%d \n\r", tempActual);
			uartWriteByte(UART_USB, 27);   		// 					comando ESC
			uartWriteString(UART_USB, "[6A");   // 		  Reposiciona el cursor
			return(0);
			break;

		case REFRIG_ON:
			printf("Modo:----------------REFRIGERACIÓN\n\r");
			printf("Calefactor:----------------APAGADO\n\r");
			printf("Ventilador:--------------ENCENDIDO\n\r");
			printf("Límite superior:----------------%d \n\r", tempDisp2);
			printf("Límite inferior:----------------%d \n\r", tempDisp1);
			printf("Temperatura :-------------------%d \n\r", tempActual);
			uartWriteByte(UART_USB, 27);   		// 					comando ESC
			uartWriteString(UART_USB, "[6A");   // 		  Reposiciona el cursor
			return(0);
			break;

		default:
			return(-1);
	}
}

/*=============================================================================
* FUNCIÓN: UART_getCmd
* Que hace: Recibe comandos por consola y actualiza las variables de control.
* PARÁMETROS:
* Que recibe: modo_t* modo_punt, dirección de la variable modo.
* 			  int* disp1_punt y disp2_punt; direcciones de las temperaturas de
* 			  disparo.
* Que devuelve: N/A
* Variables externas que modifca: variable modo (modo_t), tempDisp1 y tempDisp2
* 								 (int).
*============================================================================*/
void UART_getCmd (modo_t* modo_punt,int* disp1_punt,int* disp2_punt)
{
	uint8_t bufferIn;
	static int aux = 0;
	static int inStatus = 0;
	uartMap_t uart = UART_USB;

	if(inStatus == 0)
	{	if (uartReadByte(uart, &bufferIn))
		{
			switch (bufferIn)		//					recepción de caracteres
			{						//               solo acepta T; C; R; A o B
			case 'T':				//                  (mayúscula o minúscula)
			case 't':
				*modo_punt = TERMOMETRO;
				inStatus = 0;
				break;
			case 'C':
			case 'c':
				*modo_punt = CALEF_OFF;
				inStatus = 0;
				break;
			case 'R':
			case 'r':
				*modo_punt = REFRIG_OFF;
				inStatus = 0;
				break;
			case 'B':
			case 'b':
				inStatus = 1;
				return;
				break;
			case 'A':
			case 'a':
				inStatus = 2;
				return;
				break;
			default:
				return;
			}
		}
	}
	else if(inStatus == 1)								// recepción de números
	{													//		    casos A y B
		if (uartReadByte(uart, &bufferIn))
		{
			if ((bufferIn-48)>= 0 && (bufferIn-48) <=9) //  solo acepta números
			{
				aux = 10 * (bufferIn-48);				//  decenas de temDisp1
				inStatus = 3;
			}
			return;
		}
	}
	else if(inStatus == 2)
	{
		if (uartReadByte(uart, &bufferIn))
		{
			if((bufferIn-48)>= 0 && (bufferIn-48) <=9)  //  solo acepta números
			{
				aux = 10 * (bufferIn-48);				//  decenas de temDisp2
				inStatus = 4;
			}
			return;
		}
	}
	else if(inStatus == 3)
	{
		if (uartReadByte(uart, &bufferIn))
		{
			if ((bufferIn-48)>= 0 && (bufferIn-48) <=9) //  solo acepta números
			{
				aux = aux + (bufferIn-48);				// unidades de temDisp1
				if(aux < *disp2_punt)					// chequea que temDisp1
				{										//  no quede > temDisp2
					*disp1_punt = aux;
				}
				inStatus = 0;
			}
			return;
		}
	}
	else if(inStatus == 4)
	{
		if (uartReadByte(uart, &bufferIn))
		{
			if ((bufferIn-48)>= 0 && (bufferIn-48) <=9) //  solo acepta números
			{
				aux = aux + (bufferIn-48);				// unidades de temDisp1
				if(aux > *disp1_punt)					// chequea que temDisp2
					{									//  no quede < temDisp1
						*disp2_punt = aux;
					}
				inStatus = 0;
			}
			return;
		}
	}
	return;
}
