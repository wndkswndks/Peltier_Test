/*
 * eeprom.h
 *
 *  Created on: Sep 11, 2025
 *      Author: Cellah_SW
 */
#ifndef EEPROM_H
#define EEPROM_H

/*  			include start  			*/
#include "common.h"
/*  			include end  			*/



/*  			define start  			*/
#define CAT24C16_BASE_7BIT   0x50U  // 0x50~0x57 (7-bit form)
#define CAT24C16_PAGE_BYTES  16U    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ Å©ï¿½ï¿½(ï¿½ï¿½ï¿½ï¿½ï¿?
#define CAT24C16_TOTAL_BYTES 2048U
#define CAT24C16_ADDRESS_ID   0x50U<<1

/*  			define end  			*/


/*  			enum start  			*/
typedef enum
{
	IDX_HP1_CART_ID_START = 0,
	IDX_HP1_CART_ID_END = 0,

	IDX_HP1_MANUFAC_YY_START = 1,
	IDX_HP1_MANUFAC_YY_END = 2,

	IDX_HP1_MANUFAC_MMDD_START = 3,
	IDX_HP1_MANUFAC_MMDD_END = 4,

	IDX_HP1_ISSUED_YY_START = 5,
	IDX_HP1_ISSUED_YY_END = 6,

	IDX_HP1_ISSUED_MMDD_START = 7,
	IDX_HP1_ISSUED_MMDD_END = 8,

	IDX_HP1_FRQ_BUFF_START = 9,
	IDX_HP1_FRQ_BUFF_END = 22,

	IDX_HP1_WATT_BUFF_START = 23,
	IDX_HP1_WATT_BUFF_END = 99,

	IDX_HP1_MAX_SHOT_START = 100,
	IDX_HP1_MAX_SHOT_END = 101,

	IDX_HP1_REMIND_SHOT_START = 102,
	IDX_HP1_REMIND_SHOT_END = 103,

	IDX_CATRIDGE_STATUS_START = 104,
	IDX_CATRIDGE_STATUS_END = 104,

	MAX_EEPROM_SIZE = 200,
} EEPROM_E;
/*  			enum end  				*/



/*  			stuct start  			*/
typedef struct
{
	uint8_t saveFlag;
	uint8_t buff[MAX_EEPROM_SIZE];
} EEPROM_T;
/*  			stuct end  				*/



/*  			function start  		*/

void eeprom_test(void);
void RTC_Init(void);
void RTC_Test(void);

void Eeprom_Byte_Write(uint8_t Idx, uint8_t data);
void Eeprom_Word_Write(uint8_t startIdx, uint16_t data);

/*  			function end  			*/


/*  			extern start  			*/
extern EEPROM_T m_eep;



/*  			extern end  			*/

#endif

