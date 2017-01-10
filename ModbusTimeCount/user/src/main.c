#include <stdio.h>
#include "M051Series.h"
#include "realyctrl.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "Display.h"

#define PLLCON_SETTING    CLK_PLLCON_50MHz_HIRC /* CLK_PLLCON_50MHz_HXT*/
#define PLL_CLOCK           50000000



void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Enable external 12 MHz XTAL */
  //  CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;
	CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;
    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
   // CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk);
	CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);
   CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_IRC22M_STB_Msk);

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_TMR1_EN_Msk;

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL | CLK_CLKSEL1_TMR1_S_HCLK;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set P3 multi-function pins for UART0 RXD, TXD and T1 */
    SYS->P3_MFP = SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0 /*| SYS_MFP_P35_T1*/;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}
 void DrvSYS_Delay(uint32_t us);
/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/

extern void DisplayScan(void);
extern BOOL xMBPortTimersInit(USHORT usTim1Timerout50us);
uint8_t SlaveAddr;
extern int32_t TimeCount;
int main(void )
{
    static  uint32_t u32InitCount;
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();


	/*Init hardwave realies*/
//	HWRealyInit();
	/*Get the Addr from the HW switch */
 //  SlaveAddr = GetHwAddrIint();
   /*Init RotaryEncoder */
 RotaryEncoderHWinit();
	/*Init Display */	
 DisplayHWInit();
 /*soft the code of time init ..	 */
 TimeInit();
    /* Lock protected registers */
 //SYS_LockReg();
    /*Init Modbus RTU main thread and config USART */
 eMBInit( MB_RTU, SlaveAddr,0,9600, MB_PAR_NONE );
 /* Enable the Modbus Protocol Stack. */
 eMBEnable();
// SetNumber(0,0,0,0);
  while(1)
  {

  	  DisplayScan();
	  CountPoll();
	  BuzzePoll();	
 	(void)eMBPoll();
  }

   
}

void DrvSYS_Delay(uint32_t us)
{
    SysTick->LOAD = us * CyclesPerUs;
    SysTick->VAL  = (0x00);
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

    /* Waiting for down-count to zero */
    while((SysTick->CTRL & (1 << 16)) == 0);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/



