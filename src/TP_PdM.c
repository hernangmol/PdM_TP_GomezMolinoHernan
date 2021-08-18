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
	// Led rojo indica ERROR!
	gpioInit( LEDR, GPIO_OUTPUT );
	// Led verde indica comunicación con sensor
	gpioInit( LEDG, GPIO_OUTPUT );
	// Se inicializa el bus OneWire
	punteroBus = OWinit(GPIO2);
	// chequeo de GPIO habilitado en LUT
	if(punteroBus == NULL)
	{
		// Led RGB en rojo= ERROR!
		gpioWrite( LEDR, true);
		// bloqueo del programa
		while(TRUE);
	}
	// ciclo de refresco de consola en 1 seg
	delayInit( &hDelay, 1000 );
    // Se limpia la pantalla
	UART_clearScreen();
	 // Se imprime el encabezado
    UART_printHeader();

/*=====[Bloque de ejecución cíclica]=========================================*/

	while( true )
	{
		if( delayRead( &hDelay ) )
		{
			// Lectura de temperatura
			temp = OWreadTemperature(punteroBus->OWport,punteroBus->OWpin);
			// Refresco de pantalla
			if (UART_consRefresh(modo, temp, tempDisp1, tempDisp2))
					printf("ERROR!\n\r");
		}
		// Polling de comandos por UART
		UART_getCmd(modo_p,tempDisp1_p,tempDisp2_p);
		// Se actualizan las salidas según modo
		switch(*modo_p)
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
            // Los estados inválidos resetean
			default:
				modo = TERMOMETRO;
		}


    }
   return 0;
}


