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

uint32_t dutyTest;
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
    HAL_SPI_Receive(&hspi1, spi_rx_buf, 4, HAL_MAX_DELAY);
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

    if (HAL_SPI_Receive(&hspi1, rx_buf, 2, HAL_MAX_DELAY) != HAL_OK) {
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

uint8_t PIDEn, PIDKey;
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

}
#define ALPHA  0.7
float Tx_low_pass_buff[10];
uint8_t Tx_low_cnt;
float lowPassTemp;
void Low_Pass_Filter(int X)
{
  Tx_low_pass_buff[Tx_low_cnt++]  =  ALPHA * lowPassTemp + (1-ALPHA)* X;
  Tx_low_cnt %=10;
  if(Tx_low_cnt==0) lowPassTemp = Tx_low_pass_buff[9];
  else lowPassTemp = Tx_low_pass_buff[Tx_low_cnt-1];

}

void PID_Ctrl()
{
	static uint32_t timeStamp;
	int intTemp31855 = temp31855*10;//
	int intTemp6675 = temp6675*10;
	int intlowpass = lowPassTemp*10;
	int intTargetTemp = targetTemp*10;
	int intPIDoutput = PIDoutput;
	int intTick = timeStamp/1000;
	int intKp = Kp;

	if(HAL_GetTick()-timeStamp >= 500)
	{
		timeStamp = HAL_GetTick();

		calculate_pid(targetTemp, lowPassTemp);

		printf("%d %d %d %d\r\n", intlowpass, intTargetTemp, intPIDoutput, intKp);
	}

}
void Force_Duty()
{



	if(dutyTest && PIDEn==0)
	{
		if(dutyTest ==11)
		{
			Pwm_DutySet_Tim1_CH4(0);
		}
		else
		{
			dutyTest *=1000;
			Pwm_DutySet_Tim1_CH4(dutyTest);
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
				duration = 20000;
			}
			else
			{
				toggle = 1;
				Pwm_DutySet_Tim1_CH4(0);
				duration = 60000;

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
#if 0
			if(lowPassTemp>17)
			{
				hTempCnt++;
				if(hTempCnt==4)//1ÃÊ
				{
					hTempCnt = 0;
					timeStamp2 = HAL_GetTick();
					targetTemp = 7;
					PIDEn = 1;
				}
			}
			else hTempCnt = 0;

#else

			if(HAL_GetTick()-timeStamp3 >= 120000 && PIDEn==0)
			{
				PIDEn = 1;
				timeStamp3 = HAL_GetTick();
				timeStamp2 = timeStamp3;
			}
#endif

			if(PIDEn)
			{
				if(HAL_GetTick()-timeStamp2 >= 60000)
				{
					targetTemp = 7;
					PIDEn = 0;
					Pwm_DutySet_Tim1_CH4(0);
					printf("\r\n");
				}
			}

		}

		timeStamp = HAL_GetTick();
	}

}

uint32_t adc1, adc2;
void ADC_Read()
{

	HAL_ADC_Start(&hadc1);
	if (HAL_ADC_PollForConversion(&hadc1, 1000000) == HAL_OK)
	{
		adc1 = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);
	}

	HAL_ADC_Start(&hadc2);
	if (HAL_ADC_PollForConversion(&hadc2, 1000000) == HAL_OK)
	{
		adc2 = HAL_ADC_GetValue(&hadc2);
		HAL_ADC_Stop(&hadc2);
	}
}

void Test_While()
{

#if 0
	Force_Duty();
	PID_OnOfff_Config();
	if(PIDEn)
	{
		PID_Ctrl();
	}
#else
//	Force_DutyTest();
	//Force_Duty();
	ADC_Read();

#endif




}


