/*
 * hand1.c
 *
 *  Created on: Sep 29, 2025
 *      Author: Cellah_SW
 */

#include "hand1.h"

HD1_T m_hd1;
extern uint32_t adcChNum,adcChBuff[10];

void Main_Tx_4Data(int cmd, int indData1, int indData2, int indData3, int indData4)
{
	printf("[%d,%d,%d,%d,%d]\r\n",cmd, indData1, indData2, indData3, indData4);
}

void Main_Tx_1Data(int cmd, int indData)
{
	printf("[%d,%d]\r\n",cmd, indData);
}

void HP1_Cmd_Config()
{
	static uint32_t timeStamp;

	if(HAL_GetTick()-timeStamp >= 500)
	{

		timeStamp = HAL_GetTick();

		Main_Tx_4Data(CMD_HP1_ADD, m_hd1.mode, m_hd1.pwmDuty, adcChBuff[adcChNum], m_hd1.catridgeStatus);
	}
}


void UartRx1DataProcess()
{
	int rxCmd = m_uart1.rxCmdAdd;
	int rxData = m_uart1.rxCmdData;
	int rxCmdIdx;
	uint16_t valueTd, valueWatt;
	if(rxCmd !=0)
	{
		switch (rxCmd)
		{
			case CMD_HP1_ADD:
				switch (rxData)
				{
					case CMD_HP1_COOL_MAX:
						m_hd1.step = STEP1;
					break;

					case CMD_HP1_COOL_CTRL:
						m_hd1.step = STEP2;
					break;

					case CMD_HP1_COOL_OFF:
						m_hd1.step = STEP3;
					break;
				}
			break;

			case CMD_HP1_CART_ID:
				if(rxData == REQ_DATA)
				{
					Main_Tx_1Data(CMD_HP1_CART_ID, m_hd1.catridgeId);
				}
				else
				{
					m_hd1.catridgeId = rxData;
					Eeprom_Byte_Write(IDX_HP1_CART_ID_START, rxData);
				}
			break;

			case CMD_HP1_MANUFAC_YY:
				m_hd1.manufacYY = rxData;
				Eeprom_Word_Write(IDX_HP1_MANUFAC_YY_START, rxData);
			break;

			case CMD_HP1_MANUFAC_MMDD:
				m_hd1.manufacMMDD = rxData;
				Eeprom_Word_Write(IDX_HP1_MANUFAC_MMDD_START, rxData);
			break;

			case CMD_HP1_ISSUED_YY:
				m_hd1.issuedYY = rxData;
				Eeprom_Word_Write(IDX_HP1_ISSUED_YY_START, rxData);
			break;

			case CMD_HP1_ISSUED_MMDD:
				m_hd1.issuedMMDD = rxData;
				Eeprom_Word_Write(IDX_HP1_ISSUED_MMDD_START, rxData);
			break;

			case CMD_HP1_DAY_REQ:
				Main_Tx_4Data(CMD_HP1_DAY_REQ, m_hd1.manufacYY, m_hd1.manufacMMDD, m_hd1.issuedYY, m_hd1.issuedMMDD);
			break;

			case CMD_HP1_REMIND_SHOT:
				if(rxData == REQ_DATA)
				{
					Main_Tx_1Data(CMD_HP1_REMIND_SHOT, m_hd1.remainingShotNum);
				}
				else
				{
					m_hd1.remainingShotNum = rxData;
					Eeprom_Word_Write(IDX_HP1_REMIND_SHOT_START, rxData);

				}
			break;

			case CMD_HP1_CATRIDGE_STATUS:
				if(rxData == REQ_DATA)
				{
					Main_Tx_1Data(CMD_HP1_CATRIDGE_STATUS, m_hd1.catridgeStatus);
				}
				else
				{
					m_hd1.catridgeStatus = rxData;
					Eeprom_Byte_Write(IDX_CATRIDGE_STATUS_START, rxData);


				}
			break;
			case CMD_HP1_FRQ_REQ:
				for(int i =1 ;i <= 7;i++)
				{
					Main_Tx_1Data(i+CMD_TRANDU_FRQ_BASE, m_hd1.rfFrqBuff[i]);
					//HAL_Delay(20);
				}
			break;

			case CMD_HP1_WATT_REQ:
				for(int i =1 ;i <= 77;i++)
				{
					Main_Tx_1Data(i+CMD_TRANDU_WATT_BASE, m_hd1.rfWattBuff[i]);
					//HAL_Delay(10);
				}
			break;

			default:
				if(CMD_TRANDU1_FRQ <= rxCmd && rxCmd <=CMD_TRANDU7_FRQ)
				{
					m_hd1.rfFrqBuff[rxCmd-CMD_TRANDU_FRQ_BASE] = rxData;
					rxCmdIdx = (rxCmd-CMD_TRANDU_FRQ_BASE-1)*2 +IDX_HP1_FRQ_BUFF_START;
					Eeprom_Word_Write(rxCmdIdx, rxData);

				}
				else if(CMD_TRANDU1_WATT10 <= rxCmd && rxCmd <CMD_TRANDU7_WATT005)
				{
					valueTd = (rxCmd -CMD_TRANDU_WATT_BASE -1)/11 +1;
					valueWatt = (rxCmd -CMD_TRANDU_WATT_BASE -1)%11 +1;
					m_hd1.rfWattBuff[valueTd][valueWatt] = rxData;
					rxCmdIdx = (rxCmd-CMD_TRANDU_WATT_BASE)+22;
					Eeprom_Word_Write(rxCmdIdx, rxData);
				}

			break;


		}

		m_uart1.rxCmdAdd = 0;
		m_uart1.rxCmdData = 0;
	}
}

void HP1_Temp_Duty_Ctrl()
{
	switch (m_hd1.step)
	{
		case STEP0:

		break;

		case STEP1:

			Pwm_DutySet_Tim1_CH4(10000);
			m_hd1.pwmDuty = 100;
			m_hd1.step = STEP0;
			m_hd1.mode = STEP1;

		break;

		case STEP2:
			if(adcChBuff[0] <= 68)
			{
				Pwm_DutySet_Tim1_CH4(0);
				m_hd1.pwmDuty = 0;
			}
			else if(adcChBuff[0] == 69)
			{
				Pwm_DutySet_Tim1_CH4(0);
				m_hd1.pwmDuty = 0;
			}
			else if(adcChBuff[0] == 70)
			{
				Pwm_DutySet_Tim1_CH4(1000);
				m_hd1.pwmDuty = 10;
			}
			else if(adcChBuff[0] == 71)
			{
				Pwm_DutySet_Tim1_CH4(5000);
				m_hd1.pwmDuty = 50;
			}
			else if(adcChBuff[0] >= 72)
			{
				Pwm_DutySet_Tim1_CH4(10000);
				m_hd1.pwmDuty = 100;
			}

			m_hd1.mode = STEP2;
		break;

		case STEP3:
			Pwm_DutySet_Tim1_CH4(0);
			m_hd1.pwmDuty = 0;
			m_hd1.step = STEP0;
			m_hd1.mode = STEP3;
		break;
	}

}


