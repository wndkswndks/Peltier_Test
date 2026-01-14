/*
 * hand1.c
 *
 *  Created on: Sep 29, 2025
 *      Author: Cellah_SW
 */

#include "hand1.h"

HD1_T m_hd1;
extern uint32_t adcChNum,adcChBuff[11], adcChAvg;



void Main_Tx_4Data(int cmd, int data1, int data2, int data3, int data4)
{
	uint8_t len;
	uint8_t str[40];
	while(HAL_GetTick() -m_hd1.lastHPTxTime<50);
	len = sprintf(str,"[%d,%d,%d,%d,%d]\r\n",cmd, data1, data2, data3, data4);
	HAL_UART_Transmit(&huart2,str,len,100);
	m_hd1.lastHPTxTime = HAL_GetTick();
}

//#define DEBUG_PRINT

void Debug_MAIN_Printf(uint8_t rxtx, uint8_t cmd, uint16_t data)
{
#ifdef DEBUG_PRINT

	if(rxtx != DEBUG_RX && rxtx != DEBUG_TX) return;
	char rxtxStr[2][3] = {"Rx","Tx"};
	printf("[%s] MAIN [%u,%u]\r\n",rxtxStr[rxtx], cmd, data);

#endif

}


void Debug_Printf(int data)
{
	uint8_t len;
	uint8_t str[40];

	len = sprintf(str,"%d %d\r\n",m_hd1.shotCnt, data);

	HAL_UART_Transmit(&huart1,str,len,100);
}
void Temp_Debug_Printf()
{

	uint8_t len;
	uint8_t str[40];

	if(m_hd1.shotStatus)
	{
		if(m_hd1.shotTempCnt<400)
		{
			m_hd1.shotTempCnt++;
			Debug_Printf(adcChAvg);
		}
		else
		{
			m_hd1.shotTempCnt = 0;
			m_hd1.shotStatus = 0;
		}

	}

}

extern float Kp,Ki;

void Main_Tx_1Data(int cmd, int data)
{
	uint8_t len;
	uint8_t str[40];

	while(HAL_GetTick() -m_hd1.lastHPTxTime<50);
	len = sprintf(str,"[%d,%d]\r\n",cmd, data);
	HAL_UART_Transmit(&huart2,str,len,100);
	Debug_MAIN_Printf(DEBUG_TX, cmd, data);
	m_hd1.lastHPTxTime = HAL_GetTick();
}

extern float PIDoutput;

void HP1_Cmd_Config()
{
	static uint32_t timeStamp;
	if(m_hd1.cartAllSend == 0)return;
	if(HAL_GetTick()-timeStamp >= 1000)
	{

		timeStamp = HAL_GetTick();
	#if 1
	Main_Tx_4Data(CMD_HP1_ADD, m_hd1.mode, m_hd1.pwmDuty, adcChAvg, m_hd1.catridgeStatus);
	#else
		Debug_Printf();
	#endif
	}
}

uint16_t delTx= 100;
uint32_t tt1,tt2;
void Catridge_All_Tx()
{
	HAL_Delay(500);
	tt1 = HAL_GetTick();
	Main_Tx_1Data(CMD_CART_ID, m_hd1.catridgeId);
	Main_Tx_1Data(CMD_MANUFAC_YY, m_hd1.manufacYY);
	Main_Tx_1Data(CMD_MANUFAC_MM, m_hd1.manufacMM);
	Main_Tx_1Data(CMD_MANUFAC_DD, m_hd1.manufacDD);
	Main_Tx_1Data(CMD_ISSUED_YY, m_hd1.issuedYY);
	Main_Tx_1Data(CMD_ISSUED_MM, m_hd1.issuedMM);
	Main_Tx_1Data(CMD_ISSUED_DD, m_hd1.issuedDD);

#if 0
	for(int i =1 ;i <= 7;i++)
	{
		Main_Tx_1Data(CMD_TRANDU_FRQ_BASE+i, m_hd1.rfFrqBuff[i]);
	}

	for(int i =1 ;i <= 77;i++)
	{
		Main_Tx_1Data(CMD_TRANDU_WATT_BASE+i, m_hd1.rfWattBuff[i]);
	}
#endif

	Main_Tx_1Data(CMD_REMIND_SHOT, m_hd1.remainingShotNum);
	Main_Tx_1Data(CMD_CATRIDGE_STATUS, m_hd1.catridgeStatus);

	Main_Tx_1Data(CMD_GET_ALL_CART_END, 0);//END
	Main_Tx_1Data(CMD_CART_ALLOW, 0);
	HAL_Delay(100);
	tt2 = HAL_GetTick() - tt1;

}
void Catridge_All_Zero()
{
	m_hd1.catridgeId = 0;
	m_hd1.manufacYY = 0;
	m_hd1.manufacMM = 0;
	m_hd1.manufacDD = 0;
	m_hd1.issuedYY = 0;
	m_hd1.issuedMM = 0;
	m_hd1.issuedDD = 0;

	for(int i =1 ;i <= 7;i++)
	{
		m_hd1.rfFrqBuff[i] = 0;
	}

	for(int i =1 ;i <= 77;i++)
	{
		m_hd1.rfWattBuff[i] = 0;
	}

	m_hd1.remainingShotNum = 0;
	m_hd1.catridgeStatus = 0;

}

