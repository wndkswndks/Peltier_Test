#ifndef CMD_H
#define CMD_H

/*  			include start  			*/
#include "common.h"
/*  			include end  			*/



/*  			define start  			*/
#define RX_BUFF_SIZE	30//
#define TX_BUFF_SIZE	30




/*  			define end  			*/


/*  			enum start  			*/

/*  			enum end  				*/



/*  			stuct start  			*/
typedef struct
{
	uint8_t rxBuff[RX_BUFF_SIZE];
	uint8_t txBuff[TX_BUFF_SIZE];
	uint8_t rxViewBuff[RX_BUFF_SIZE];
	uint16_t rxViewCnt;
	uint16_t rxCnt;
	int rxCmdAdd;
	int rxCmdData;
	uint8_t clearFlag1;
	uint8_t clearFlag2;
	uint8_t txFlag;
	uint8_t startFlag;
	uint8_t endFlag;
	uint8_t rs485En;//ï¿½Ê¿ï¿½ï¿?
} UART_T;



/*  			stuct end  				*/



/*  			function start  		*/
void TxTest();


/*  			function end  			*/
void Uart_Gulobal();
void Uart_Init();


/*  			extern start  			*/


extern UART_T m_uart1;

/*  			extern end  			*/

#endif
//remocon.h


