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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "string.h"
#include "md_button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef enum
{
    SWD_BUTTON = 0,
    UART_BUTTON,
		JVREF_BUTTON,
    SW_BUTTON_MAX
} sw_button_t;

static md_button_t sw_button[SW_BUTTON_MAX];

static uint8_t common_btn_read(void *arg)
{
    uint8_t value = 0;

    md_button_t *btn = (md_button_t *)arg;

    switch (btn->id)
    {
      case SWD_BUTTON:
          value = HAL_GPIO_ReadPin(SWD_SW_GPIO_Port, SWD_SW_Pin);
          break;

      case UART_BUTTON:
          value = HAL_GPIO_ReadPin(UART_SW_GPIO_Port, UART_SW_Pin);
          break;

      case JVREF_BUTTON:
          value = HAL_GPIO_ReadPin(JVREF_SW_GPIO_Port, JVREF_SW_Pin);
          break;

      default: break;
    }

    return value;
}

uint8_t key_swd_flag = 0;
uint8_t key_uart_flag = 0;
uint8_t key_jvref_flag = 0;

static void common_btn_evt_cb(void *arg)
{
    md_button_t *btn = (md_button_t *)arg;

    if (btn->id == SWD_BUTTON && btn->event == MD_BTN_CLICK)
    {
      key_swd_flag = 1;
    }
    else if (btn->id == UART_BUTTON && btn->event == MD_BTN_CLICK)
    {
      key_uart_flag = 1;
    }
    else if (btn->id == JVREF_BUTTON && btn->event == MD_BTN_CLICK)
    {
      key_jvref_flag = 1;
    }
}

static void sw_button_init(uint32_t freq)
{
    uint32_t button_cnt = 0;
    memset(&sw_button[0], 0x0, sizeof(sw_button));
  
    md_button_set_scan_freq(freq);

    for (int i = 0; i < SW_BUTTON_MAX; i ++)
    {
        sw_button[i].usr_button_read = common_btn_read;
        sw_button[i].cb = common_btn_evt_cb;
        sw_button[i].active_level = 0;
        sw_button[i].short_press_duration = md_button_ms2cnt(800);
        sw_button[i].long_press_duration = md_button_ms2cnt(3000);
        sw_button[i].max_multiple_clicks_interval = md_button_ms2cnt(200);
        button_cnt = md_button_add(&sw_button[i]);
    }
}

#define INDEX_NONE 0
void uart_sel_ch(uint8_t index)
{
  uint8_t a0 = 1;
  uint8_t a1 = 1;
  uint8_t led1 = 1;
  uint8_t led2 = 1;
  uint8_t led3 = 1;

  switch(index)
  {
    case 0: break;
    case 1: 
    {
      a0 = 1; a1 = 0;
      led1 = 0;
    } break;

    case 2: 
    {
      a0 = 0; a1 = 1;
      led2 = 0;
    } break;

    case 3: 
    {
      a0 = 0; a1 = 0;
      led3 = 0;
    } break;

    default: break;
  }
  HAL_GPIO_WritePin(LED_UART1_GPIO_Port, LED_UART1_Pin, led1);
  HAL_GPIO_WritePin(LED_UART2_GPIO_Port, LED_UART2_Pin, led2);
  HAL_GPIO_WritePin(LED_UART3_GPIO_Port, LED_UART3_Pin, led3);
  HAL_GPIO_WritePin(UART_SW_A0_GPIO_Port, UART_SW_A0_Pin, a0);
  HAL_GPIO_WritePin(UART_SW_A1_GPIO_Port, UART_SW_A1_Pin, a1);
}

void swd_sel_ch(uint8_t index)
{
  uint8_t a0 = 1;
  uint8_t a1 = 1;
  uint8_t led1 = 1;
  uint8_t led2 = 1;
  uint8_t led3 = 1;

  switch(index)
  {
    case 0: break;
    case 1: 
    {
      a0 = 1; a1 = 1;
      led1 = 0;
    } break;

    case 2: 
    {
      a0 = 1; a1 = 0;
      led2 = 0;
    } break;

    case 3: 
    {
      a0 = 0; a1 = 1;
      led3 = 0;
    } break;

    default: break;
  }
  HAL_GPIO_WritePin(LED_SWD1_GPIO_Port, LED_SWD1_Pin, led1);
  HAL_GPIO_WritePin(LED_SWD2_GPIO_Port, LED_SWD2_Pin, led2);
  HAL_GPIO_WritePin(LED_SWD3_GPIO_Port, LED_SWD3_Pin, led3);
  HAL_GPIO_WritePin(SWD_SW_A0_GPIO_Port, SWD_SW_A0_Pin, a0);
  HAL_GPIO_WritePin(SWD_SW_A1_GPIO_Port, SWD_SW_A1_Pin, a1);
}

