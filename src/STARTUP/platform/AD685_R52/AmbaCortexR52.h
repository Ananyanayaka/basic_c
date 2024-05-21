/*
* @file AmbaCortexBase.h
*
* Copyright (c) 2023 Ambarella International LP
*
* This file and its contents ("Software") are protected by intellectual
* property rights including, without limitation, U.S. and/or foreign
* copyrights. This Software is also the confidential and proprietary
* information of Ambarella International LP and its licensors. You may not use, reproduce,
* disclose, distribute, modify, or otherwise prepare derivative works of this
* Software or any portion thereof except pursuant to a signed license agreement
* or nondisclosure agreement with Ambarella International LP or its authorized affiliates.
* In the absence of such an agreement, you agree to promptly notify and return
* this Software to Ambarella International LP.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
* MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL AMBARELLA INTERNATIONAL LP OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef AMBA_CORTEX_R52_H
#define AMBA_CORTEX_R52_H


#define AMBA_TCM0_BASE_ADDR                         0xff000000U
#define AMBA_TCM0_BASE_ADDR_END                     (AMBA_TCM0_BASE_ADDR + 0x20000U)
#define AMBA_TCM1_BASE_ADDR                         0xff020000U
#define AMBA_TCM1_BASE_ADDR_END                     (AMBA_TCM1_BASE_ADDR + 0x20000U)
#define AMBA_TCM2_BASE_ADDR                         0xff040000U
#define AMBA_TCM2_BASE_ADDR_END                     (AMBA_TCM2_BASE_ADDR + 0x20000U)

#define AMBA_SHMEM_BASE_ADDR                        0x7fc00000U                                             /* SHMEM, 1MB */
#define AMBA_SIMEM_BASE_ADDR                        0x7fd00000U                                             /* SIMEM, 2048KB */

#define AMBA_PCIE0_BASE_ADDR                        0x80000000U                                             /* SHMEM, 1MB */
#define AMBA_PCIE1_BASE_ADDR                        0xA0000000U                                             /* SIMEM, 3008KB */
#define AMBA_PCIE2_BASE_ADDR                        0xC0000000U                                             /* SHMEM, 1MB */

/* map select 0x0 */
#define AMBA_CORTEX_R52_DRAM_CFG_PHYS_BASE_ADDR     0xe8000000U                                             /* DRAM Configuration Base Address 1MB */

/* map select 0x1 */
#define AMBA_CORTEX_r52_PCIE_PHY_BASE_ADDR          0xe0000000U                                             /* PCIE phy0 Base Address 1MB */
#define AMBA_CORTEX_r52_PCIE_PHY1_2_BASE_ADDR       0xe0100000U                                             /* PCIE phy1_2 Base Address 1MB */

/* map select 0x2 */
#define AMBA_CORTEX_R52_PCIE_CONFIG_BASE_ADDR       0xe0000000U                                             /* PCIE config0 Base Address 64MB */
#define AMBA_CORTEX_R52_PCIE_CONFIG1_BASE_ADDR      0xe4000000U                                             /* PCIE config1 Base Address 64MB */
#define AMBA_CORTEX_R52_PCIE_CONFIG2_BASE_ADDR      0xe8000000U                                             /* PCIE config2 Base Address 64MB */
#define AMBA_CORTEX_R52_USB32_PHY_BASE_ADDR         0xec000000U                                             /* USB32 phy Base Address 64MB */

/* map select 0x3 */
#define AMBA_CORTEX_R52_CPHY_BASE_ADDR              0xe0000000U                                             /* CPHY Base Address 128MB */