void Watt_All_Tx()
{
	HAL_Delay(500);
	tt1 = HAL_GetTick();

#if 1

	for(int i =1 ;i <= 77;i++)
	{
		Main_Tx_1Data(CMD_TRANDU_WATT_BASE+i, m_hd1.rfWattBuff[i]);
	}
#endif


	Main_Tx_1Data(CMD_GET_WATT_CART, 1);//END
	HAL_Delay(100);
	tt2 = HAL_GetTick() - tt1;

}


void Catridge_Detect_Button()
{
	static uint32_t timeStamp;

	if(HAL_GetTick()-timeStamp >= 100)
	{
		timeStamp = HAL_GetTick();
		if(IS_HP1_SHOT_PUSH())
		{
			m_hd1.catridgeDetect = 1;
		}
		else
		{
			m_hd1.catridgeDetect = 0;
		}
	}

}
void Catridge_Detect_Event()
{
	static uint8_t step = STEP0;
	static uint32_t timeStamp;

	Catridge_Detect_Button();

	if(m_hd1.catridgeDetect)
	{
		if(m_hd1.catridgeAcction==0)
		{
			HAL_Delay(100); // wate for eeprom wake up
			Eeprom_All_Read();
			Main_Tx_1Data(CMD_CART_ID, m_hd1.catridgeId);
			Main_Tx_1Data(CMD_REMIND_SHOT, m_hd1.remainingShotNum);
			Main_Tx_1Data(CMD_CATRIDGE_EVENT, CATRIGE_DETECT);
			m_hd1.detectOnCnt++;
		}
		m_hd1.catridgeAcction = 1;
	}
	else
	{
		if(m_hd1.catridgeAcction==1)
		{
			Catridge_All_Zero();
			Catridge_All_Tx();
			Main_Tx_1Data(CMD_CATRIDGE_EVENT, CATRIGE_UN_DETECT);
			m_hd1.detectOffCnt++;
		}
		m_hd1.catridgeAcction = 0;
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
//		switch (rxCmd)
//		{
//			case CMD_HP1_TEST_IO:
//				m_hd1.catridgeDetect = rxData;
//				m_hd1.catridgeAcction = 0;
//			break;
//		}
		m_uart1.rxCmdAdd = 0;
		m_uart1.rxCmdData = 0;
	}

}

