/*
 * TestMy.c
 *
 *  Created on: Jul 17, 2025
 *      Author: Cellah_SW
 */

#include "TestMy.h"

void Pwm_Init()
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

}

uint32_t dutyTest = 10;
void Pwm_DutySet_Tim1_CH4(uint32_t pwmDuty)
{
	TIM1->CCR4 = pwmDuty;
}
#define MAX31855_CS_GPIO_Port   GPIOA
#define MAX31855_CS_Pin         GPIO_PIN_4

// CS ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½Å©ï¿½ï¿½
#define MAX31855_CS_LOW()    HAL_GPIO_WritePin(MAX31855_CS_GPIO_Port, MAX31855_CS_Pin, GPIO_PIN_RESET)
#define MAX31855_CS_HIGH()   HAL_GPIO_WritePin(MAX31855_CS_GPIO_Port, MAX31855_CS_Pin, GPIO_PIN_SET)

// ï¿½Âµï¿½ ï¿½Ð±ï¿½ ï¿½Ô¼ï¿½

float temp31855;
float internal_temp;
uint8_t errTemp;
uint8_t spi_rx_buf[4] = {0};
uint32_t errTempCnt31855;

uint8_t MAX31855_ReadTemperature()
{
    uint32_t raw = 0;

    MAX31855_CS_LOW();
//    HAL_SPI_Receive(&hspi1, spi_rx_buf, 4, HAL_MAX_DELAY);
    MAX31855_CS_HIGH();

    // ï¿½ï¿½ï¿½Åµï¿½ 4ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ uint32_tï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
    raw = (spi_rx_buf[0] << 24) | (spi_rx_buf[1] << 16) | (spi_rx_buf[2] << 8) | spi_rx_buf[3];

    // ï¿½ï¿½ï¿½ï¿½ Ã¼Å©
    if (raw & 0x00010000) {
        // ï¿½ï¿½Æ® 16: Open Circuit (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ü¼ï¿½)
        errTempCnt31855++;
        return 2;
    }

    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Âµï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½Æ® 31~18, 14ï¿½ï¿½Æ® signed ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ 12ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿?
    int16_t temp_raw = (raw >> 18) & 0x3FFF;
    if (raw & 0x80000000) {
        temp_raw |= 0xC000; // sign extend (ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½)
    }
    temp31855 = temp_raw * 0.25f;

    // ï¿½ï¿½ï¿½ï¿½ ï¿½Âµï¿½ ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½Æ® 15~4, 12ï¿½ï¿½Æ® signed)
    int16_t int_raw = (raw >> 4) & 0x0FFF;
    if (int_raw & 0x800) {
        int_raw |= 0xF000; // sign extend
    }
    internal_temp = int_raw * 0.0625f;

    return 1;
}

#define MAX6675_CS_GPIO_Port   GPIOA
#define MAX6675_CS_Pin         GPIO_PIN_4
#define MAX6675_CS_LOW()   HAL_GPIO_WritePin(MAX6675_CS_GPIO_Port, MAX6675_CS_Pin, GPIO_PIN_RESET)
#define MAX6675_CS_HIGH()  HAL_GPIO_WritePin(MAX6675_CS_GPIO_Port, MAX6675_CS_Pin, GPIO_PIN_SET)
float temp6675;
uint32_t errTempCnt6675;

uint8_t MAX6675_ReadTemperature()
{
    uint8_t rx_buf[2] = {0};
    uint16_t raw_data;

    MAX6675_CS_LOW();
    HAL_Delay(1);  // ï¿½à°£ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ã°ï¿½

//    if (HAL_SPI_Receive(&hspi1, rx_buf, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        MAX6675_CS_HIGH();
        errTempCnt6675++;
        return 2;
    }

    MAX6675_CS_HIGH();

    raw_data = (rx_buf[0] << 8) | rx_buf[1];

    // D2 (ï¿½ï¿½Æ®2) == 1 ï¿½Ì¸ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ü¼ï¿½ (No thermocouple connected)
    if (raw_data & 0x0004) {
    	errTempCnt6675++;
        return 2;
    }

    // ï¿½ï¿½ï¿½ï¿½ 12ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½Âµï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (bit 15 ~ bit 3), 0.25ï¿½ï¿½C ï¿½ï¿½ï¿½ï¿½
    raw_data >>= 3;
    temp6675 = raw_data * 0.25f;

    return 1;
}




float targetTemp = 7;
float currentTemp;
float PIDerror;
float prev_error;

float PIDoutput;
uint32_t PIDoutputCCR;
uint8_t KpUpDn;

uint8_t PIDEn = 0, PIDKey;
float integral;
float derivative;

#define PID_DT	0.5
float Kp = 50.0f;  // ï¿½ï¿½ï¿?ï¿½Ìµï¿½, ï¿½Ê¿ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½
float Ki = 0.0f;
float Kd = 0.0f;

void calculate_pid(float target_temp, float current_temp)
{
    PIDerror = current_temp - target_temp;

    integral += PIDerror * PID_DT;                // I Ç× ´©Àû
    derivative = (PIDerror - prev_error) / PID_DT;  // D Ç× º¯È­À²

    prev_error = PIDerror;

    PIDoutput = Kp * PIDerror + Ki * integral + Kd * derivative;

    if (PIDoutput > 100.0f)
        PIDoutput = 100.0f;
    else if (PIDoutput < 0.0f)
        PIDoutput = 0.0f;

	PIDoutputCCR = PIDoutput*100;
	Pwm_DutySet_Tim1_CH4(PIDoutputCCR);

}


