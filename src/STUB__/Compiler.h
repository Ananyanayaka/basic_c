// It is here just to satisfy include dependency of Platform_Types.h from adas_platforms package (needed starting from the release 6.0.9).

// Currently the below part is used only for the Ti_C66 compiler in AWR2944 variant
//   Below part is simply copied from the adas_platforms file adas_platforms\src\TI_C6600\Compiler_Cfg.h from release 5.8.10

#if (defined(C6600))

// File should come from MCAL component Platforms
#ifndef STUBS_TS_PLATFORMS_H
#define STUBS_TS_PLATFORMS_H

//////////////////////////CSI*/////////////////////////////
/*****************************************************************************/
/* COMPILER_CFG.H v16.9.1                                                    */
/*                                                                           */
/* Copyright (c) 2008-2016 Texas Instruments Incorporated                    */
/* http://www.ti.com/                                                        */
/*                                                                           */
/*  Redistribution and  use in source  and binary forms, with  or without    */
/*  modification,  are permitted provided  that the  following conditions    */
/*  are met:                                                                 */
/*                                                                           */
/*     Redistributions  of source  code must  retain the  above copyright    */
/*     notice, this list of conditions and the following disclaimer.         */
/*                                                                           */
/*     Redistributions in binary form  must reproduce the above copyright    */
/*     notice, this  list of conditions  and the following  disclaimer in    */
/*     the  documentation  and/or   other  materials  provided  with  the    */
/*     distribution.                                                         */
/*                                                                           */
/*     Neither the  name of Texas Instruments Incorporated  nor the names    */
/*     of its  contributors may  be used to  endorse or  promote products    */
/*     derived  from   this  software  without   specific  prior  written    */
/*     permission.                                                           */
/*                                                                           */
/*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS    */
/*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT    */
/*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR    */
/*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT    */
/*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    */
/*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT    */
/*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,    */
/*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY    */
/*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE    */
/*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     */
/*                                                                           */
/*****************************************************************************/
#ifndef _COMPILER_CFG_H_
#define _COMPILER_CFG_H_

/*****************************************************************************/
/* This is an empty header file which should be replaced by the end user.    */
/*                                                                           */
/* According to the AUTOSAR document "Specification of Compiler Abstraction" */
/* version 2.0.1, as part of AUTOSAR release 3.1, COMPILER55:                */
/*   The file Compiler_Cfg.h shall contain the module specific parameters    */
/*   (ptrclass and memclass) that are passed to the macros defined in        */
/*   Compiler.h.                                                             */
/*                                                                           */
/* The following is the form of these macro definitions.  MSN stands for     */
/* "Module Short Name of BSW module list"                                    */
/*                                                                           */
/* #define <MSN>_CODE                                                        */
/* #define <MSN>_CONST                                                       */
/* #define <MSN>_APPL_DATA                                                   */
/* #define <MSN>_APPL_CONST                                                  */
/* #define <MSN>_APPL_CODE                                                   */
/* #define <MSN>_VAR_NOINIT                                                  */
/* #define <MSN>_VAR_POWER_ON_INIT                                           */
/* #define <MSN>_VAR_FAST                                                    */
/* #define <MSN>_VAR                                                         */
/*                                                                           */
/* NOTE: On 32 bit architectures these definitions are required to be empty  */
/*****************************************************************************/

//////////////////CSI///////////////////
//#define BswM_CanSM_CurrentState //this was added due to dependencies between CANSM and BSWM
//#define BswM_ComM_CurrentMode   //this was added due to dependencies between COMM and BSWM
//this will need to be dropped once BSWM is generated by Tresos
//////////////////CSI///////////////////


#define COM_APPL_CONST
#define COM_APPL_DATA



#define ADC_CODE
#define ADC_CONST
#define ADC_APPL_DATA
#define ADC_APPL_CONST
#define ADC_APPL_CODE
#define ADC_CALLOUT_CODE
#define ADC_VAR_NOINIT
#define ADC_VAR_POWER_ON_INIT
#define ADC_VAR_FAST
#define ADC_VAR
/**@}*/