void UartRx2DataProcess()
{
	int rxCmd = m_uart2.rxCmdAdd;
	int rxData = m_uart2.rxCmdData;
	int rxCmdIdx;
	uint16_t valueTd, valueWatt;
	if(rxCmd !=0)
	{
		Debug_MAIN_Printf(DEBUG_RX, rxCmd, rxData);
		switch (rxCmd)
		{
			case CMD_HP1_ADD:
				switch (rxData)
				{
					case CMD_HP1_COOL_CTRL:
						m_hd1.step = STEP1;
					break;
					case CMD_HP1_COOL_OFF:
						m_hd1.step = STEP2;
					break;
				}
			break;

			case CMD_CART_ID:
				if(rxData == REQ_DATA)
				{
					Main_Tx_1Data(CMD_CART_ID, m_hd1.catridgeId);
				}
				else
				{
					m_hd1.catridgeId = rxData;
					Eeprom_Byte_Write(IDX_HP1_CART_ID_START, rxData);
				}
			break;

			case CMD_MANUFAC_YY:
				m_hd1.manufacYY = rxData;
				Eeprom_Byte_Write(IDX_HP1_MANUFAC_YY_START, rxData);
			break;

			case CMD_MANUFAC_MM:
				m_hd1.manufacMM = rxData;
				Eeprom_Byte_Write(IDX_HP1_MANUFAC_MM_START, rxData);
			break;

			case CMD_MANUFAC_DD:
				m_hd1.manufacDD = rxData;
				Eeprom_Byte_Write(IDX_HP1_MANUFAC_DD_START, rxData);
			break;

			case CMD_ISSUED_YY:
				m_hd1.issuedYY = rxData;
				Eeprom_Byte_Write(IDX_HP1_ISSUED_YY_START, rxData);
			break;

			case CMD_ISSUED_MM:
				m_hd1.issuedMM = rxData;
				Eeprom_Byte_Write(IDX_HP1_ISSUED_MM_START, rxData);
			break;

			case CMD_ISSUED_DD:
				m_hd1.issuedDD = rxData;
				Eeprom_Byte_Write(IDX_HP1_ISSUED_DD_START, rxData);
			break;

			case CMD_REMIND_SHOT:
				if(rxData == REQ_DATA)
				{
					Main_Tx_1Data(CMD_REMIND_SHOT, m_hd1.remainingShotNum);
				}
				else
				{
					m_hd1.remainingShotNum = rxData;
					Eeprom_Word_Write(IDX_HP1_REMIND_SHOT_START, rxData);

				}
			break;

			case CMD_CART_ALLOW:
				Catridge_All_Tx();
			break;


			case CMD_LCD_EXP:
				if(rxData == 2)
				{
					//m_hd1.shotStatus = 0;
//					Debug_Printf(444);
				}
				else if(rxData == 1)
				{
					m_hd1.shotStatus = 1;
					m_hd1.shotTempCnt = 0;
					m_hd1.shotCnt++;
//					Debug_Printf(555);
				}

			break;

			case CMD_PULSE_TRIGER:
				Debug_Printf(rxData);
			break;




			case CMD_DAY_REQ:
				Main_Tx_1Data(CMD_MANUFAC_YY, m_hd1.manufacYY);
				Main_Tx_1Data(CMD_MANUFAC_MM, m_hd1.manufacMM);
				Main_Tx_1Data(CMD_MANUFAC_DD, m_hd1.manufacDD);

				Main_Tx_1Data(CMD_ISSUED_YY, m_hd1.issuedYY);
				Main_Tx_1Data(CMD_ISSUED_MM, m_hd1.issuedMM);
				Main_Tx_1Data(CMD_ISSUED_DD, m_hd1.issuedDD);
			break;

			case CMD_WATT_REQ:
				for(int i =1 ;i <= 77;i++)
				{
					Main_Tx_1Data(i+CMD_TRANDU_WATT_BASE, m_hd1.rfWattBuff[i]);
					//HAL_Delay(10);
				}
			break;

			case CMD_FRQ_REQ:
				for(int i =1 ;i <= 7;i++)
				{
					Main_Tx_1Data(i+CMD_TRANDU_FRQ_BASE, m_hd1.rfFrqBuff[i]);
					//HAL_Delay(20);
				}
			break;

			case CMD_CATRIDGE_STATUS:
				if(rxData == REQ_DATA)
				{
					Main_Tx_1Data(CMD_CATRIDGE_STATUS, m_hd1.catridgeStatus);
				}
				else
				{
					m_hd1.catridgeStatus = rxData;
					Eeprom_Byte_Write(IDX_CATRIDGE_STATUS_START, rxData);


				}
			break;

			case CMD_DO_ALL_LIVE:
				m_hd1.liveChkCnt++;
				m_hd1.cartAllSend = 0;
				Main_Tx_1Data(CMD_DO_ALL_LIVE, 0);
			break;

			case CMD_GET_ALL_CART:
				Catridge_All_Tx();
			break;

			case CMD_GET_ALL_CART_END:
				m_hd1.cartAllSend = 1;
			break;

			case CMD_GET_WATT_CART:
				 Watt_All_Tx();
			break;



			default:
				if(CMD_TRANDU1_FRQ <= rxCmd && rxCmd <=CMD_TRANDU7_FRQ)
				{
					m_hd1.rfFrqBuff[rxCmd-CMD_TRANDU_FRQ_BASE] = rxData;
					rxCmdIdx = (rxCmd-CMD_TRANDU_FRQ_BASE-1)*2 +IDX_HP1_FRQ_BUFF_START;
					Eeprom_Word_Write(rxCmdIdx, rxData);

				}
				else if(CMD_TRANDU1_WATT10 <= rxCmd && rxCmd <=CMD_TRANDU7_WATT005)
				{
					valueTd = rxCmd -CMD_TRANDU_WATT_BASE;
					m_hd1.rfWattBuff[valueTd] = rxData;
					rxCmdIdx = (rxCmd-CMD_TRANDU_WATT_BASE-1)*2+IDX_HP1_WATT_BUFF_START;
					Eeprom_Word_Write(rxCmdIdx, rxData);
				}

			break;


		}

		m_uart2.rxCmdAdd = 0;
		m_uart2.rxCmdData = 0;
	}
}


void Pwm_Duty_Ctrl(uint32_t pwmDuty)
{
	Pwm_DutySet_Tim1_CH4(pwmDuty);
	m_hd1.pwmDuty = pwmDuty/100;

}