void calculate_p_pwm(float target_temp, float current_temp) {

    PIDerror = current_temp - target_temp;

	if(PIDerror>0) // ¸ñÇ¥º¸´Ù ¶ß°Å¿ì¸é °¡µ¿
	{
	    PIDoutput = Kp * PIDerror;

	    if (PIDoutput > 100.0f)
	        PIDoutput = 100.0f;
	    else if (PIDoutput < 0.0f)
	        PIDoutput = 0.0f;

	}
	else if(PIDerror<0)
	{
		PIDoutput = 0.0f;
	}
	else if(PIDerror==0)
	{
		//
	}

	PIDoutputCCR = PIDoutput*100;
	Pwm_DutySet_Tim1_CH4(PIDoutputCCR);

}

void Test_Init()
{
	Pwm_Init();
//	HAL_Delay(2000);
//	Pwm_DutySet_Tim1_CH4(10000);
	m_hd1.step = STEP1;


}
#define ALPHA  0.7
float lowPassTemp,lowPassTempPre;
float lowPassTempCh[10],lowPassTempPreCh[10];

extern uint32_t adcChNum,adcChBuff[11];

uint32_t Low_Pass_Filter(int X)
{
  uint32_t temp;
  lowPassTemp  =  ALPHA * lowPassTempPre + (1-ALPHA)* X;
  temp = (uint32_t)lowPassTemp;
  lowPassTempPre = lowPassTemp;

  return temp;
}


uint32_t Low_Pass_Filter_Ch(int X,uint8_t ch)
{
  uint32_t temp;
  lowPassTempCh[ch]  =  ALPHA * lowPassTempPreCh[ch] + (1-ALPHA)* X;
  temp = (uint32_t)lowPassTempCh[ch];
  lowPassTempPreCh[ch] = lowPassTempCh[ch];

  return temp;
}

float nowTemp;
void PID_Ctrl()
{
	static uint32_t timeStamp;
	int intTemp31855 = temp31855*10;//
	int intTemp6675 = temp6675*10;
	int intlowpass = adcChBuff[0];
	int intTargetTemp = targetTemp*10;
	int intPIDoutput = PIDoutput;
	int intTick = timeStamp/1000;
	int intKp = Kp;
	if(HAL_GetTick()-timeStamp >= 500)
	{
		timeStamp = HAL_GetTick();
		nowTemp = (float)adcChBuff[0]/10;
		calculate_pid(targetTemp, nowTemp);

//		printf("%d %d %d %d\r\n", intlowpass, intTargetTemp, intPIDoutput, intKp);
	}

}

uint16_t ttuuDuty;
void Force_Duty()
{



	if(dutyTest && PIDEn==0)
	{
		if(dutyTest ==11)
		{
			Pwm_DutySet_Tim1_CH4(0);
			ttuuDuty = 0;
		}
		else
		{
			dutyTest *=1000;
			Pwm_DutySet_Tim1_CH4(dutyTest);
			ttuuDuty = dutyTest;
		}
		dutyTest = 0;
	}
}

void Force_DutyTest()
{
	static uint32_t timeStamp,duration;

	if(HAL_GetTick()-timeStamp >= duration)
	{
		static uint8_t toggle = 1;

			if(toggle)
			{
				toggle = 0;
				Pwm_DutySet_Tim1_CH4(10000);
				duration = 120000;
			}
			else
			{
				toggle = 1;
				Pwm_DutySet_Tim1_CH4(0);
				duration = 120000;

			}
		timeStamp = HAL_GetTick();
	}

}
void PID_OnOfff_Config()
{
	static uint8_t hTempCnt = 0;
	static uint32_t timeStamp,timeStamp2,timeStamp3;

	if(HAL_GetTick()-timeStamp >= 300)
	{
		errTemp = MAX6675_ReadTemperature();
		Low_Pass_Filter(temp6675);

		if(PIDKey)
		{
			if(HAL_GetTick()-timeStamp3 >= 120000 && PIDEn==0)
			{
				PIDEn = 1;
				timeStamp3 = HAL_GetTick();
				timeStamp2 = timeStamp3;
			}

			if(PIDEn)
			{
				if(HAL_GetTick()-timeStamp2 >= 60000)
				{
					targetTemp = 7;
					PIDEn = 0;
					Pwm_DutySet_Tim1_CH4(0);
				}
			}

		}
		timeStamp = HAL_GetTick();
	}

}







void ADC1_Channel_Selection(uint8_t ch)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  if(ch == 0) sConfig.Channel = ADC_CHANNEL_0;
  else if(ch == 1)  sConfig.Channel = ADC_CHANNEL_1;
  else if(ch == 4)  sConfig.Channel = ADC_CHANNEL_4;
  else if(ch == 5)  sConfig.Channel = ADC_CHANNEL_5;
  else if(ch == 6)  sConfig.Channel = ADC_CHANNEL_6;
  else if(ch == 7)  sConfig.Channel = ADC_CHANNEL_7;
  else if(ch == 8)  sConfig.Channel = ADC_CHANNEL_8;
  else if(ch == 9)  sConfig.Channel = ADC_CHANNEL_9;
  else return;

  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}


uint8_t gpioRead[10];

void ddGpio(GPIO_TypeDef *gpio,uint16_t pin, uint8_t ch)
{
	if (HAL_GPIO_ReadPin(gpio, pin))
	{
		gpioRead[ch] = 2;
	}
	else
	{
		gpioRead[ch] = 1;
	}

}


void Test_While()
{


#if 0
//	TxTest();

#else
	Force_Duty();
	HP1_Cmd_Config();
	HP1_Temp_Duty_Ctrl();
	Catridge_Detect_Event();
	NTC_TempWhile();
	UartRxDataProcess();


#endif

//	ddGpio(CAT_DET_GPIO_Port,CAT_DET_Pin,0);
}


