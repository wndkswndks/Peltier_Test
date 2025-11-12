/*
 * eeprom.c
 *
 *  Created on: Sep 11, 2025
 *      Author: Cellah_SW
 */
#include "eeprom.h"


EEPROM_T m_eep;

static inline uint8_t CAT24C16_MemAddr8(uint16_t abs_addr)
{
    return (uint8_t)(abs_addr & 0xFF);          // 블록 내 8비트 주소
}

static HAL_StatusTypeDef CAT24C16_WaitReady(I2C_HandleTypeDef *hi2c, uint16_t abs_addr, uint32_t timeout_ms)
{
    uint32_t tick_start = HAL_GetTick();
    uint16_t dev =CAT24C16_ADDRESS_ID;
    while ((HAL_GetTick() - tick_start) < timeout_ms)
    {
        if (HAL_I2C_IsDeviceReady(hi2c, dev, 1, 5) == HAL_OK)
            return HAL_OK;
    }
    return HAL_TIMEOUT;
}

HAL_StatusTypeDef CAT24C16_WriteByte(I2C_HandleTypeDef *hi2c, uint16_t abs_addr, uint8_t data)
{
    if (abs_addr >= CAT24C16_TOTAL_BYTES) return HAL_ERROR;

    uint16_t dev  = CAT24C16_ADDRESS_ID;
    uint8_t  maddr= CAT24C16_MemAddr8(abs_addr);

    HAL_StatusTypeDef st = HAL_I2C_Mem_Write(hi2c, dev, maddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    if (st != HAL_OK) return st;

    // 내부 쓰기 완료 대기(ACK 폴링)
    return CAT24C16_WaitReady(hi2c, abs_addr, 10 /*ms*/);
}

HAL_StatusTypeDef CAT24C16_ReadByte(I2C_HandleTypeDef *hi2c, uint16_t abs_addr, uint8_t *p_data)
{
    if (abs_addr >= CAT24C16_TOTAL_BYTES || p_data == NULL) return HAL_ERROR;

    uint16_t dev  = CAT24C16_ADDRESS_ID;
    uint8_t  maddr= CAT24C16_MemAddr8(abs_addr);

    return HAL_I2C_Mem_Read(hi2c, dev, maddr, I2C_MEMADD_SIZE_8BIT, p_data, 1, 100);
}



uint8_t bufQQ[255] = {0};
void eeprom_test(void)
{
    // 1) 0x000 ~ 0x00A에 패턴 기록
//    for (uint16_t i = 0; i <= 250; ++i)
//    {
//        uint8_t w = i;  // 임의 패턴
//        if (CAT24C16_WriteByte(&hi2c1, i, w) != HAL_OK)
//        {
//            // 에러 처리
//            return;
//        }
//    }

    // 2) 같은 구간 읽기
    for (int i = 0; i <= 250; ++i)
    {
        if (CAT24C16_ReadByte(&hi2c1, i, &bufQQ[i]) != HAL_OK)
        {
            // 에러 처리
            return;
        }
    }


}

void Eeprom_All_Read(void)
{
    // 2) 같은 구간 읽기
    for (int i = 0; i <= 200; ++i)
    {
        if (CAT24C16_ReadByte(&hi2c1, i, &m_eep.buff[i]) != HAL_OK)
        {
            // 에러 처리
            return;
        }
    }

	if(m_eep.buff[IDX_HP1_IS_FLASH_FIRST] != FLASHA_FIRST_FLAG)
	{
		m_hd1.flashFirst = FLASHA_FIRST_FLAG;
		Eeprom_Byte_Write(IDX_HP1_IS_FLASH_FIRST, m_hd1.flashFirst);

	    for (uint16_t i = 1; i < 200; ++i)
	    {
	        if (CAT24C16_WriteByte(&hi2c1,i, 0) != HAL_OK)
	        {
	            // 에러 처리
	            return;
	        }
	    }

	}
	else
	{
		m_hd1.flashFirst = FLASHA_COMPLITE_FLAG;
		m_hd1.catridgeId = m_eep.buff[IDX_HP1_CART_ID_START];
		m_hd1.manufacYY = m_eep.buff[IDX_HP1_MANUFAC_YY_START];
		m_hd1.manufacMM = m_eep.buff[IDX_HP1_MANUFAC_MM_START];
		m_hd1.manufacDD = m_eep.buff[IDX_HP1_MANUFAC_DD_START];
		m_hd1.issuedYY = m_eep.buff[IDX_HP1_ISSUED_YY_START];
		m_hd1.issuedMM = m_eep.buff[IDX_HP1_ISSUED_MM_START];
		m_hd1.issuedDD = m_eep.buff[IDX_HP1_ISSUED_DD_START];
		for(int i =0 ;i < 7; i++)
		{
			m_hd1.rfFrqBuff[i+1] = m_eep.buff[i*2+IDX_HP1_FRQ_BUFF_START]<<8|m_eep.buff[i*2+1+IDX_HP1_FRQ_BUFF_START];
		}
		for(int i =0 ;i < 77; i++)
		{
			m_hd1.rfWattBuff[i+1] = m_eep.buff[i*2+IDX_HP1_WATT_BUFF_START]<<8|m_eep.buff[i*2+1+IDX_HP1_WATT_BUFF_START];
		}
		m_hd1.remainingShotNum = m_eep.buff[IDX_HP1_REMIND_SHOT_START]<<8|m_eep.buff[IDX_HP1_REMIND_SHOT_END];
		m_hd1.catridgeStatus = m_eep.buff[IDX_CATRIDGE_STATUS_START];
	}



}

void Eeprom_All_Save(void)
{

	if(m_eep.saveFlag)
	{
		m_eep.saveFlag = 0;
	    for (uint16_t i = 0; i < 200; ++i)
	    {
	        if (CAT24C16_WriteByte(&hi2c1,i, m_eep.buff[i]) != HAL_OK)
	        {
	            // 에러 처리
	            return;
	        }
	    }




	}
}
void Eeprom_Byte_Write(uint8_t Idx, uint8_t data)
{
	if(Idx >= MAX_EEPROM_SIZE) return;
	m_eep.buff[Idx] = (uint8_t)data;
	CAT24C16_WriteByte(&hi2c1, Idx, data);
}

void Eeprom_Word_Write(uint8_t startIdx, uint16_t data)
{
	uint8_t lsb,msb;
	if(startIdx >= MAX_EEPROM_SIZE-1) return;

	msb = (data>>8)&0xff;
	lsb = (data)&0xff;
	m_eep.buff[startIdx] = msb;
	m_eep.buff[startIdx+1] = lsb;
	CAT24C16_WriteByte(&hi2c1, startIdx, msb);
	CAT24C16_WriteByte(&hi2c1, startIdx+1, lsb);
}


