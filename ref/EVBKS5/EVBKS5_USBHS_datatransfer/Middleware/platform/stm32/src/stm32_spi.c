/**
  ******************************************************************************
  * @file    stm32_spi.c
  * @author  iclm team
  * @brief   spi services
  ******************************************************************************
  */
#include "stm32_spi.h"
#include "utilities.h"
#include "dataprocess.h"
#include "rtos.h"

uint8_t g_dataRecvBuf[CHANNEL_MAX][DATA_RECV_BUF_SIZE] __ALIGN(4);
volatile uint8_t g_dataRecvFlag[CHANNEL_MAX][DMA_RECV_FLAG_MAX];

//static SPI_HandleTypeDef hspi1;
static SPI_HandleTypeDef hspi3;
static SPI_HandleTypeDef hspi4;
//DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_spi3_rx;
DMA_HandleTypeDef hdma_spi4_rx;


static uint16_t curSpiFrameLen = 0;
static uint8_t recivedmaData = 0;


//static void SPI1_Init(void)
//{
//    hspi1.Instance = SPI1;
//    hspi1.Init.Mode = SPI_MODE_SLAVE;
//    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
//    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
//    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
//    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
//    hspi1.Init.NSS = SPI_NSS_HARD_INPUT;
//    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
//    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
//    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//    hspi1.Init.CRCPolynomial = 10;
//    if (HAL_SPI_Init(&hspi1) != HAL_OK)
//    {
//        RunFailed((uint8_t *)__FILE__, __LINE__);
//    }
//}

static void SPI3_Init(void)
{
    hspi3.Instance = SPI3;
    hspi3.Init.Mode = SPI_MODE_SLAVE;
    hspi3.Init.Direction = SPI_DIRECTION_2LINES;
    hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi3.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi3.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi3) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}


//static void SPI1_DeInit(void)
//{
//    if (HAL_SPI_DeInit(&hspi1) != HAL_OK)
//    {
//        RunFailed((uint8_t *)__FILE__, __LINE__);
//    }
//}

static void SPI3_DeInit(void)
{
    if (HAL_SPI_DeInit(&hspi3) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}


static void SPI4_Init(void)
{
    hspi4.Instance = SPI4;
    hspi4.Init.Mode = SPI_MODE_SLAVE;
    hspi4.Init.Direction = SPI_DIRECTION_2LINES;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi4.Init.NSS = SPI_NSS_HARD_INPUT;
    hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi4.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi4) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}

static void SPI4_DeInit(void)
{
    if (HAL_SPI_DeInit(&hspi4) != HAL_OK)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}



void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    if(spiHandle->Instance==SPI1)
//    {
//        __HAL_RCC_SPI1_CLK_ENABLE();
//        __HAL_RCC_GPIOA_CLK_ENABLE();
//        /**SPI1 GPIO Configuration    
//        PA4     ------> SPI1_NSS
//        PA5     ------> SPI1_SCK
//        PA6     ------> SPI1_MISO
//        PA7     ------> SPI1_MOSI 
//        */
//        GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
//        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//        GPIO_InitStruct.Pull = GPIO_NOPULL;
//        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
//        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//        __HAL_RCC_DMA2_CLK_ENABLE();
//        hdma_spi1_rx.Instance = DMA2_Stream0;
//        hdma_spi1_rx.Init.Channel = DMA_CHANNEL_3;
//        hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//        hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//        hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
//        hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//        hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//        hdma_spi1_rx.Init.Mode = DMA_CIRCULAR;
//        hdma_spi1_rx.Init.Priority = DMA_PRIORITY_HIGH;
//        hdma_spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//        if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
//        {
//            RunFailed((uint8_t *)__FILE__, __LINE__);
//        }