/* map select 0x4 */
#define AMBA_CORTEX_R52_AHB_PHYS_BASE_ADDR          0xe0000000U                                             /* AHB Mapped Peripherals, same as 0xff_e000_0000, 64MB */
#define AMBA_CORTEX_R52_APB_PHYS_BASE_ADDR          0xe4000000U                                             /* APB Mapped Peripherals, same as 0xff_e400_0000, 64MB */
#define AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR     AMBA_CORTEX_R52_DRAM_CFG_PHYS_BASE_ADDR                 /* DRAM Configuration */
#define AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR          AMBA_CORTEX_R52_AHB_PHYS_BASE_ADDR                      /* Non-Secure AHB Mapped Peripherals */
#define AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR          AMBA_CORTEX_R52_APB_PHYS_BASE_ADDR                      /* Non-Secure APB Mapped Peripherals */

#define AMBA_CORTEX_R52_NIC_SI_PHYS_BASE_ADDR       0xf0000000U                                             /* NIC-400 SI Base Address 1MB */
#define AMBA_CORTEX_R52_NIC_PCIE0_PHYS_BASE_ADDR    0xf0300000U                                             /* NIC-400 PCIE0 Base Address 1MB */
#define AMBA_CORTEX_R52_NIC_PCIE1_PHYS_BASE_ADDR    0xf0400000U                                             /* NIC-400 PCIE1 Base Address 1MB */
#define AMBA_CORTEX_R52_NIC_PCIE2_PHYS_BASE_ADDR    0xf0500000U                                             /* NIC-400 PCIE2 Base Address 1MB */
#define AMBA_CORTEX_R52_NIC_PERI_PHYS_BASE_ADDR     0xf1000000U                                             /* NIC-400 PERI Base Address 1MB */
#define AMBA_CORTEX_R52_NIC_CR52_PHYS_BASE_ADDR     0xffe00000U                                             /* NIC-400 CR52 Base Address 1MB */

#define AMBA_CORTEX_R52_SD0_BASE_ADDR               (0xF2000000U)                                           /* SD Host */
#define AMBA_CORTEX_R52_SD1_BASE_ADDR               (0xF2001000U)                                           /* SD Host */
#define AMBA_CORTEX_R52_SD2_BASE_ADDR               (0xF2002000U)                                           /* SD Host */
//#define AMBA_CORTEX_R52_SDIO0_BASE_ADDR             (AMBA_CORTEX_A78_SD_BASE_ADDR + 0x01000UL)        //SDIO Host 0
//#define AMBA_CORTEX_R52_SDIO1_BASE_ADDR             (AMBA_CORTEX_A78_SD_BASE_ADDR + 0x02000UL)        //SDIO Host 1
//#define AMBA_CORTEX_R52_CPHY0_BASE_ADDR             (AMBA_CORTEX_A78_CPHY_BASE_ADDR)                  // CPHY0
//#define AMBA_CORTEX_R52_CPHY1_BASE_ADDR             (AMBA_CORTEX_A78_CPHY_BASE_ADDR + 0x10000UL)      // CPHY1

#define AMBA_CORTEX_R52_AXI_CONFIG_PHYS_BASE_ADDR   0xf3000000U                                             /* AXI Configuration Base Address 64KB */
#define AMBA_CORTEX_R52_APB_WDT_BASE_ADDR           0xf4000000U                                             /* WDT phy Base Address 1MB */
#define AMBA_CORTEX_R52_MSI_DETECT0_BASE_ADDR       0xf4010000U                                             /* MSI DETECT0 phy Base Address */
#define AMBA_CORTEX_R52_MSI_DETECT1_BASE_ADDR       0xf4020000U                                             /* MSI DETECT1 phy Base Address */
#define AMBA_CORTEX_R52_MSI_DETECT2_BASE_ADDR       0xf4030000U                                             /* MSI DETECT2 phy Base Address */
#define AMBA_CORTEX_R52_GPV_PHYS_BASE_ADDR          0xf5000000U                                             /* GPV for NIC-400 A Configuration Base Address 1MB */
#define AMBA_CORTEX_R52_GIC_PHYS_BASE_ADDR          0xf7000000U                                             /* GIC Base Address 16MB */
#define AMBA_CORTEX_R52_SI_HSM_BASE_ADDR            0xff060000U                                             /* GIC Base Address 4KB */

