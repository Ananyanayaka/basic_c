
/******************************************************************/
/* Core M7_0 startup code                                         */
/******************************************************************/
.globl OsCfg_Hal_Core_OsCore0_VectorTable
.section ".startup_c0","ax"
.syntax unified
.thumb

.set VTOR_REG, 0xE000ED08
.align 2
.globl CATCH_CORE_RESET

.globl Reset_Handler 
Reset_Handler:
.globl _start
_start:
/********************************************/
/* Clear General Purpose Registers          */
/********************************************/
 mov   r0,  #0
 mov   r1,  #0
 mov   r2,  #0
 mov   r3,  #0
 mov   r4,  #0
 mov   r5,  #0
 mov   r6,  #0
 mov   r7,  #0
 mov   r8,  #0
 mov   r9,  #0
 mov   r10, #0
 mov   r11, #0
 
/*******************************************/
/* Set vector table address in RAM         */
/*******************************************/
ldr  r0, =VTOR_REG
ldr  r1, =OsCfg_Hal_Core_OsCore0_VectorTable
ldr  r2, =(1 << 29)
orr  r1, r2 /* r1 = r1 | r2 */
str  r1,[r0]

/*******************************************/
/* Set stack pointer                       */
/*******************************************/
ldr  r0, =__StackTop
msr MSP, r0

/*******************************************/
/* FP register setup                       */
/*******************************************/
bl SystemInit

/*******************************************/
/* Initialize TCM region                   */
/*******************************************/
  ldr  r2, =0x20000000
  ldr  r3, =0x20010000

FillZeroTCM_c0:
  stm   r2!,{r4-r11}
  cmp   r2, r3
  bcc   FillZeroTCM_c0

/*******************************************/
/* Call Main Routine                       */
/*******************************************/
bl main


/******************************************************************/
/* Core M7_1 startup code                                         */
/******************************************************************/
.globl OsCfg_Hal_Core_OsCore1_VectorTable
.section ".startup_c1","ax"
.thumb

.set VTOR_REG, 0xE000ED08
.align 2
.globl CATCH_CORE_RESET

.globl Reset_Handler_Core1 
Reset_Handler_Core1:
.globl _start_c1
_start_c1:
/*******************************************/
/* Clear General Purpose Registers         */
/*******************************************/
 mov   r0,  #0
 mov   r1,  #0
 mov   r2,  #0
 mov   r3,  #0
 mov   r4,  #0
 mov   r5,  #0
 mov   r6,  #0
 mov   r7,  #0
 mov   r8,  #0
 mov   r9,  #0
 mov   r10, #0
 mov   r11, #0
 
/*******************************************/
/* Set vector table address in RAM         */
/*******************************************/
ldr  r0, =VTOR_REG
ldr  r1, =OsCfg_Hal_Core_OsCore1_VectorTable
ldr  r2, =(1 << 29)
orr  r1, r2 /* r1 = r1 | r2 */
str  r1,[r0]

/*******************************************/
/* Set stack pointer                       */
/*******************************************/
ldr  r0, =__StackTop_c1
msr MSP, r0

/*******************************************/
/* FP register setup                       */
/*******************************************/
bl SystemInit

/*******************************************/
/* Initialize TCM region                   */
/*******************************************/
  ldr  r2, =0x20000000
  ldr  r3, =0x20010000

FillZeroTCM_c1:
  stm   r2!,{r4-r11}
  cmp   r2, r3
  bcc   FillZeroTCM_c1

/*******************************************/
/* Call Main Routine                       */
/*******************************************/
bl main_c1


/******************************************************************/
/* Core M7_2 startup code                                         */
/******************************************************************/
.globl OsCfg_Hal_Core_OsCore2_VectorTable
.section ".startup_c2","ax"
.thumb

.set VTOR_REG, 0xE000ED08
.align 2
.globl CATCH_CORE_RESET

.globl Reset_Handler_Core2
Reset_Handler_Core2:
.globl _start_c2
_start_c2:
/*******************************************/
/* Clear General Purpose Registers         */
/*******************************************/
 mov   r0,  #0
 mov   r1,  #0
 mov   r2,  #0
 mov   r3,  #0
 mov   r4,  #0
 mov   r5,  #0
 mov   r6,  #0
 mov   r7,  #0
 mov   r8,  #0
 mov   r9,  #0
 mov   r10, #0
 mov   r11, #0
 
/*******************************************/
/* Set vector table address in RAM         */
/*******************************************/
ldr  r0, =VTOR_REG
ldr  r1, =OsCfg_Hal_Core_OsCore2_VectorTable
ldr  r2, =(1 << 29)
orr  r1, r2 /* r1 = r1 | r2 */
str  r1,[r0]

/*******************************************/
/* Set stack pointer                       */
/*******************************************/
ldr  r0, =__StackTop_c2
msr MSP, r0

/*******************************************/
/* FP register setup                       */
/*******************************************/
bl SystemInit

/*******************************************/
/* Initialize TCM region                   */
/*******************************************/
  ldr  r2, =0x20000000
  ldr  r3, =0x20010000

FillZeroTCM_c2:
  stm   r2!,{r4-r11}
  cmp   r2, r3
  bcc   FillZeroTCM_c2

/*******************************************/
/* Call Main Routine                       */
/*******************************************/
bl main_c2
