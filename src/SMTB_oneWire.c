/*============================================================================
 * Programación de microcontroladores
 * Trabajo práctico
 * Archivo: SMTB_oneWire.c
 * Fecha 18/08/2021
 * Alumno: Hernán Gomez Molino
 *===========================================================================*/

#include "SMTB_oneWire.h"
#include "sapi.h"

/*=====[Declaración de (function like) macros]===============================*/

#define OWsetOut(port,pin)	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, port, pin)
#define OWsetIn(port,pin)	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, port, pin)
#define OWread(port,pin)	Chip_GPIO_GetPinState(LPC_GPIO_PORT, port, pin)
#define OWlow(port,pin)		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, port, pin)
#define OWhigh(port,pin)	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, port, pin)

/*=====[Declaración de funciones privadas]===================================*/

static uint8_t OWcrc(uint8_t* code, uint8_t n);
static void OWdelay_uS(uint32_t );

/*=============================================================================
* FUNCIÓN: OWinit
* Que hace: Crea una struct OWbus_t y la carga con el GPIO, port y pin
* asociados, setea el modo del GPIO y los registros DWT y DEMCR para la
* temporización que usará oneWire.
* PARÁMETROS:
* Que recibe: gpioMap_t GPIO_OW, el GPIO que usará oneWire
* Que devuelve: OWbus_t* punt, puntero a OWbus_t con los datos del bus oneWire
* Variables externas que modifca: N/A
*============================================================================*/

OWbus_t* OWinit(gpioMap_t GPIO_OW)
{
	uint32_t * H_DWT_DEMCR	 = (uint32_t *)0xE000EDFC;
	uint32_t * H_DWT_CTRL	 = (uint32_t *)0xE0001000;

	static OWbus_t bus;
	static OWbus_t* punt = &bus;

	bus.OWgpio = GPIO_OW;
	switch(GPIO_OW)			  //				  Look up table GPIO/ port,pin
		{
		case GPIO0:
			bus.OWport = 3;
			bus.OWpin = 0;
			break;
		case GPIO1:
			bus.OWport = 3;
			bus.OWpin = 3;
			break;
		case GPIO2:
			bus.OWport = 3;
			bus.OWpin = 4;
			break;
		default:
			punt = NULL;
			return (punt);
		}

	Chip_SCU_PinMux 		  //                  seteo de modo de pin oneWire
	(bus.OWport,bus.OWpin,SCU_MODE_INACT
	| SCU_MODE_ZIF_DIS, SCU_MODE_FUNC0 );
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, bus.OWport, bus.OWpin);
	*H_DWT_DEMCR |= 1<<24;    //      bit24[TRCENA]   = habilita todos los DWT
	*H_DWT_CTRL |= 1;	      //              bit0[CYCCNTENA] =  enable CYCCNT
	return (punt);
}

/*=============================================================================
* FUNCIÓN: OWpresence
* Que hace: Chequea la presencia de sensores (esclavos) en el bus oneWire.
* PARÁMETROS:
* Que recibe: int port, int pin que usa oneWire.
* Que devuelve: -1 si no hay sensores, 0 si hay sensores.
* Variables externas que modifca: N/A
*============================================================================*/

int OWpresence(int port, int pin)
{
	OWsetOut(port,pin);				// 					   envía pulso de reset
	OWhigh(port,pin);
	OWdelay_uS(1000);
	OWlow(port,pin);
	OWdelay_uS(480);
	OWsetIn(port,pin);
	OWdelay_uS(40);					//						 lee el bus oneWire
	if(OWread(port,pin)==true)
		{
		return -1;
		}
	else
		{
		return 0;
		}
}

/*=============================================================================
* FUNCIÓN: OWcommand
* Que hace: Escribe en el bus OW la secuencia de bits de un comando, lee la
* respuesta y la deja en el buffer.
* PARÁMETROS:
* Que recibe: uint8_t cmd, comando para el DS18B29
* 			  uint8_t * buffer, dirección del buffer para guardar respuesta
* 			  uint8_t n tamaño del buffer
* 			  int port, int pin puerto y pin del bus oneWire
* Que devuelve: N/A
* Variables externas que modifca: el buffer apuntado por el segundo parámetro
*============================================================================*/

void OWcommand(uint8_t cmd, uint8_t * buffer, uint8_t n, int port, int pin)
{
	volatile uint8_t i = 1, j;
	volatile uint8_t * p = (uint8_t *)buffer;

	OWsetOut(port,pin);
	do
	{
		if(cmd & i)					//			    		  si la máscara = 1
		{
			OWlow(port,pin);
			OWdelay_uS(3);
			OWhigh(port,pin);
			OWdelay_uS(60);
		}
		else						//			    		  si la máscara = 0
		{
			OWlow(port,pin);
			OWdelay_uS(60);
			OWhigh(port,pin);
			OWdelay_uS(10);
		}
		if(i==0x80)					//		     chequea si llegó al último bit
		{
			break;
		}
		else
		{
			i <<= 1;				//	  corrimiento a izquierda de la máscara
		}
	}while(i != 0);

	for(i=0; i<n; i++)				// lectura de un byte(8-> ROM, 9-> scratch)
	{
		p[i] = 0;
		for(j=0; j<8; j++)			//						lectura de a un bit
		{
			OWsetOut(port,pin);
			OWlow(port,pin);
			OWdelay_uS(3);
			OWsetIn(port,pin);
			OWdelay_uS(12);
			p[i] >>= 1;
			if(OWread(port,pin)) p[i] |= 0x80;
			OWdelay_uS(55);
		}
	}
}