#define AMBA_CORTEX_R52_AXI_CONFIG_VIRT_BASE_ADDR   AMBA_CORTEX_R52_AXI_CONFIG_PHYS_BASE_ADDR               /* AXI Configuration Base Address */

#define AMBA_CR52_DRAM_CTRL_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR)               /* DRAM Controller */
#define AMBA_CR52_DDR_CTRL_ALL_BASE_ADDR            (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x8000U)     /* DDR SDRAM Controller Broadcast */
#define AMBA_CR52_DDR_CTRL0_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x14000U)    /* DDR SDRAM Controller-0 */
#define AMBA_CR52_DDR_CTRL1_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x20000U)    /* DDR SDRAM Controller-1 */
#define AMBA_CR52_DDR_CTRL2_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x2c000U)    /* DDR SDRAM Controller-2 */
#define AMBA_CR52_DDR_CTRL3_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x38000U)    /* DDR SDRAM Controller-3 */
#define AMBA_CR52_DDR_CTRL4_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x44000U)    /* DDR SDRAM Controller-4 */
#define AMBA_CR52_DDR_CTRL5_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x50000U)    /* DDR SDRAM Controller-5 */
#define AMBA_CR52_DDR_CTRL6_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x5c000U)    /* DDR SDRAM Controller-6 */
#define AMBA_CR52_DDR_CTRL7_BASE_ADDR               (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x68000U)    /* DDR SDRAM Controller-7 */
#define AMBA_CR52_ECCH_CTRL0_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0xf800U)     /* ECC Handler Controller-0 */
#define AMBA_CR52_ECCH_CTRL1_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x13800U)    /* ECC Handler Controller-1 */
#define AMBA_CR52_ECCH_CTRL2_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x1b800U)    /* ECC Handler Controller-2 */
#define AMBA_CR52_ECCH_CTRL3_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x1f800U)    /* ECC Handler Controller-3 */
#define AMBA_CR52_ECCH_CTRL4_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x27800U)    /* ECC Handler Controller-4 */
#define AMBA_CR52_ECCH_CTRL5_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x2b800U)    /* ECC Handler Controller-5 */
#define AMBA_CR52_ECCH_CTRL6_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x33800U)    /* ECC Handler Controller-6 */
#define AMBA_CR52_ECCH_CTRL7_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x37800U)    /* ECC Handler Controller-7 */
#define AMBA_CR52_ECCH_CTRL8_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x3f800U)    /* ECC Handler Controller-8 */
#define AMBA_CR52_ECCH_CTRL9_BASE_ADDR              (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x43800U)    /* ECC Handler Controller-9 */
#define AMBA_CR52_ECCH_CTRL10_BASE_ADDR             (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x4b800U)    /* ECC Handler Controller-10 */
#define AMBA_CR52_ECCH_CTRL11_BASE_ADDR             (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x4f800U)    /* ECC Handler Controller-11 */
#define AMBA_CR52_ECCH_CTRL12_BASE_ADDR             (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x57800U)    /* ECC Handler Controller-12 */
#define AMBA_CR52_ECCH_CTRL13_BASE_ADDR             (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x5b800U)    /* ECC Handler Controller-13 */
#define AMBA_CR52_ECCH_CTRL14_BASE_ADDR             (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x63800U)    /* ECC Handler Controller-14 */
#define AMBA_CR52_ECCH_CTRL15_BASE_ADDR             (AMBA_CORTEX_R52_DRAM_CFG_VIRT_BASE_ADDR + 0x67800U)    /* ECC Handler Controller-15 */

