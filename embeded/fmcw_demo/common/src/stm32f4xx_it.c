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

static uint16_t uhCaptureNumber = 0;
uint16_t uhICReadValue;
extern u16 buffer[ELEMENT_SIZE / 2 * ELEMENT_COUNT] __attribute__ ((aligned(4)));
extern ring_buf_t ring_buffer;

////////////
extern uint32_t cycle_all_ready;
extern uint32_t cycle_all;
////////////
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
extern uint32_t USBD_OTG_EP1IN_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);
#endif
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
void DMA1_Stream5_IRQHandler(void)
{
    static uint8_t chirp_index = 0;
    
    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_HTIF5))
    {
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_HTIF5);
    }
    
    if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5))
    {        
        if (NUM_CHIRPS_PER_FRAME == chirp_index++%(NUM_CHIRPS_PER_FRAME + 1))
        {
            stop_dac_timer();
            chirp_index = 0;
        }
        else
        {
            start_adc_timer();
        }
        DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
    }    
}

void DMA2_Stream0_IRQHandler(void)
{
    static uint8_t i = 1;
    static uint8_t j = 0;
    u32 current_address = 0;
    
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_HTIF0))
    {
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_HTIF0);
    }
    
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
    {
        stop_adc_timer();
        
        DMA_Cmd(DMA2_Stream0, DISABLE);
        
        if (0 == i%(NUM_CHIRPS_PER_FRAME))
        {
            j++;
        }
        
        DMA2_Stream0->M0AR = (uint32_t)(&buffer[(i++%NUM_CHIRPS_PER_FRAME) * SAMPLE_NUM_PER_CHIRP \
                                        + ((j%ELEMENT_COUNT) * NUM_CHIRPS_PER_FRAME * SAMPLE_NUM_PER_CHIRP)]);
        DMA_Cmd(DMA2_Stream0, ENABLE);
        
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
        
        if (1 == i%(NUM_CHIRPS_PER_FRAME))
        {
            ring_buffer_put(&ring_buffer);
        }
        
        current_address = DMA2_Stream0->M0AR;
    }    
}

void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5, TIM_IT_Update))
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        LED1_TOGGLE;
        start_dac_timer();
    }
}

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
        CV_LOG("uart1 rev: 0x%02X\r\n", temp);
        uart_receive_input(temp);
    }
}

/**
  * @brief  This function handles TIM1 global interrupt request.
  * @param  None
  * @retval None
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

void TIM1_CC_IRQHandler(void)
{
    static uint8_t i = 0;
    static uint32_t sum = 0;
    
    if(TIM_GetITStatus(TIM1, TIM_IT_CC1) == SET)
    {
        if(uhCaptureNumber == 0)
        {
            TIM_SetCounter(TIM1, 0);
            uhCaptureNumber = 1;
        }
        else if(uhCaptureNumber == 1)
        {
            uhICReadValue = TIM_GetCapture1(TIM1);
            uhCaptureNumber = 0;
            
            /////////////////
            sum += uhICReadValue;
            i++;
            
            if (i%2 == 0)
            {
                cycle_all = (uint32_t)(sum / 2.0f + 0.5f);
                cycle_all_ready = 1;
                sum = 0;
            }
            /////////////////
        }
        TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
    }
}

/**
  * @brief  This function handles OTG_HS Handler.
  * @param  None
  * @retval None
  */
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler(void)
#else
void OTG_FS_IRQHandler(void)
#endif
{
    USBD_OTG_ISR_Handler(&USB_OTG_dev);
}

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
/**
  * @brief  This function handles EP1_IN Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_IN_IRQHandler(void)
{
    USBD_OTG_EP1IN_ISR_Handler(&USB_OTG_dev);
}

/**
  * @brief  This function handles EP1_OUT Handler.
  * @param  None
  * @retval None
  */
void OTG_HS_EP1_OUT_IRQHandler(void)
{
    USBD_OTG_EP1OUT_ISR_Handler(&USB_OTG_dev);
}
#endif
