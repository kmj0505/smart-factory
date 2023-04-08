/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "wifi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CMD_SIZE 50
#define ARR_CNT 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char sendData[50]={0};  //wifi send buff
char recvData[50]={0};  //wifi recv buff
extern char rx3Data[50];
extern char rx2Data;
extern volatile unsigned char rx2Flag;
extern volatile unsigned char rx3Flag;
extern volatile unsigned long systick_sec;            //1sec
extern volatile int systick_secFlag;
extern volatile char uart2_rxdata[5][100];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
	(void)HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 100);
	return ch;
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART3_UART_Init();
	MX_TIM2_Init();
	MX_USART2_UART_Init();
	Serial3_Send_String("start main()\r\n");
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */
	WIFI_init();
	sprintf(sendData,"["LOGID":"PASSWD"]");
	WIFI_send(sendData);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		recvData[0] = 0;
		if(wifi_wait("+IPD","+IPD", 50))  //수신포멧 :  +IPD,6:hello  끝문자 0x0a
		{
			for(int i=0;i<5;i++)
			{
				if(strncmp((char *)uart2_rxdata[i],"+IPD",4)==0)
				{
					strcpy(recvData,(char *)(uart2_rxdata[i]+8));
					recvData[strlen((char *)(uart2_rxdata[i]+8)) - 1] = 0;
				}
			}
		}

		if(rx3Flag)
		{
			printf("rx3Data : %s\r\n",rx3Data);
//			Serial2_Send_String(rx3Data);
//			Serial2_Send_String("\r");
			rx3Flag = 0;
		}
		if(recvData[0] != 0)
		{
			Serial2_Event();
			recvData[0] = 0;
		}
	/* USER CODE END WHILE */
	/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void Serial2_Event()
{

//  Serial3_Send_String(rx2Data);
//  Serial3_Send_String("\r\n");

  int i=0;
  int num;
  char * pToken;
  char * pArray[ARR_CNT]={0};
  char recvBuf[CMD_SIZE]={0};

  strcpy(recvBuf,recvData);
//  Serial3_Send_String(recvBuf);
//  Serial3_Send_String("\n\r");
  printf("rx2Data : %s\r\n",recvBuf);

  pToken = strtok(recvBuf,"[@]");

  while(pToken != NULL)
  {
    pArray[i] = pToken;
    if(++i >= ARR_CNT)
      break;
    pToken = strtok(NULL,"[@]");
  }

  if(!strcmp(pArray[2],"SERVO"))
  {
    if(pArray[3] != NULL)
      num = atoi(pArray[3]);
    else
      num = 0;
    if(!strcmp(pArray[3],"1"))
    {
    	printf("SERVO 1 ON!!!!!!!\r\n");
    	TIM2->CCR1 = 25;
    }
    else if(!strcmp(pArray[3],"2"))
    {
    	printf("SERVO 2 ON!!!!!!!\r\n");
    	TIM2->CCR1 = 75;
    }
    else if(!strcmp(pArray[3],"3"))
    {
    	printf("SERVO 3 ON!!!!!!!\r\n");
    	TIM2->CCR1 = 120;
    }
  }
  else if(!strncmp(pArray[2]," New conn",8))
  {
     return;
  }
  else if(!strncmp(pArray[2]," Already log",8))
  {
      connectWiFi();
      sprintf(sendData,"["LOGID":"PASSWD"]");
      WIFI_send(sendData);
      return;
  }
  else
      return;

   sprintf(sendData,"[%s]%s@%s@ON\n",pArray[0],pArray[1],pArray[2]);
   WIFI_send(sendData);
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