/**
@{
* @brief CAN memory and pointer classes.
* @implements DBASE04001
*/
#define CAN_CODE
#define CAN_CONST
#define CAN_APPL_DATA
#define CAN_APPL_CONST
#define CAN_APPL_CODE
#define CAN_CALLOUT_CODE
#define CAN_VAR_NOINIT
#define CAN_VAR_POWER_ON_INIT
#define CAN_VAR_FAST
#define CAN_VAR
/**@}*/

/**
@{
* @brief CANIF memory and pointer classes.
* @implements DBASE04001
*/
#define CANIF_CODE
#define CANIF_CONST
#define CANIF_APPL_DATA
#define CANIF_APPL_CONST
#define CANIF_APPL_CODE
#define CANIF_CALLOUT_CODE
#define CANIF_VAR_NOINIT
#define CANIF_VAR_POWER_ON_INIT
#define CANIF_VAR_FAST
#define CANIF_VAR
/**@}*/

/**
@{
* @brief CRCU memory and pointer classes.
* @implements DBASE04001
*/
#define CRCU_CODE
#define CRCU_CONST
#define CRCU_APPL_DATA
#define CRCU_APPL_CONST
#define CRCU_APPL_CODE
#define CRCU_CALLOUT_CODE
#define CRCU_VAR_NOINIT
#define CRCU_VAR_POWER_ON_INIT
#define CRCU_VAR_FAST
#define CRCU_VAR
/**@}*/
/**
@{
* @brief DEM memory and pointer classes.
* @implements DBASE04001
*/
#define DEM_CODE
#define DEM_CONST
#define DEM_APPL_DATA
#define DEM_APPL_CONST
#define DEM_APPL_CODE
#define DEM_CALLOUT_CODE
#define DEM_VAR_NOINIT
#define DEM_VAR_POWER_ON_INIT
#define DEM_VAR_FAST
#define DEM_VAR
/**@}*/

/**
@{
* @brief DET memory and pointer classes.
* @implements DBASE04001
*/
#define DET_CODE
#define DET_CONST
#define DET_APPL_DATA
#define DET_APPL_CONST
#define DET_APPL_CODE
#define DET_CALLOUT_CODE
#define DET_VAR_NOINIT
#define DET_VAR_POWER_ON_INIT
#define DET_VAR_FAST
#define DET_VAR
/**@}*/

/**
@{
* @brief DIO memory and pointer classes.
* @implements DBASE04001
*/
#define DIO_CODE
#define DIO_CONST
#define DIO_APPL_DATA
#define DIO_APPL_CONST
#define DIO_APPL_CODE
#define DIO_CALLOUT_CODE
#define DIO_VAR_NOINIT
#define DIO_VAR_POWER_ON_INIT
#define DIO_VAR_FAST
#define DIO_VAR
/**@}*/

/**
@{
* @brief ETH memory and pointer classes.
* @implements DBASE04001
*/
#define ETH_CODE
#define ETH_CONST
#define ETH_APPL_DATA
#define ETH_APPL_CONST
#define ETH_APPL_CODE
#define ETH_CALLOUT_CODE
#define ETH_VAR_NOINIT
#define ETH_VAR_POWER_ON_INIT
#define ETH_VAR_FAST
#define ETH_VAR
/**@}*/



/**
@{
* @brief FEE memory and pointer classes.
* @implements DBASE04001
*/
#define FEE_CODE
#define FEE_CONST
#define FEE_APPL_DATA
#define FEE_APPL_CONST
#define FEE_APPL_CODE
#define FEE_CALLOUT_CODE
#define FEE_VAR_NOINIT
#define FEE_VAR_POWER_ON_INIT
#define FEE_VAR_FAST
#define FEE_VAR
/**@}*/

/**
@{
* @brief FLS memory and pointer classes.
* @implements DBASE04001
*/
#define FLS_CODE
#define FLS_CONST
#define FLS_APPL_DATA
#define FLS_APPL_CONST
#define FLS_APPL_CODE
#define FLS_CALLOUT_CODE
#define FLS_VAR_NOINIT
#define FLS_VAR_POWER_ON_INIT
#define FLS_VAR_FAST
#define FLS_VAR
/**@}*/