//        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);
//        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
//        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
//    }
		if(spiHandle->Instance==SPI3)
    {
        __HAL_RCC_SPI3_CLK_ENABLE();
        
			  __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**SPI3 GPIO Configuration
				PA15     ------> SPI3_NSS
				PC10     ------> SPI3_SCK
				PC11     ------> SPI3_MISO
				PC12     ------> SPI3_MOSI
				*/
        GPIO_InitStruct.Pin = GPIO_PIN_15;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

				GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
				HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
				__HAL_RCC_DMA1_CLK_ENABLE();
        hdma_spi3_rx.Instance = DMA1_Stream0;
        hdma_spi3_rx.Init.Channel = DMA_CHANNEL_0;
        hdma_spi3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi3_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi3_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi3_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi3_rx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi3_rx);
				
        HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
    }
    else if(spiHandle->Instance==SPI4)
    {
        __HAL_RCC_SPI4_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**SPI4 GPIO Configuration    
        PE11     ------> SPI4_NSS
        PE12     ------> SPI4_SCK
        PE13     ------> SPI4_MISO
        PE14     ------> SPI4_MOSI 
        */
        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        __HAL_RCC_DMA2_CLK_ENABLE();        
        hdma_spi4_rx.Instance = DMA2_Stream3;
        hdma_spi4_rx.Init.Channel = DMA_CHANNEL_5;
        hdma_spi4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi4_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi4_rx.Init.Mode = DMA_CIRCULAR;
        hdma_spi4_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi4_rx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }

        __HAL_LINKDMA(spiHandle,hdmarx,hdma_spi4_rx);
        HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
//    if(spiHandle->Instance==SPI1)
//    {
//        __HAL_RCC_SPI1_CLK_DISABLE();
//        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
//        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
//        {
//            RunFailed((uint8_t *)__FILE__, __LINE__);
//        }
//    }
		if(spiHandle->Instance==SPI3)
    {
        __HAL_RCC_SPI3_CLK_DISABLE();
			
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);
				HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12);
			
        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
        {
            RunFailed((uint8_t *)__FILE__, __LINE__);
        }
    }
    else if(spiHandle->Instance==SPI4)
    {
        __HAL_RCC_SPI4_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14);
        if (HAL_DMA_DeInit(spiHandle->hdmarx) != HAL_OK)
        {

            RunFailed((uint8_t *)__FILE__, __LINE__);
        }
    }
} 


//static void SPI1_DMA_XferCplt(DMA_HandleTypeDef *hdma)
//{
//	if(!recivedmaData)
//	{
//		return;
//	}
//    
//    if (g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_1])
//    {
//        Indicator_RadarDataRecvOverFlow();
//    }
//    g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_0] = 1;
//}
static void SPI3_DMA_XferCplt(DMA_HandleTypeDef *hdma)
{
	if(!recivedmaData)
	{
		return;
	}
	RADAR_DATA_T radarData;
    
    if (g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_1])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_0] = 1;
    
	radarData.buf = g_dataRecvBuf[CHANNEL_1];
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_0;
    
    DataProc_Send2RadarDataQueue(CHANNEL_1, &radarData);
}


//static void SPI1_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
//{
//	if(!recivedmaData)
//	{
//		return;
//	}

//    if (g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_0])
//    {
//        Indicator_RadarDataRecvOverFlow();
//    }
//    g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_1] = 1;
//}
static void SPI3_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
{
	if(!recivedmaData)
	{
		return;
	}
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_0])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_1][DMA_RECV_FLAG_MEM_1] = 1;

	radarData.buf = &(g_dataRecvBuf[CHANNEL_1][curSpiFrameLen]);
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_1;
    
    DataProc_Send2RadarDataQueue(CHANNEL_1, &radarData); 
}


//static void SPI1_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
//{
//	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
//	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
//    {
//		hspi->State = HAL_SPI_STATE_READY;
//	}
//	
//	HAL_SPI_ErrorCallback(hspi);
//} 
static void SPI3_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
    {
		hspi->State = HAL_SPI_STATE_READY;
	}
	
	HAL_SPI_ErrorCallback(hspi);
} 


static void SPI4_DMA_XferCplt(DMA_HandleTypeDef *hdma)
{
	if(!recivedmaData)
	{
		return;
	}
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0] = 1;

	radarData.buf = g_dataRecvBuf[CHANNEL_0];
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_0;
        
    DataProc_Send2RadarDataQueue(CHANNEL_0, &radarData); 
}

static void SPI4_DMA_XferM1Cplt(DMA_HandleTypeDef *hdma)
{
	if(!recivedmaData)
	{
		return;
	}
	RADAR_DATA_T radarData;

    if (g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_0])
    {
        Indicator_RadarDataRecvOverFlow();
    }
    g_dataRecvFlag[CHANNEL_0][DMA_RECV_FLAG_MEM_1] = 1;

	radarData.buf = &(g_dataRecvBuf[CHANNEL_0][curSpiFrameLen]);
	radarData.len = curSpiFrameLen;
    radarData.dmaFlag = DMA_RECV_FLAG_MEM_1;

    DataProc_Send2RadarDataQueue(CHANNEL_0, &radarData); 
}

