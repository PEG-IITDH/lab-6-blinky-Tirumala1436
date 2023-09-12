#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define RED_LED 1

/* SysTick memory-mapped registers */
#define STCTRL *((volatile long *) 0xE000E010)    // control and status
#define STRELOAD *((volatile long *) 0xE000E014)    // reload value
#define STCURRENT *((volatile long *) 0xE000E018)    // current value

#define COUNT_FLAG  (1 << 16)   // bit 16 of CSR automatically set to 1
                                //   when timer expires
#define ENABLE      (1 << 0)    // bit 0 of CSR to enable the timer
#define CLKINT      (1 << 2)    // bit 2 of CSR to specify CPU clock
#define INTEN       (1 << 1)    //bit 1 of CSR to enable Interrupt bit

#define CLOCK_MHZ 16

/*Defining Interrupt Registers*/
#define MASK_BITS 0x11 // pins for which interrupts should be enabled: Pin0 and Pin4

void PortFHandler()//GPIO Interrupt ISR
{
    GPIO_PORTF_ICR_R |= MASK_BITS; // clear the GPIO interrupt
    GPIO_PORTF_DATA_R |= (1<<RED_LED);//Turn ON Red LED
    //Configuring systick:
         STRELOAD=1000*16000;//1000ms delay
         STCURRENT=0;
         STCTRL |= (CLKINT | ENABLE |INTEN);//start systick
}

void SysTickHandler()//systick ISR
{
    GPIO_PORTF_DATA_R &= ~(1<<RED_LED);//Turn OFF Red LED
    //GPIO_PORTF_DATA_R ^= 0x02;//Toggle Red LED
    STCTRL=0x00;//to stop timer and clear Interrupt bit
}


int main(void)
{
        //Deciding to Toggle on PORTF
        SYSCTL_RCGC2_R |= 0x00000020;;      /* enable clock to GPIOF */
        GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
        GPIO_PORTF_CR_R = 0xFF;             /* make PORTF configurable */
        GPIO_PORTF_DIR_R = 0xEE;            /* set PORTF pins 0,4 as input, remaining output */
        GPIO_PORTF_DEN_R = 0xFF;            /* set PORTF pins as digital pins */
        GPIO_PORTF_PUR_R |= 0x11;            /* enable pull-up for pins 0 and 4 */

        GPIO_PORTF_IM_R &= ~MASK_BITS; // mask interrupt by clearing bits
        GPIO_PORTF_IS_R &= ~MASK_BITS; // edge sensitive interrupts
        GPIO_PORTF_IBE_R &= ~MASK_BITS; // interrupt NOT on both edges
        GPIO_PORTF_IEV_R &= ~MASK_BITS; // falling edge trigger

        /* Prioritize and enable interrupts in NVIC */
        NVIC_PRI7_R = (NVIC_PRI7_R & 0xF1FFFFFF) | (3 << 21);
         // interrupt priority register 7
         // bits 21-23 for interrupt 30 (port F)

        NVIC_EN0_R  |= 1 << 30; // enable interrupt 30 (port F)

        /* Enable interrupt generation in GPIO */
        GPIO_PORTF_ICR_R |= MASK_BITS; // clear any prior interrupt
        GPIO_PORTF_IM_R |= MASK_BITS; // unmask interrupt by setting bits

        //GPIO_PORTF_DATA_R |= (1<<RED_LED);

        while(1);//Program shouldn't end
}


