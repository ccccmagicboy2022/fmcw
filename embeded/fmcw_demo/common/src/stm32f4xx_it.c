/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "sys.h"

uint16_t uhIC3ReadValue1 = 0;
uint16_t uhIC3ReadValue2 = 0;
uint16_t uhCaptureNumber = 0;
uint32_t uwCapture = 0;
float uwTIM8Freq = 0.0f;

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  //TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @brief  uart1 rev handler
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
    char temp;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        temp = USART_ReceiveData(USART1);
        uart_receive_input(temp);
    }
}

/**
  * @brief  This function handles TIM8 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM8_CC_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM8, TIM_IT_CC1) == SET)
    {
        /* Clear TIM8 Capture compare interrupt pending bit */
        TIM_ClearITPendingBit(TIM8, TIM_IT_CC1);
        if(uhCaptureNumber == 0)
        {
            /* Get the Input Capture value */
            uhIC3ReadValue1 = TIM_GetCapture2(TIM8);
            uhCaptureNumber = 1;
        }
        else if(uhCaptureNumber == 1)
        {
            /* Get the Input Capture value */
            uhIC3ReadValue2 = TIM_GetCapture2(TIM8);

            /* Capture computation */
            if (uhIC3ReadValue2 > uhIC3ReadValue1)
            {
                uwCapture = (uhIC3ReadValue2 - uhIC3ReadValue1);
            }
            else if (uhIC3ReadValue2 < uhIC3ReadValue1)
            {
                uwCapture = ((0xFFFF - uhIC3ReadValue1) + uhIC3ReadValue2);
            }
            else
            {
                uwCapture = 0;
            }
            /* Frequency computation */
            uwTIM8Freq = SystemCoreClock / uwCapture;
            uhCaptureNumber = 0;
        }
    }
}

