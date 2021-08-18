/*============================================================================
 * Programación de microcontroladores
 * Trabajo práctico
 * Archivo: TP_PdM.c
 * Fecha 18/08/2021
 * Alumno: Hernán Gomez Molino
 *===========================================================================*/

/*=====[Inclusión de dependencias]===========================================*/

#include "TP_PdM.h"
#include "sapi.h"
#include "SMTB_oneWire.h"
#include "miApp_UART.h"


int main( void )
{
/*=====[Declaración e Inicialización de variables locales]===================*/

	delay_t hDelay;
	OWbus_t* punteroBus = NULL;
	modo_t modo = TERMOMETRO;
	modo_t *modo_p= &modo;
	int tempDisp1 = 20;
	int *tempDisp1_p = &tempDisp1;
	int tempDisp2 = 30;
	int *tempDisp2_p = &tempDisp2;
	int temp = 0;

/*=====[Bloque de ejecución por única vez]===================================*/

	boardInit();
	uartInit( UART_USB, 115200);
	gpioInit( LEDR, GPIO_OUTPUT );	//					 Led rojo indica ERROR!
	gpioInit( LEDG, GPIO_OUTPUT );	// Led verde indica comunicación con sensor
	punteroBus = OWinit(GPIO2);     //      	   Se inicializa el bus OneWire
	if(punteroBus == NULL)			//		  chequeo de GPIO habilitado en LUT
	{
		gpioWrite( LEDR, true);		//					Led RGB en rojo= ERROR!
		while(TRUE);				//					   bloqueo del programa
	}
	delayInit( &hDelay, 1000 );     //	  ciclo de refresco de consola en 1 seg
	UART_clearScreen();             //		              Se limpia la pantalla
    UART_printHeader();             //		           Se imprime el encabezado

/*=====[Bloque de ejecución cíclica]=========================================*/

	while( true )
	{
		if( delayRead( &hDelay ) )
		{
			temp = OWreadTemperature(punteroBus->OWport,punteroBus->OWpin);
									//					 Lectura de temperatura
			if (UART_consRefresh(modo, temp, tempDisp1, tempDisp2))
					printf("ERROR!\n\r");
									//					   Refresco de pantalla
		}
		UART_getCmd(modo_p,tempDisp1_p,tempDisp2_p);
									//			   Polling de comandos por UART
		switch(*modo_p)				//     Se actualizan las salidas según modo
		{
			case TERMOMETRO:
				gpioWrite( LED2, OFF );
				gpioWrite( LEDB, OFF );
				break;

			case CALEF_OFF:
				if(temp <= tempDisp1)
				{
					gpioWrite( LED2, ON );
					gpioWrite( LEDB, OFF );
					modo = CALEF_ON;
				}
				break;

			case CALEF_ON:
				if(temp >= tempDisp2)
				{
					gpioWrite( LED2, OFF );
					gpioWrite( LEDB, OFF );
					modo = CALEF_OFF;
				}
				break;

			case REFRIG_OFF:
				if(temp >= tempDisp2)
				{
					gpioWrite( LED2, OFF );
					gpioWrite( LEDB, ON );
					modo = REFRIG_ON;
				}
				break;

			case REFRIG_ON:
				if(temp <= tempDisp1)
				{
					gpioWrite( LED2, OFF );
					gpioWrite( LEDB, OFF );
					modo = REFRIG_OFF;
				}
				break;
			default:                //           Los estados inválidos resetean
				modo = TERMOMETRO;
		}


    }
   return 0;
}


