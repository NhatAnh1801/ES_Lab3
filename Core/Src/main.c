/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define INIT	1
#define RED1_GREEN2 2
#define RED1_YELLOW2 3
#define GREEN1_RED2	4
#define YELLOW1_RED2	5
#define MAN_RED		11
#define MAN_GREEN	12
#define MAN_YELLOW	13
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void test_LedY0();
void test_LedY1();
void test_7seg();
void test_button();
void test_lcd();
void fsm_run();
void man_run();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int status = INIT;
int counter = 0;
int red_timer = 5;
int green_timer = 3;
int yellow_timer = 2;
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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  lcd_Clear(WHITE);
  test_lcd();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  while(!flag_timer2);
	  flag_timer2 = 0;
	  button_Scan();

	  //fsm function run
	 fsm_run();
	  man_run();
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
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  setTimer2(50);
	  setTimer1(500);
}

uint8_t count_led_debug = 0;
uint8_t count_led_Y0 = 0;
uint8_t count_led_Y1 = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

void test_LedY0(){
	count_led_Y0 = (count_led_Y0+ 1)%100;
	if(count_led_Y0 > 40){
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 1);
	} else {
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	}
}

void test_LedY1(){
	count_led_Y1 = (count_led_Y1+ 1)%40;
	if(count_led_Y1 > 10){
		HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	} else {
		HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y1_Pin, 1);
	}
}
void test_7seg(){
	led7_SetDigit(0, 0, 0);
	led7_SetDigit(5, 1, 0);
	led7_SetDigit(4, 2, 0);
	led7_SetDigit(7, 3, 0);
}
void test_button(){
	for(int i = 0; i < 16; i++){
		if(button_count[i] == 1){
			lcd_ShowIntNum(140, 105, i, 2, BRED, WHITE, 32);
		}
	}
}
void test_lcd(){
	lcd_Fill(0, 0, 240, 20, BLUE);
	lcd_StrCenter(0, 2, "Nhat anh dep trai", RED, BLUE, 16, 1);
	lcd_ShowStr(20, 30, "He thong giao thong", WHITE, RED, 24, 0);
	//First light
	lcd_DrawCircle(40, 120, GREEN, 30, 0);
	//Second light
	lcd_DrawCircle(200, 120, GREEN, 30, 0);
	lcd_ShowPicture(80, 200, 90, 90, gImage_logo);
}
void Toggle_RED(){
	lcd_DrawCircle(40, 120, RED, 30, 1);
	//Second light
	lcd_DrawCircle(200, 120, RED, 30, 1);

}
void Toggle_GREEN(){
	lcd_DrawCircle(40, 120, GREEN, 30, 1);
	//Second light
	lcd_DrawCircle(200, 120, GREEN, 30, 1);
}
void Toggle_YELLOW(){
	lcd_DrawCircle(40, 120, YELLOW, 30, 1);
	//Second light
	lcd_DrawCircle(200, 120, YELLOW, 30, 1);
}

void fsm_run(){
	switch(status){
		case INIT:
			setTimer1(green_timer*1000);
			setTimer0(1000);
			status = RED1_GREEN2;
			//Display
			break;
		case RED1_GREEN2:
			counter = green_timer;
			lcd_ShowIntNum(120, 135, counter, 2, BRED, WHITE, 32);
			lcd_DrawCircle(40, 120, RED, 30, 1);
			//Second light
			lcd_DrawCircle(200, 120, GREEN, 30, 1);
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			//DISPLAY MODE
			if(flag_timer1 == 1){
				setTimer1(yellow_timer*1000);
				status = RED1_YELLOW2;
			}

			break;
		case RED1_YELLOW2:
			counter = yellow_timer;
			lcd_ShowIntNum(120, 135, counter, 2, BRED, WHITE, 32);
			lcd_DrawCircle(40, 120, RED, 30, 1);
			//Second light
			lcd_DrawCircle(200, 120, YELLOW, 30, 1);
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			if(flag_timer1 == 1){
				status = GREEN1_RED2;
				setTimer1(green_timer*1000);
			}
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			break;
		case GREEN1_RED2:
			counter = red_timer;
			lcd_ShowIntNum(120, 135, counter, 2, BRED, WHITE, 32);
			lcd_DrawCircle(40, 120, GREEN, 30, 1);
			//Second light
			lcd_DrawCircle(200, 120, RED, 30, 1);
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			if(flag_timer1 == 1){
				status = YELLOW1_RED2;
				setTimer1(yellow_timer*1000);
			}
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			break;
		case YELLOW1_RED2:
			counter = red_timer;
			lcd_ShowIntNum(120, 135, counter, 2, BRED, WHITE, 32);
			lcd_DrawCircle(40, 120, YELLOW, 30, 1);
			//Second light
			lcd_DrawCircle(200, 120, RED, 30, 1);
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			if(flag_timer1 == 1){
				status = RED1_GREEN2;
				setTimer1(red_timer*1000);
			}
			if(button_count[0] == 1){
				status = MAN_RED;
			}
			break;
		default:
			break;
	}
}
void man_run(){
	switch(status){
		case MAN_RED:
		Toggle_RED();
		//DISPLAY MODE
		lcd_ShowStr(20, 30, "Red modification mode!!!", WHITE, RED, 24, 0);
		lcd_ShowIntNum(120, 135, red_timer, 2, BRED, WHITE, 32);
		//DISPLAY COUNTER
		if(button_count[1] == 1){
			status = MAN_GREEN;
		}
		if(button_count[4] == 1){
			red_timer++;
			if(red_timer > 99){
				red_timer = 0;
			}
		}
		break;
	case MAN_GREEN:
		Toggle_GREEN();
		//DISPLAY MODE
		lcd_ShowStr(20, 30, "Green modification mode!!!", WHITE, RED, 24, 0);
		lcd_ShowIntNum(120, 135, green_timer, 2, BRED, WHITE, 32);
		//DISPLAY COUNTER
		if(button_count[2] == 1){
			status = MAN_YELLOW;
		}
		if(button_count[4] == 1){
			green_timer++;
			if(green_timer > 99) green_timer = 0;
		}
		break;
	case MAN_YELLOW:
		Toggle_YELLOW();
		test_LedY0();
		//DISPLAY MODE
		lcd_ShowStr(20, 30, "Green modification mode!!!", WHITE, RED, 24, 0);
		lcd_ShowIntNum(120, 135, yellow_timer, 2, BRED, WHITE, 32);
		//DISPLAY COUNTER
		if(button_count[3] == 1){
			status = INIT;
		}
		if(button_count[4] == 1){
			yellow_timer++;
			if(yellow_timer > 99){
				yellow_timer = 0;
			}
		}
		break;
	default:
		break;
	}


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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
