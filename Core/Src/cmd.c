//cmd.c

#include "cmd.h"
UART_T m_uart1;
UART_T m_uart2;
UART_T m_uart3;



uint8_t Rx_data1[1];
uint8_t Rx_data2[1];
uint8_t Rx_data3[1];

void Uart_Init()
{
	HAL_UART_Receive_IT(&huart1, Rx_data2, 1);
}


#if 0
int putchar(int ch)
{
	while(HAL_OK != HAL_UART_Transmit_IT(&huart2, (uint8_t *)&ch, 1))
	{}
	return ch;

}

#else
int __io_putchar(int ch)
{
    while(HAL_OK != HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1,100))
	{}
	return ch;
}

#endif
uint8_t Uart_RxBuff_Get(UART_T* uart, uint8_t data,char startChar, char endChar)
{
	if(data ==startChar)
	{
		uart->rxCnt = 0;
		uart->startFlag = 1;
		memset(uart->rxBuff, 0, RX_BUFF_SIZE);
	}
	else if(uart->startFlag &&data ==endChar)
	{
		uart->endFlag = 1;
	}
	if(uart->startFlag)
	{
		uart->rxBuff[uart->rxCnt] = data;
		uart->rxCnt++;
		uart->rxCnt%=RX_BUFF_SIZE;
	}

	if(uart->startFlag&&uart->endFlag)
	{
		uart->startFlag = 0;
		uart->endFlag = 0;
		return 1;
	}
	else
	{
		return 0;
	}

}


void Rx_BuffClear(UART_T *uart)
{
	memset(uart->rxBuff,0,RX_BUFF_SIZE);
	uart->rxCnt = 0;
}





int imgAddress;
int dataNum;
uint32_t onTime, offTime, pluse, ready, standby;
void Uart1_Passing(uint8_t* ptr)//stm32쪽
{
	int i = 0;
	for(i =0 ;i < RX_BUFF_SIZE;i++)
	{
		if(m_uart1.rxBuff[i]==NULL)i++;
		else break;
	}
	if(i==RX_BUFF_SIZE)// 들어온 데이터가 없으면 나가기
	{
		return;
	}
	char* ptrStart = strstr((char*)(m_uart1.rxBuff+i),"[");
	char* ptrEnd = strstr((char*)(m_uart1.rxBuff+i),"]");

	if(ptrStart !=NULL && ptrEnd !=NULL)
	{
		sscanf(ptr,"[%d,%d]",&imgAddress,&dataNum);
		Rf_RxLcd_Parssing(imgAddress, dataNum);
	}
	else
	{
//		ERR_FUC_LINE();
		printf("%s \r\n",m_uart1.rxBuff);
	}

	Rx_BuffClear(&m_uart1);
}


void Uart_PassingConfig(UART_T* uart, uint8_t data, char startChar, char endChar)
{
	if(Uart_RxBuff_Get(uart, data, startChar, endChar))
	{
		Uart1_Passing(uart->rxBuff);
	}
}

void TxTest()
{
	static uint32_t timeStamp;

	if(HAL_GetTick()-timeStamp >= 500 )
	{
		timeStamp = HAL_GetTick();
//		printf("time %u \r\n",HAL_GetTick());
		HAL_UART_Transmit(&huart1, "hello", 5,100);


	}
}
void Uart_RxBuff_View(UART_T* uart, uint8_t data)
{
	uart->rxViewBuff[uart->rxViewCnt++] = data;
	uart->rxViewCnt %= RX_BUFF_SIZE;
}


void Uart_ClearCharBuff(uint8_t* buff, uint16_t*cnt, int size)
{
	memset(buff, 0, size);
	*cnt = 0;
}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	uint8_t cmd = 0;
	static uint8_t startFlag = 0;
	static uint8_t endFlag = 0;
	 if (huart == &huart1)
	 {
		HAL_UART_Receive_IT(&huart1, Rx_data2, 1);
		Uart_RxBuff_View(&m_uart2, Rx_data2[0]);
//	    Rf_Rx_Get(Rx_data2[0]);

	 }
//	 else if (huart == &huart1)
//	 {
//		HAL_UART_Receive_IT(&huart1, Rx_data1, 1);
//		Uart_RxBuff_View(&m_uart1, Rx_data1[0]);
//	 	Uart_PassingConfig(&m_uart1, Rx_data1[0],'[', ']');
//	 }
//	 if (huart == &huart3)
//	 {
//		HAL_UART_Receive_IT(&huart3, Rx_data3, 1);
//		Uart_RxBuff_View(&m_uart3, Rx_data3[0]);
//		Rf_Rx_Get(Rx_data3[0]);

//	 }
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)//485
{
	if(huart == &huart1)
	{
//		HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);//485
	}

}

extern uint32_t readRfidID;
void Uart_Gulobal()
{
	if(m_uart2.clearFlag1)
	{
		m_uart2.clearFlag1 = 0;
		Uart_ClearCharBuff(m_uart2.rxBuff,&m_uart2.rxCnt ,RX_BUFF_SIZE);
	}

	if(m_uart2.clearFlag2)
	{
		m_uart2.clearFlag2 = 0;
		Uart_ClearCharBuff(m_uart2.rxViewBuff,&m_uart2.rxViewCnt ,RX_BUFF_SIZE);
	}
	static uint32_t timeStamp;


}



