/**
  @page WhiteNoiseMethod AN2668 WhiteNoiseMethod Readme File
  
  @verbatim
  ******************** (C) COPYRIGHT 2009 STMicroelectronics *******************
  * @file WhiteNoiseMethod/readme.txt 
  * @author   MCD Application Team
  * @version  V2.0.0
  * @date     04/27/2009
  * @brief    Description of the AN2668 Application note's white noise firmware.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
   @endverbatim

@par Description

This firmware gives the STM32L ADC oversampling using white noise. This method 
consists on sampling the input signal OSR times and shifting the result by the 
required additional bits. 
The timer TIM2 is used to generate the input signal sampling frequency. 
The STM32L ADC clock is configured to 16Mhz and the channel 5 is converted (PA.05).
After OSR DMA transfer from the ADC data register to a buffer in RAM, the new 
sample is computed. 

When uncommenting the line #define Thermal_Noise_Measure, the ADC thermal noise 
can be determined.
When uncommenting the line #define Oversampling_Test, the oversampled data is 
displayed on the hyperterminal and can be captured in a file for the evaluation.
results.
The Hyperterminal configuration is 115.2KB and 8-bit data with 1 stop bit on USART2.

@par Directory contents 

  - inc: containing the user header files  
     - WhiteNoiseMethod/inc/stm32l1xx_conf.h  Library Configuration files
     - WhiteNoiseMethod/inc/stm32l1xx_it.h    Interrupt handlers header files
     - WhiteNoiseMethod/inc/oversampling.h    Oversampling header file
    
  - src: containg the user source files  
    - WhiteNoiseMethod/src/oversampling.c    Oversampling source file 
    - WhiteNoiseMethod/src/stm32l1xx_it.c    Interrupt handlers
    - WhiteNoiseMethod/src/main.c            Main program

@par Hardware and Software environment 

    - This example runs on STM32L1xx High-Density, STM32L1xx Medium-Density
      Devices.
            
    - This example has been tested with STMicroelectronics STM32L152-EVAL 
      evaluations board and can be easily tailored to any other 
      supported device and development board.
      
    - Connect the input signal to be converted on ADC Channel 5: PA.05

    - Set JP13 to 3.3V position.
             
@par How to use it? 

In order to load the Oversampling White Noise Method code, you have do the following:

 - EWARM: 
    - Open the White_Noise.eww workspace
    - In the workspace toolbar select the project config:
        - STM32L152-EVAL: to configure the project for STM32L Medium-density devices
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)

 - RIDE: 
    - Open the White_Noise.rprj project
    - In the configuration toolbar(Project->properties) select the project config:
        - STM32L152-EVAL: to configure the project for STM32L Medium-density devices
    - Rebuild all files: Project->build project
    - Load project image: Debug->start(ctrl+D)
    - Run program: Debug->Run(ctrl+F9)

 - MDK-ARM: 
    - Open the White_Noise.uvproj project
    - In the build toolbar select the project config:
        - STM32L152-EVAL: to configure the project for STM32L Medium-density devices
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)

 - TrueSTUDO:
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace directory.
    - Click on File->Import, select General->'Existing Projects into Workspace' and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory, select the project:
        - STM32L152-EVAL: to configure the project for STM32L Medium-density devices
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Run->Debug (F11)

 - TASKING:
    - Open TASKING toolchain.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to  TASKING workspace directory and select the project: 
      - STM32L152-EVAL: to configure the project for STM32L Medium-density devices
     - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)
    

@note
 - Medium-density devices are STM32L1xx microcontrollers where
   the Flash memory density ranges between 64 and 128 Kbytes.
 - High-density devices are STM32L1xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.   
        
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 */