#define AMBA_CR52_GIC_DISTRIBUTOR_BASE_ADDR         (AMBA_CORTEX_R52_GIC_PHYS_BASE_ADDR + 0x0000U)          /* Internal GIC-400 Distributor */
#define AMBA_CR52_GIC_CPU_IF_BASE_ADDR              (AMBA_CORTEX_R52_GIC_PHYS_BASE_ADDR + 0x2000U)          /* Internal GIC-400 CPU Interfaces */
#define AMBA_CR52_GIC_REDISTRIBUTOR_BASE_ADDR       (AMBA_CORTEX_R52_GIC_PHYS_BASE_ADDR + 0x100000U)        /* Internal GIC-400 ReDistributor */
#define AMBA_CR52_GIC_REDIST_SPISGI_BASE_ADDR       (AMBA_CORTEX_R52_GIC_PHYS_BASE_ADDR + 0x110000U)        /* Internal GIC-400 ReDistributor Spi/Sgi Configure Block */

#define AMBA_CORTEX_R52_SCRATCHPAD0_BASE_ADDR       0xfff00000U                                             /* Cortex R52 Scratchpad0 */
#define AMBA_CORTEX_R52_SCRATCHPAD1_BASE_ADDR       0xfff01000U                                             /* Cortex R52 Scratchpad1 */
#define AMBA_CORTEX_R52_SCRATCHPAD2_BASE_ADDR       0xfff02000U                                             /* Cortex R52 Scratchpad2 */
#define AMBA_CORTEX_R52_WDT0_BASE_ADDR              0xfff03000U                                             /* Watchdog Timer for Cortex-CR52-0 */
#define AMBA_CORTEX_R52_WDT1_BASE_ADDR              0xfff04000U                                             /* Watchdog Timer for Cortex-CR52-1 */
#define AMBA_CORTEX_R52_WDT2_BASE_ADDR              0xfff05000U                                             /* Watchdog Timer for Cortex-CR52-2 */
#define AMBA_CORTEX_R52_CEHU_BASE_ADDR              0xfff06000U                                             /* CEHU */
#define AMBA_CORTEX_R52_GPIO_BASE_ADDR              0xfff08000U                                             /* GPIO Group */
#define AMBA_CORTEX_R52_I2C0_BASE_ADDR              0xfff0a000U                                             /* I2C Master 0 */
#define AMBA_CORTEX_R52_I2C1_BASE_ADDR              0xfff0b000U                                             /* I2C Master 1 */
#define AMBA_CORTEX_R52_SPI0_BASE_ADDR              0xfff0c000U                                             /* SPI Master 0 */
#define AMBA_CORTEX_R52_SPI1_BASE_ADDR              0xfff0d000U                                             /* SPI Master 1 */
#define AMBA_CORTEX_R52_DMA0_BASE_ADDR              0xfff0e000U                                             /* DMA Engine 0 */
#define AMBA_CORTEX_R52_CAN0_BASE_ADDR              0xfff0f000U                                             /* CAN Bus 0 */
#define AMBA_CORTEX_R52_CAN1_BASE_ADDR              0xfff10000U                                             /* CAN Bus 1 */
#define AMBA_CORTEX_R52_ENET_BASE_ADDR              0xfff11000U                                             /* Ethernet */
#define AMBA_CORTEX_R52_MDMA_BASE_ADDR              0xfff13000U                                             /* MDMA */
#define AMBA_CORTEX_R52_TMR0_BASE_ADDR              0xfff14000U                                             /* Interval Timer 0 */
#define AMBA_CORTEX_R52_TMR1_BASE_ADDR              0xfff15000U                                             /* Interval Timer 1 */
#define AMBA_CORTEX_R52_TMR2_BASE_ADDR              0xfff16000U                                             /* Interval Timer 2 */
#define AMBA_CORTEX_R52_SI_CONFIG_BASE_ADDR         0xfff17000U                                             /* Safety Island config */
#define AMBA_CORTEX_R52_ECRU_BASE_ADDR              0xfff18000U                                             /* ECRU */