void jvref_sel_ch(uint8_t index)
{
  /**
   * 0 -- none
   * 1 -- 3v3
   * 2 -- 1v8
   * 3 -- none
   */
  switch(index)
  {
    case 0:
    case 3:
    {
      HAL_GPIO_WritePin(JVREF_EN_GPIO_Port, JVREF_EN_Pin, 0);
      HAL_GPIO_WritePin(LED_JVREF_1V8_GPIO_Port, LED_JVREF_1V8_Pin, 1);
      HAL_GPIO_WritePin(LED_JVREF_3V3_GPIO_Port, LED_JVREF_3V3_Pin, 1);
    } break;

    case 2:
    {
      HAL_GPIO_WritePin(JVREF_EN_GPIO_Port, JVREF_EN_Pin, 1);
			HAL_GPIO_WritePin(JVREF_SEL_GPIO_Port,  JVREF_SEL_Pin, 1);
      HAL_GPIO_WritePin(LED_JVREF_1V8_GPIO_Port, LED_JVREF_1V8_Pin, 0);
      HAL_GPIO_WritePin(LED_JVREF_3V3_GPIO_Port, LED_JVREF_3V3_Pin, 1);
    } break;
    case 1:
    {
      HAL_GPIO_WritePin(JVREF_EN_GPIO_Port, JVREF_EN_Pin, 1);
      HAL_GPIO_WritePin(JVREF_SEL_GPIO_Port, JVREF_SEL_Pin, 0);
      HAL_GPIO_WritePin(LED_JVREF_1V8_GPIO_Port, LED_JVREF_1V8_Pin, 1);
      HAL_GPIO_WritePin(LED_JVREF_3V3_GPIO_Port, LED_JVREF_3V3_Pin, 0);
    } break;
  }
}
void jlink_switcher_init(void)
{
  uart_sel_ch(INDEX_NONE);
  swd_sel_ch(INDEX_NONE);
	jvref_sel_ch(INDEX_NONE);
  sw_button_init(100);
}
static uint8_t uart_index = 0;
static uint8_t swd_index = 0;
static uint8_t jvref_index[4] = {0};

void jlink_switcher_process(void)
{
  static uint32_t last_tick = 0;
  uint32_t now_tick = HAL_GetTick();
  if (now_tick - last_tick >= 10)
  {
    last_tick = now_tick;
    md_button_scan();
  }

  if (key_swd_flag == 1)
  {
    key_swd_flag = 0;
    swd_index ++;
    if (swd_index > 3) {
      swd_index = 1;
    }
  }

  if (key_uart_flag == 1)
  {
    key_uart_flag = 0;
    uart_index ++;
    if (uart_index > 3) {
      uart_index = 1;
    }
  }

  if (key_jvref_flag == 1)
  {
    key_jvref_flag = 0;

    jvref_index[swd_index]++;
    
    if (jvref_index[swd_index] > 3) {
      jvref_index[swd_index] = 1;
    }
  }

  uart_sel_ch(uart_index);
  swd_sel_ch(swd_index);
  if (swd_index != INDEX_NONE)
  {
    jvref_sel_ch(jvref_index[swd_index]);
  }
}
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	jlink_switcher_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		jlink_switcher_process();
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SWD_SW_A0_Pin|SWD_SW_A1_Pin|JVREF_SEL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, UART_SW_A0_Pin|UART_SW_A1_Pin|LED_SWD1_Pin|LED_SWD2_Pin
                          |LED_SWD3_Pin|LED_UART3_Pin|LED_UART2_Pin|JVREF_EN_Pin
                          |LED_JVREF_1V8_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_UART1_Pin|LED_JVREF_3V3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SWD_SW_A0_Pin SWD_SW_A1_Pin JVREF_SEL_Pin */
  GPIO_InitStruct.Pin = SWD_SW_A0_Pin|SWD_SW_A1_Pin|JVREF_SEL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : UART_SW_A0_Pin UART_SW_A1_Pin LED_SWD1_Pin LED_SWD2_Pin
                           LED_SWD3_Pin LED_UART3_Pin LED_UART2_Pin LED_JVREF_1V8_Pin */
  GPIO_InitStruct.Pin = UART_SW_A0_Pin|UART_SW_A1_Pin|LED_SWD1_Pin|LED_SWD2_Pin
                          |LED_SWD3_Pin|LED_UART3_Pin|LED_UART2_Pin|LED_JVREF_1V8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SWD_SW_Pin */
  GPIO_InitStruct.Pin = SWD_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SWD_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_UART1_Pin LED_JVREF_3V3_Pin */
  GPIO_InitStruct.Pin = LED_UART1_Pin|LED_JVREF_3V3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : UART_SW_Pin JVREF_SW_Pin */
  GPIO_InitStruct.Pin = UART_SW_Pin|JVREF_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : JVREF_EN_Pin */
  GPIO_InitStruct.Pin = JVREF_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(JVREF_EN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
