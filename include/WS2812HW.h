#pragma once
#include <inttypes.h>
#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_dma.h>

TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_ch1;

namespace WS2812Logic
{

	volatile uint8_t PWM_HI;	// PWM Code HI Log.1 period
	volatile uint8_t PWM_LO;	// PWM Code LO Log.1 period


	#define WS2812       ///< Family: {WS2811S, WS2811F, WS2812, SK6812}
	// WS2811S — RGB, 400kHz;
	// WS2811F — RGB, 800kHz;
	// WS2812  — GRB, 800kHz;
	// SK6812  — RGBW, 800kHz
	
	#define TIM_NUM	   2  ///< Timer number
	#define TIM_CH	   TIM_CHANNEL_1  ///< Timer's PWM channel
	#define DMA_HANDLE hdma_tim2_ch1  ///< DMA Channel
	
	/// Timer handler
	#if TIM_NUM == 1
	#define TIM_HANDLE  htim1
	#elif TIM_NUM == 2
	#define TIM_HANDLE  htim2
	#elif TIM_NUM == 3
	#define TIM_HANDLE  htim3
	#elif TIM_NUM == 4
	#define TIM_HANDLE  htim4
	#elif TIM_NUM == 5
	#define TIM_HANDLE  htim5
	#elif TIM_NUM == 8
	#define TIM_HANDLE  htim8
	#else
	#error Wrong timer! Fix it in ARGB.h string 41
	#warning If you shure, set TIM_HANDLE and APB ring by yourself
	#endif
	
	/// Timer's RCC Bus
	#if TIM_NUM == 1 || (TIM_NUM >= 8 && TIM_NUM <= 11)
	#define APB1
	#else
	#define APB2
	#endif
	
	
	// enable HAL_TIM_MODULE_ENABLED
	// enable HAL_DMA_MODULE_ENABLED
	// in src\stm32f1xx_it.c >> void DMA1_Channel5_IRQHandler(void)
	
	
	
	
	
	void HAL_TIM_Base_MspInit111(TIM_HandleTypeDef* htim_base)
	{
	  if(htim_base->Instance==TIM2)
	  {
	  /* USER CODE BEGIN TIM2_MspInit 0 */
	
	  /* USER CODE END TIM2_MspInit 0 */
		/* Peripheral clock enable */
		__HAL_RCC_TIM2_CLK_ENABLE();
	
		/* TIM2 DMA Init */
		/* TIM2_CH1 Init */
		hdma_tim2_ch1.Instance = DMA1_Channel5;
		hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
		hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
		if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
		{
		  //Error_Handler();
		 // DEBUG_LOG_TOPIC("LED", "err 1\n");
		}
	
		__HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_CC1],hdma_tim2_ch1);
	
	  /* USER CODE BEGIN TIM2_MspInit 1 */
	