#define AMBA_CA78_WDT_HSM_BASE_ADDR                 (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR)                     /* Watchdog Timer */
#define AMBA_CA78_WDT0_BASE_ADDR                    (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x1000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT1_BASE_ADDR                    (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x2000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT2_BASE_ADDR                    (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x3000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT3_BASE_ADDR                    (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x4000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT4_BASE_ADDR                    (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x5000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT5_BASE_ADDR                    (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x6000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT_MORC_BASE_ADDR                (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x7000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT_CM3_0_BASE_ADDR               (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x8000U)           /* Watchdog Timer */
#define AMBA_CA78_WDT_CM3_1_BASE_ADDR               (AMBA_CORTEX_R52_APB_WDT_BASE_ADDR + 0x9000U)           /* Watchdog Timer */

#define AMBA_CA78_CPUID_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x00000U)
#define AMBA_CA78_NOR_SPI_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x01000U)
#define AMBA_CA78_FLASH_CPU_BASE_ADDR               (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x02000U)         /* Flash Controller */
#define AMBA_CA78_FLASH_0_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x03000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_1_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x04000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_2_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x05000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_3_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x06000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_4_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x07000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_5_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x08000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_6_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x09000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_FLASH_7_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x0a000U)         /* NAND Flash Controller FIFO */
#define AMBA_CA78_GDMA_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x0b000U)         /* Graphics DMA */
#define AMBA_CA78_USB_BASE_ADDR                     (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x0c000U)         /* USB Device */
#define AMBA_CA78_ETH0_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x0e000U)         /* Ethernet 1 (MAC) */
#define AMBA_CA78_SPI_SLAVE_BASE_ADDR               (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x10000U)         /* SPI Slave */
#define AMBA_CA78_SPI0_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x11000U)         /* SPI Master 0 */
#define AMBA_CA78_SPI1_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x12000U)         /* SPI Master 1 */
#define AMBA_CA78_SPI2_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x13000U)         /* SPI Master 2 */
#define AMBA_CA78_SPI3_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x14000U)         /* SPI Master 3 */
#define AMBA_CA78_DMIC_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x15000U)         /* DMIC */
#define AMBA_CA78_HDMI_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x16000U)         /* HDMI */
#define AMBA_CA78_UART0_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x17000U)         /* UART AHB 0 */
#define AMBA_CA78_UART1_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x18000U)         /* UART AHB 1 */
#define AMBA_CA78_UART2_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x19000U)         /* UART AHB 2 */
#define AMBA_CA78_UART3_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x1a000U)         /* UART AHB 3 */
#define AMBA_CA78_GDMA1_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x1b000U)         /* Graphics DMA */
#define AMBA_CA78_DAI0_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x1c000U)         /*  */
#define AMBA_CA78_DAI1_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x1d000U)         /*  */
#define AMBA_CA78_GDMA2_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x1e000U)         /* Graphics DMA */
#define AMBA_CA78_GDMA3_BASE_ADDR                   (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x1f000U)         /* Graphics DMA */
#define AMBA_CA78_DMA0_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x20000U)         /* DMA Engine 0 */
#define AMBA_CA78_DMA1_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x21000U)         /* DMA Engine 1 */
#define AMBA_CA78_ETH1_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x22000U)         /* Ethernet 1 (MAC) */
#define AMBA_CA78_SPI4_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x25000U)         /* SPI Master 4 */
#define AMBA_CA78_SPI5_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x26000U)         /* SPI Master 5 */
#define AMBA_CA78_CAN0_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x27000U)         /* CAN Bus 0 */
#define AMBA_CA78_CAN1_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x28000U)         /* CAN Bus 1 */
#define AMBA_CA78_DMA2_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x29000U)         /* DMA Engine 1 */
#define AMBA_CA78_ETH2_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x2a000U)         /* Ethernet 2 (MAC) */
#define AMBA_CA78_SPI6_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x2c000U)         /* SPI Master 4 */
#define AMBA_CA78_SPI7_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x2d000U)         /* SPI Master 5 */
#define AMBA_CA78_SCRATCHPAD_S_BASE_ADDR            (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x2e000U)         /* Scratchpad (Secure Part) */
#define AMBA_CA78_S_SRAM_BASE_ADDR                  (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x30000U)         /* Scratchpad SRAM (Secure Part), 32KB */
#define AMBA_CA78_SCRATCHPAD_NS_BASE_ADDR           (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x3F000U)         /* Scratchpad (Non-Secure Part) */
#define AMBA_CA78_NS_SRAM_BASE_ADDR                 (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x40000U)         /* Scratchpad SRAM (Non-Secure Part), 64KB */
#define AMBA_CA78_CAN2_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x50000U)         /* CAN Bus 2 */
#define AMBA_CA78_CAN3_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x51000U)         /* CAN Bus 3 */
#define AMBA_CA78_CAN4_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x52000U)         /* CAN Bus 4 */
#define AMBA_CA78_CAN5_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x53000U)         /* CAN Bus 5 */
#define AMBA_CA78_ECRU_BASE_ADDR                    (AMBA_CORTEX_R52_AHB_VIRT_BASE_ADDR + 0x56000U)         /* ECRU */

