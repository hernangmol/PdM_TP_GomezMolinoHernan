# PdM_TP_GomezMolinoHernan

Trabajo práctico de la materia Programación de Microcontroladores

Alumno: Hernán Gomez Molino

Plataforma embebida: EDU-CIAA-NXP


Aplicación:

Se trata de un termostato con histéresis que tiene tres modos de funcionamiento:

. Modo termómetro: Solo informa la temperatura. Las salidas permanecen inactivas.

. Modo calefacción: Informa la temperatura y comanda una salida (led rojo) según los parámetros seteados para activar un elemento calefactor.

. Modo refrigeración: Informa la temperatura y comanda una salida (led azul) según los parámetros seteados para activar un ventilador.

La medición de temperatura se hace con termómetro digital DS18B20 utilizando el protocolo OneWire.
Se utiliza una UART para recibir el seteo de modo y temperaturas de disparo (ver diagrama), y enviar la temperatura medida a una consola.

Periféricos:

. Termómetro DS18B20

. UART


Breve descripción de cada estado:

. Termómetro: informa la temperatura cíclicamente por consola, hasta que recibe un pedido de cambio de modo por consola. Es el estado inicial de la MEF y retorna a este si se recibe pedido de cambio a modo OFF (M->OFF).

. Calef_OFF: ver próximo

. Calef_ON: estos dos estados corresponden al modo calefacción y alternará entre estos dependiendo de la temperatura medida y los seteos de temperatura 1 y 2.  

. Refrig_OFF: ver próximo

. Refrig_ON: estos dos estados corresponden al modo refrigeración y alternará entre estos dependiendo de la temperatura medida y los seteos de temperatura 1 y 2.  
Módulos de software:

SMTB_oneWire.c y SMTB_oneWire.h para el sensor DS18B20.

miApp_UART.c  y miApp_UART.h para el envío de temperatura y recepción de los comandos.


Prototipos de las principales funciones públicas y privadas de cada módulo definido SMTB_oneWire.c

typedef struct OWbus_t{    gpioMap_t OWgpio;  int OWport;  int OWpin;} OWbus_t;

OWbus_t* OWinit (gpioMap_t) //Inicializa el bus One Wire.

uint8_t OWpresence(uint8_t port,uint8_t pin) //detecta presencia del sensor.

void OWcommand(int command) // envia los comandos propios del sensor.

void OWreadScratch() // lee el area de memoria RAm del sensor.

void OWdelay(uint32_t delay-us) // genera retardos del orden de microsegundos.

miApp_UART.c

typedef enum{TERMOMETRO, CALEF_OFF,  CALEF_ON, REFRIG_OFF, REFRIG_ON} modo_t;

void UART_consRefresh (modo_t modo, uint8_t tempActual, uint8_t tempDisp1,uint8_t tempDisp2);// Muestra ciclicamente la temperatura actual, el modo actual, el seteo de los disparo y el estado de las salidas.

void UART_getCmd(modo_t * modReq,uint8_t tempDisp1,uint8_t tempDisp2); //Recibe los seteos de modo y temperaturas de disparo.