	  /* USER CODE END TIM2_MspInit 1 */
	  }
	
	}
	
	static void MX_TIM2_Init(void)
	{
		TIM_ClockConfigTypeDef sClockSourceConfig = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_OC_InitTypeDef sConfigOC = {0};
	
		htim2.Instance = TIM2;
		htim2.Init.Prescaler = 0;
		htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim2.Init.Period = 89;
		htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	
		HAL_TIM_Base_MspInit111(&htim2);
	
	
	
		if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
		{
			//Error_Handler();
			//DEBUG_LOG_TOPIC("LED", "err 2\n");
		}
	
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
		{
			//Error_Handler();
			//DEBUG_LOG_TOPIC("LED", "err 3\n");
		}
	
		if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
		{
			//Error_Handler();
			//DEBUG_LOG_TOPIC("LED", "err 4\n");
		}
	
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
		{
			//Error_Handler();
			//DEBUG_LOG_TOPIC("LED", "err 5\n");
		}
	
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
		{
			//Error_Handler();
			//DEBUG_LOG_TOPIC("LED", "err 6\n");
		}
	
	   // HAL_TIM_MspPostInit(&htim2);
	
	   GPIO_InitTypeDef GPIO_InitStruct = {0};
	   __HAL_RCC_GPIOA_CLK_ENABLE();
	   /**TIM2 GPIO Configuration
	   PA0-WKUP     ------> TIM2_CH1
	   */
	   GPIO_InitStruct.Pin = GPIO_PIN_0;
	   GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
	
	/**
	 * Enable DMA controller clock
	 */
	static void MX_DMA_Init(void)
	{
		// DMA controller clock enable
		__HAL_RCC_DMA1_CLK_ENABLE();
	
		// DMA interrupt init
		// DMA1_Channel5_IRQn interrupt configuration
		HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	}
	
	
	
	void DMAInit()
	{
		/* Auto-calculation! */
		uint32_t APBfq; // Clock freq
	#ifdef APB1
		APBfq = HAL_RCC_GetPCLK1Freq();
		APBfq *= (RCC->CFGR & RCC_CFGR_PPRE1) == 0 ? 1 : 2;
	#endif
	#ifdef APB2
		APBfq = HAL_RCC_GetPCLK2Freq();
		APBfq *= (RCC->CFGR & RCC_CFGR_PPRE2) == 0 ? 1 : 2;
	#endif
	#ifdef WS2811S
		APBfq /= (uint32_t) (400 * 1000);  // 400 KHz - 2.5us
	#else
		APBfq /= (uint32_t) (800 * 1000);  // 800 KHz - 1.25us
	#endif
		TIM_HANDLE.Instance->PSC = 0;                        // dummy hardcode now
		TIM_HANDLE.Instance->ARR = (uint16_t) (APBfq - 1);   // set timer prescaler
		TIM_HANDLE.Instance->EGR = 1;                        // update timer registers
	#if defined(WS2811F) || defined(WS2811S)
		PWM_HI = (uint8_t) (APBfq * 0.48) - 1;     // Log.1 - 48% - 0.60us/1.2us
		PWM_LO = (uint8_t) (APBfq * 0.20) - 1;     // Log.0 - 20% - 0.25us/0.5us
	#endif
	#ifdef WS2812
		PWM_HI = (uint8_t) (APBfq * 0.65) - 1;     // Log.1 - 56% - 0.70us
		PWM_LO = (uint8_t) (APBfq * 0.34) - 1;     // Log.0 - 28% - 0.35us
	#endif
	#ifdef SK6812
		PWM_HI = (uint8_t) (APBfq * 0.48) - 1;     // Log.1 - 48% - 0.60us
		PWM_LO = (uint8_t) (APBfq * 0.24) - 1;     // Log.0 - 24% - 0.30us
	#endif
	
		TIM_CCxChannelCmd(TIM_HANDLE.Instance, TIM_CH, TIM_CCx_ENABLE); // Enable GPIO to IDLE state
	//		ARGB_LOC_ST = ARGB_READY; // Set Ready Flag
		HAL_Delay(1); // Make some delay
			
	}
	
	#if TIM_CH == TIM_CHANNEL_1
	#define ARGB_TIM_DMA_ID TIM_DMA_ID_CC1
	#define ARGB_TIM_DMA_CC TIM_DMA_CC1
	#define ARGB_TIM_CCR CCR1
	#elif TIM_CH == TIM_CHANNEL_2
	#define ARGB_TIM_DMA_ID TIM_DMA_ID_CC2
	#define ARGB_TIM_DMA_CC TIM_DMA_CC2
	#define ARGB_TIM_CCR CCR2
	#elif TIM_CH == TIM_CHANNEL_3
	#define ARGB_TIM_DMA_ID TIM_DMA_ID_CC3
	#define ARGB_TIM_DMA_CC TIM_DMA_CC3
	#define ARGB_TIM_CCR CCR3
	#elif TIM_CH == TIM_CHANNEL_4
	#define ARGB_TIM_DMA_ID TIM_DMA_ID_CC4
	#define ARGB_TIM_DMA_CC TIM_DMA_CC4
	#define ARGB_TIM_CCR CCR4
	#endif
	
	
	
	
};