/**
@{
* @brief FlexRay memory and pointer classes.
* @implements DBASE04001
*/
#define FR_CODE
#define FR_CONST
#define FR_APPL_DATA
#define FR_APPL_CONST
#define FR_APPL_CODE
#define FR_CALLOUT_CODE
#define FR_VAR_NOINIT
#define FR_VAR_POWER_ON_INIT
#define FR_VAR_FAST
#define FR_VAR
/**@}*/

/**
@{
* @brief GPT memory and pointer classes.
* @implements DBASE04001
*/
#define GPT_CODE
#define GPT_CONST
#define GPT_APPL_DATA
#define GPT_APPL_CONST
#define GPT_APPL_CODE
#define GPT_CALLOUT_CODE
#define GPT_VAR_NOINIT
#define GPT_VAR_POWER_ON_INIT
#define GPT_VAR_FAST
#define GPT_VAR
/**@}*/

/**
@{
* @brief ICU memory and pointer classes.
* @implements DBASE04001
*/
#define ICU_CODE
#define ICU_CONST
#define ICU_APPL_DATA
#define ICU_APPL_CONST
#define ICU_APPL_CODE
#define ICU_CALLOUT_CODE
#define ICU_VAR_NOINIT
#define ICU_VAR_POWER_ON_INIT
#define ICU_VAR_FAST
#define ICU_VAR
/**@}*/

/**
@{
* @brief LIN memory and pointer classes.
* @implements DBASE04001
*/
#define LIN_CODE
#define LIN_CONST
#define LIN_APPL_DATA
#define LIN_APPL_CONST
#define LIN_APPL_CODE
#define LIN_CALLOUT_CODE
#define LIN_VAR_NOINIT
#define LIN_VAR_POWER_ON_INIT
#define LIN_VAR_FAST
#define LIN_VAR
/**@}*/

/**
@{
* @brief MCEM memory and pointer classes.
* @implements
*/
#define MCEM_CODE
#define MCEM_CONST
#define MCEM_APPL_DATA
#define MCEM_APPL_CONST
#define MCEM_APPL_CODE
#define MCEM_CALLOUT_CODE
#define MCEM_VAR_NOINIT
#define MCEM_VAR_POWER_ON_INIT
#define MCEM_VAR_FAST
#define MCEM_VAR
/**@}*/

/**
@{
* @brief MCL memory and pointer classes.
* @implements
*/
#define MCL_CODE
#define MCL_CONST
#define MCL_APPL_DATA
#define MCL_APPL_CONST
#define MCL_APPL_CODE
#define MCL_CALLOUT_CODE
#define MCL_VAR_NOINIT
#define MCL_VAR_POWER_ON_INIT
#define MCL_VAR_FAST
#define MCL_VAR
/**@}*/

/**
@{
* @brief MCU memory and pointer classes.
* @implements DBASE04001
*/
#define MCU_CODE
#define MCU_CONST
#define MCU_APPL_DATA
#define MCU_APPL_CONST
#define MCU_APPL_CODE
#define MCU_CALLOUT_CODE
#define MCU_VAR_NOINIT
#define MCU_VAR_POWER_ON_INIT
#define MCU_VAR_FAST
#define MCU_VAR
/**@}*/

/**
@{
* @brief PORT memory and pointer classes.
* @implements DBASE04001
*/
#define PORT_CODE
#define PORT_CONST
#define PORT_APPL_DATA
#define PORT_APPL_CONST
#define PORT_APPL_CODE
#define PORT_CALLOUT_CODE
#define PORT_VAR_NOINIT
#define PORT_VAR_POWER_ON_INIT
#define PORT_VAR_FAST
#define PORT_VAR
/**@}*/

/**
@{
* @brief PWM memory and pointer classes.
* @implements DBASE04001
*/
#define PWM_CODE
#define PWM_CONST
#define PWM_APPL_DATA
#define PWM_APPL_CONST
#define PWM_APPL_CODE
#define PWM_CALLOUT_CODE
#define PWM_VAR_NOINIT
#define PWM_VAR_POWER_ON_INIT
#define PWM_VAR_FAST
#define PWM_VAR
/**@}*/