/*=============================================================================
* FUNCIÓN: OWreadScratch
* Que hace: Lee el scratchpad.
* PARÁMETROS:
* Que recibe: uint8_t * buffer9, dirección del buffer para guardar respuesta
* 			  int port, int pin puerto y pin del bus oneWire
* Que devuelve:  -1 si no hay lectura, 0 si hay lectura.
* Variables externas que modifca: N/A
*============================================================================*/

int OWreadScratch(uint8_t * buffer9, int port, int pin)
{
	int rv = -1;
	uint8_t crc = 0;
	uint8_t * p = buffer9;

	if(OWpresence(port, pin)==0)
	{
		OWdelay_uS(400);
		__set_PRIMASK(1);					 //		 deshabilita interrupciones
		OWcommand(0x33, p, 8, port, pin);	 //  			   comando Skip ROM
		OWcommand(0xBE, p, 9, port, pin);	 //			   comando Read scratch
		__set_PRIMASK(0);					 //			habilita interrupciones
		crc = OWcrc(p, 8);					 //                     chequea CRC
		if(crc == p[8])
		{
			rv = 0;
		}
	}
	return rv;
}

/*=============================================================================
* FUNCIÓN: OWdelay_uS
* Que hace: Hace un delay bloqueante del orden de uS.
* PARÁMETROS:
* Que recibe: uint32_t t, delay en uS.
* Que devuelve: N/A
* Variables externas que modifca: N/A
*============================================================================*/

static void OWdelay_uS(uint32_t t)
{
	static volatile uint32_t * H_DWT_CYCCNT	 = (uint32_t *)0xE0001004;

	*H_DWT_CYCCNT = 0;				//		   carga el contador de ciclos en 0
	t *= (SystemCoreClock/1000000); //					 carga los uS a esperar
	while(*H_DWT_CYCCNT < t);		// chequea si el contador alcanzó la cuenta
}

/*=============================================================================
* FUNCIÓN: OWreadTemperature
* Que hace: Lee la temperatura del sensor.
* PARÁMETROS:
* Que recibe: int port, int pin puerto y pin del bus oneWire
* Que devuelve: la temperatura en grados, sin decimales (int)
*============================================================================*/

int OWreadTemperature(int port, int pin)
{
	volatile int buffTemp = -1;
	volatile uint8_t crc = 0;
	uint8_t p[9];

	if(OWpresence(port,pin)==0)
	{
		gpioWrite( LEDG, true);
		OWdelay_uS(400);
		__set_PRIMASK(1); 					//		 deshabilita interrupciones
		OWcommand(0x33, p, 8, port, pin); 	//  			   comando Read ROM
		OWcommand(0x44, p, 0, port, pin); 	//		   comando Start convertion
		__set_PRIMASK(0); 					//			habilita interrupciones
		OWsetIn(port,pin);
		while(OWread(port,pin) == false);	// 		espera el fin de conversión
		OWpresence(port, pin);
		OWdelay_uS(400);
		__set_PRIMASK(1);					//		 deshabilita interrupciones
		OWcommand(0x33, p, 8, port, pin); 	//  			   comando Read ROM
		OWcommand(0xBE, p, 9, port, pin);	//			   comando Read scratch
		__set_PRIMASK(0);					//			habilita interrupciones
		crc = OWcrc(p, 8);					//                      chequea CRC
		if(crc == p[8])
		{
			buffTemp = p[1];
			buffTemp <<= 8;
			buffTemp |= p[0];
			buffTemp = buffTemp >> 4;
		}
	}
	gpioWrite(LEDG,FALSE);
	return buffTemp;
}

/*=============================================================================
* FUNCIÓN: OWreadROM
* Que hace: Lee la dirección del dispositivo oneWire (ROM).
* PARÁMETROS:
* Que recibe: uint8_t * buffer8, dirección del buffer para guardar respuesta
* 			  int port, int pin puerto y pin del bus oneWire
* Que devuelve:  -1 si no hay lectura, 0 si hay lectura.
* Variables externas que modifca: N/A
*============================================================================*/

int OWreadROM(uint8_t * buffer8, int port, int pin)
{
	int rv = -1;
	uint8_t crc = 0;
	uint8_t *p = buffer8;

	if(OWpresence(port, pin)==0)
	{
		OWdelay_uS(400);
		__set_PRIMASK(1);					//     	 deshabilita interrupciones
		OWcommand(0x33, p, 8, port, pin);	//             	   comando Read ROM
		__set_PRIMASK(0);					//			habilita interrupciones
		crc = OWcrc(p, 7);					//                      chequea CRC
		if(crc == p[7])
		{
			rv = 0;
		}
	}
	return rv;
}

/*=============================================================================
* FUNCIÓN: OWcrc
* Que hace: Calcula el CRC para comparar con el enviado por el dispositivo.
* PARÁMETROS:
* Que recibe: uint8_t* code, puntero a la copia de la ROM o scratchpad
* 			  uint8_t n, longitud (7 para ROM, 8 para scratchpad)
* Que devuelve: el CRC calculado (uint8_t)
* Variables externas que modifca: N/A
*============================================================================*/

static uint8_t OWcrc(uint8_t* code, uint8_t n)
{
	uint8_t crc=0, inbyte, i, mix;

	while(n--)							//				      recorre cada byte
	{
		inbyte = *code++;
		for(i=8; i; i--)				//					  recorre bit a bit
		{
			mix= (crc ^ inbyte) & 0x01;	//					   calcula el carry
			crc >>= 1;					//		   corrimiento a derecha de CRC
			if(mix)						//						si carry es uno
			{
				crc ^= 0x8C;			//		 hace XOR bitwise bits 7, 3 y 2
			}
			inbyte >>= 1;				//			  corrimiento a derecha del
		}								// 						byte de entrada

	}
	return crc;
}
