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
#define FLASHA_FIRST_FLAG   44
#define FLASHA_COMPLITE_FLAG   66


/*  			define end  			*/


/*  			enum start  			*/
typedef enum
{
	IDX_HP1_IS_FLASH_FIRST = 0,

	IDX_HP1_CART_ID_START = 1,
	IDX_HP1_CART_ID_END = 1,

	IDX_HP1_MANUFAC_YY_START = 2,
	IDX_HP1_MANUFAC_YY_END = 2,

	IDX_HP1_MANUFAC_MM_START = 3,
	IDX_HP1_MANUFAC_MM_END = 3,

	IDX_HP1_MANUFAC_DD_START = 4,
	IDX_HP1_MANUFAC_DD_END = 4,

	IDX_HP1_ISSUED_YY_START = 5,
	IDX_HP1_ISSUED_YY_END = 5,

	IDX_HP1_ISSUED_MM_START = 6,
	IDX_HP1_ISSUED_MM_END = 6,

	IDX_HP1_ISSUED_DD_START = 7,
	IDX_HP1_ISSUED_DD_END = 7,

	IDX_HP1_FRQ_BUFF_START = 8,
	IDX_HP1_FRQ_BUFF_END = 21,

	IDX_HP1_WATT_BUFF_START = 23,
	IDX_HP1_WATT_BUFF_END = 176,

	IDX_HP1_MAX_SHOT_START = 177,
	IDX_HP1_MAX_SHOT_END = 178,

	IDX_HP1_REMIND_SHOT_START = 179,
	IDX_HP1_REMIND_SHOT_END = 180,

	IDX_CATRIDGE_STATUS_START = 181,
	IDX_CATRIDGE_STATUS_END = 181,

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
void Eeprom_All_Read(void);


/*  			function end  			*/


/*  			extern start  			*/
extern EEPROM_T m_eep;



/*  			extern end  			*/

#endif