/**
@{
* @brief RamTST memory and pointer classes.
* @implements DBASE04001
*/
#define RAMTST_CODE
#define RAMTST_CONST
#define RAMTST_APPL_DATA
#define RAMTST_APPL_CONST
#define RAMTST_APPL_CODE
#define RAMTST_CALLOUT_CODE
#define RAMTST_VAR_NOINIT
#define RAMTST_VAR_POWER_ON_INIT
#define RAMTST_VAR_FAST
#define RAMTST_VAR
/**@}*/

/**
@{
* @brief SchM memory and pointer classes.
* @implements DBASE04001
*/
#define SCHM_CODE
#define SCHM_CONST
#define SCHM_APPL_DATA
#define SCHM_APPL_CONST
#define SCHM_APPL_CODE
#define SCHM_CALLOUT_CODE
#define SCHM_VAR_NOINIT
#define SCHM_VAR_POWER_ON_INIT
#define SCHM_VAR_FAST
#define SCHM_VAR
/**@}*/

/**
@{
* @brief SPI memory and pointer classes.
* @implements DBASE04001
*/
#define SPI_CODE
#define SPI_CONST
#define SPI_APPL_DATA
#define SPI_APPL_CONST
#define SPI_APPL_CODE
#define SPI_CALLOUT_CODE
#define SPI_VAR_NOINIT
#define SPI_VAR_POWER_ON_INIT
#define SPI_VAR_FAST
#define SPI_VAR
/**@}*/

/**
@{
* @brief WDG memory and pointer classes.
* @implements DBASE04001
*/
#define WDG_CODE
#define WDG_CONST
#define WDG_APPL_DATA
#define WDG_APPL_CONST
#define WDG_APPL_CODE
#define WDG_CALLOUT_CODE
#define WDG_VAR_NOINIT
#define WDG_VAR_POWER_ON_INIT
#define WDG_VAR_FAST
#define WDG_VAR
/**@}*/

/**
@{
* @brief WDGIF memory and pointer classes.
* @implements DBASE04001
*/
#define WDGIF_CODE
#define WDGIF_CONST
#define WDGIF_APPL_DATA
#define WDGIF_APPL_CONST
#define WDGIF_APPL_CODE
#define WDGIF_CALLOUT_CODE
#define WDGIF_VAR_NOINIT
#define WDGIF_VAR_POWER_ON_INIT
#define WDGIF_VAR_FAST
#define WDGIF_VAR
/**@}*/

/**
@{
* @brief Specify cache line size in bytes at compile time for memory alignment
*/
#define CACHE_LINE_SIZE_R5  16U
#define CACHE_LINE_SIZE_C66 64U
#define CACHE_LINE_SIZE_OWN_CORE CACHE_LINE_SIZE_C66
/**@}*/

#ifdef __cplusplus

    #define PRAGMA(x) _Pragma(#x)

    // Macros for linking code into a specific section
    #define SECTION_CODE_DECL(x)
    #define SECTION_CODE_IMPL(x) PRAGMA(CODE_SECTION(x))

    // Macros for linking data into a specific section
    #define SECTION_DATA_DECL(x)
    #define SECTION_DATA_IMPL(x) PRAGMA(DATA_SECTION(x))

    // Macros for aligning a section
    #define SECTION_ALIGN_DECL(x)
    #define SECTION_ALIGN_IMPL(x) PRAGMA(DATA_ALIGN(x))

#else

    #define PRAGMA(x) _Pragma(#x)

    // Macros for linking code into a specific section
    #define SECTION_CODE_DECL(x)
    #define SECTION_CODE_IMPL(x, y) PRAGMA(CODE_SECTION(x, y))

    // Macros for linking data into a specific section
    #define SECTION_DATA_DECL(x)
    #define SECTION_DATA_IMPL(x, y) PRAGMA(DATA_SECTION(x, y))

    // Macros for aligning a section
    #define SECTION_ALIGN_DECL(x)
    #define SECTION_ALIGN_IMPL(x, y) PRAGMA(DATA_ALIGN(x, y))

#endif // __cplusplus


#endif /* _COMPILER_CFG_H_ */

#endif // #ifndef STUBS_TS_PLATFORMS_H

// Reviewer: uie71896, 2023.10.18,
// Summary: Msg(3:0883) Include file code is not protected against repeated inclusion.
// Reason: Intended behaviour, platform dependent guarding exists in place.
// PRQA S 0883 1
#endif // C6600