#define AMBA_CA78_UART_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR)                    /* UART APB */
#define AMBA_CA78_IRIF_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x01000U)         /* IRIF */
#define AMBA_CA78_PWM0_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x03000U)         /* PWM 0 */
#define AMBA_CA78_TMR0_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x04000U)         /* Interval Timer 0 */
#define AMBA_CA78_TMR1_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x05000U)         /* Interval Timer 1 */
#define AMBA_CA78_TMR2_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x06000U)         /* Interval Timer 2 */
#define AMBA_CA78_I2C_SLAVE_BASE_ADDR               (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x07000U)         /* I2C Slave */
#define AMBA_CA78_I2C0_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x08000U)         /* I2C Master 0 */
#define AMBA_CA78_I2C1_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x09000U)         /* I2C Master 1 */
#define AMBA_CA78_I2C2_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x0a000U)         /* I2C Master 2 */
#define AMBA_CA78_I2C3_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x0b000U)         /* I2C Master 3 */
#define AMBA_CA78_PWM1_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x0c000U)         /* PWM 1 */
#define AMBA_CA78_PWM2_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x0d000U)         /* PWM 2 */
#define AMBA_CA78_I2C4_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x0e000U)         /* I2C Master 4 */
#define AMBA_CA78_I2C5_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x0f000U)         /* I2C Master 5 */
#define AMBA_CA78_IOMUX_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x10000U)         /* IOMUX */
#define AMBA_CA78_GPIO0_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x13000U)         /* GPIO Group 0 */
#define AMBA_CA78_GPIO1_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x14000U)         /* GPIO Group 1 */
#define AMBA_CA78_GPIO2_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x15000U)         /* GPIO Group 2 */
#define AMBA_CA78_GPIO3_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x16000U)         /* GPIO Group 3 */
#define AMBA_CA78_GPIO4_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x17000U)         /* GPIO Group 0 */
#define AMBA_CA78_GPIO5_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x18000U)         /* GPIO Group 1 */
#define AMBA_CA78_GPIO6_BASE_ADDR                   (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x19000U)         /* GPIO Group 2 */
#define AMBA_CA78_I2C6_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x1c000U)         /* I2C Master 0 */
#define AMBA_CA78_I2C7_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x1d000U)         /* I2C Master 1 */
#define AMBA_CA78_I2C8_BASE_ADDR                    (AMBA_CORTEX_R52_APB_VIRT_BASE_ADDR + 0x1e000U)         /* I2C Master 2 */

#endif /* AMBA_CORTEX_R52_H */
