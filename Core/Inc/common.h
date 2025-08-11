//common.h
#ifndef COMMON_H
#define COMMON_H
//wndkswnkds
/*  			include start  			*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>//

#include "main.h"
#include "usart.h"
//#include "debug.h"
#include "cmd.h"
//#include "peripheral.h"

#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "TestMy.h"
//#include "can.h"

//#include "gpio.h"



/*  			include end  			*/



/*  			define start  			*/
#define ERR_FUC_LINE() send_debug_info_uart(__FUNCTION__, __LINE__)

/*  			define end  			*/


/*  			enum start  			*/
typedef enum
{
	STEP0,
	STEP1,
	STEP2,
	STEP3,
	STEP4,
	STEP5,
	STEP6,
	STEP7,
	STEP8,
	STEP9,
	STEP10,
	STEP11,
	STEP12,
	STEP13,
	STEP14,
	STEP15,
	STEP16,
	STEP17,
	STEP18,
	STEP19,
	STEP20,
	STEP21,
	STEP22,
	STEP23,
	STEP24,
	STEP25,
	STEP26,
	STEP27,
	STEP28,
	STEP29,
	STEP30,
	STEP31,
	STEP32,
	STEP33,
	STEP34,
	STEP35,
	STEP36,
	STEP37,
	STEP38,
	STEP39,
	STEP40,

} STEP_E;
typedef enum
{
	NONE_OK,
	NOT_YET = 0,
	FAIL = 0,
	ERR = 0,
	OK = 1,
} OK_E;
/*  			enum end  				*/


typedef enum
{

	RELEASE	= 0,
	PUSH	= 1,

	NONE_MODE	= 0,
	SHOTKEY_MODE = 1,
	LONGKEY_MODE = 2,

	MINSHOT_TIME	= 1,
	MAXSHOT_TIME	= 10,
	LONG_TIME		= 12,

	KEY_NONE = 0,
	KEY_SHOT1,
	KEY_SHOT2,
	KEY_SHOT3,
	KEY_SHOT4,

	KEY_LONG1,
	KEY_LONG2,
	KEY_LONG3,
	KEY_LONG4,


	KEY_READ_UP  = 1,
	KEY_READ_DN  = 2,
	KEY_READ_POWER  = 4,
	KEY_READ_LAMP  = 8,
} CTRL_KEY_E;

typedef enum
{
	BUFF_SIZE =   10,
} BUFF_E;

/*  			stuct start  			*/

typedef struct
{
	uint8_t buff[BUFF_SIZE];
	uint16_t cnt;
} BUFF_T;


typedef struct
{
	uint8_t status;
	uint32_t timeStamp;
	uint8_t cnt;
	uint8_t shotCnt;
	uint8_t longCnt;
	uint8_t mode;
	uint8_t initOk;

	uint8_t myKey;
} BUTTON_T;



/*  			stuct end  				*/



/*  			function start  		*/


/*  			function end  			*/


/*  			extern start  			*/



/*  			extern end  			*/

#endif


