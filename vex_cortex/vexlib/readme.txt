/******************** (C) COPYRIGHT 2010 VEX ********************
* File Name          : readme.txt
* Author             : Application Team
* Version            : V2.0.3
* Date               : 02/22/2010
* Description        : This sub-directory contains all the user-modifiable files 
*                      needed to create a new project linked with the STM32F10x  
*                      Firmware Library and working with RIDE7 software toolchain 
*                      (RIDE7 IDE:7.02.0001, RKitARM for RIDE7:1.04.0001)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

Source files are located in the Source directory

Directory contents
===================
- project .rprj/.rapp: A pre-configured project file with the provided library 
                       structure that produces an executable image with RIDE7.
                
- stm32f10x_vector.c: This file contains the vector table for STM32F10x.
                      User can also enable the use of external SRAM mounted on 
                      STM3210E-EVAL board as data memory.
                      
- cortexm3_macro.s: Instruction wrappers for special Cortex-M3 instructions. 

- ProjectFiles.JPG - complete listing of GCC files to build Vex_Default.hex

      
How to use it
=============
- Open the Project.rprj project.
- Rebuild all files: Project->build project
- Load project image: Debug->start(ctrl+D)
- Run program: Debug->Run(ctrl+F9)  

NOTE:
 - Medium-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 32 and 128 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes. 

******************* (C) COPYRIGHT 2010 VEX *****END OF FILE******