static void SPI4_DMA_ErrorCallback(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef*  hspi = (SPI_HandleTypeDef *)(hdma->Parent);
	if (hspi->ErrorCode != HAL_DMA_ERROR_FE) 
    {
		hspi->State = HAL_SPI_STATE_READY;
	}
	
	HAL_SPI_ErrorCallback(hspi);
} 



static void SPI_DMA_Enable(uint16_t bufSize, uint8_t dataType)
{
    uint32_t srcAddr = 0;
    
    if (bufSize > (DATA_RECV_BUF_SIZE >> 1))
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
    
//    hspi1.hdmarx->XferCpltCallback = SPI1_DMA_XferCplt;
//    hspi1.hdmarx->XferM1CpltCallback = SPI1_DMA_XferM1Cplt;
//    hspi1.hdmarx->XferErrorCallback = SPI1_DMA_ErrorCallback;

//    srcAddr = (uint32_t)&(hspi1.Instance->DR);
//    __HAL_UNLOCK(hspi1.hdmarx);
//    if (HAL_DMAEx_MultiBufferStart_IT(hspi1.hdmarx, srcAddr, 
//        (uint32_t)g_dataRecvBuf[CHANNEL_1], (uint32_t)&(g_dataRecvBuf[CHANNEL_1][bufSize]),
//        (uint32_t)bufSize) != HAL_OK) 
//    {
//        RunFailed((uint8_t *)__FILE__, __LINE__);
//    }

//    if((hspi1.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
//    {
//        __HAL_SPI_ENABLE(&hspi1);
//    }

//    SET_BIT(hspi1.Instance->CR2, SPI_CR2_ERRIE);
//    SET_BIT(hspi1.Instance->CR2, SPI_CR2_RXDMAEN); 
  
    /*================================================================*/		
		
    hspi3.hdmarx->XferCpltCallback = SPI3_DMA_XferCplt;
    hspi3.hdmarx->XferM1CpltCallback = SPI3_DMA_XferM1Cplt;
    hspi3.hdmarx->XferErrorCallback = SPI3_DMA_ErrorCallback;

    srcAddr = (uint32_t)&(hspi3.Instance->DR);
    __HAL_UNLOCK(hspi3.hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(hspi3.hdmarx, srcAddr, 
        (uint32_t)g_dataRecvBuf[CHANNEL_1], (uint32_t)&(g_dataRecvBuf[CHANNEL_1][bufSize]),
        (uint32_t)bufSize) != HAL_OK) 
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    if((hspi3.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        __HAL_SPI_ENABLE(&hspi3);
    }

    SET_BIT(hspi3.Instance->CR2, SPI_CR2_ERRIE);
    SET_BIT(hspi3.Instance->CR2, SPI_CR2_RXDMAEN); 

    /*================================================================*/		
	  hspi4.hdmarx->XferCpltCallback = SPI4_DMA_XferCplt;
    hspi4.hdmarx->XferM1CpltCallback = SPI4_DMA_XferM1Cplt;
    hspi4.hdmarx->XferErrorCallback = SPI4_DMA_ErrorCallback;
    srcAddr = (uint32_t)&(hspi4.Instance->DR);

    __HAL_UNLOCK(hspi4.hdmarx);
    if (HAL_DMAEx_MultiBufferStart_IT(hspi4.hdmarx, srcAddr, 
        (uint32_t)g_dataRecvBuf[CHANNEL_0], (uint32_t)&(g_dataRecvBuf[CHANNEL_0][bufSize]),
        (uint32_t)bufSize) != HAL_OK) 
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    if((hspi4.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
    {
        __HAL_SPI_ENABLE(&hspi4);
    }
 
    SET_BIT(hspi4.Instance->CR2, SPI_CR2_ERRIE);
    SET_BIT(hspi4.Instance->CR2, SPI_CR2_RXDMAEN); 
}


void SPI_Init(uint16_t dmaBufLen, uint8_t dataType)
{ 
    //SPI1_Init(); 
	  SPI3_Init();
    SPI4_Init(); 
	
    curSpiFrameLen = dmaBufLen >> 1;
    SPI_DMA_Enable(curSpiFrameLen, dataType);
  	recivedmaData = 1;
}

void SPI_DeInit(void)
{
	  recivedmaData = 0;
    //SPI1_DeInit(); 
	  SPI3_DeInit(); 
    SPI4_DeInit(); 
}

