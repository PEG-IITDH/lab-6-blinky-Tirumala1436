/* SysTick memory-mapped registers */
#define STCTRL *((volatile long *) 0xE000E010)    // control and status
#define STRELOAD *((volatile long *) 0xE000E014)    // reload value
#define STCURRENT *((volatile long *) 0xE000E018)    // current value

#define COUNT_FLAG  (1 << 16)   // bit 16 of CSR automatically set to 1
                                //   when timer expires
#define ENABLE      (1 << 0)    // bit 0 of CSR to enable the timer
#define CLKINT      (1 << 2)    // bit 2 of CSR to specify CPU clock

#define CLOCK_MHZ 16

void Delay(int ms)
{
    STCURRENT=0;            //to flush the current value
    STRELOAD = ms;                       // reload value for 'ms' milliseconds
    STCTRL |= (CLKINT | ENABLE);        // set internal clock, enable the timer
    //int count_flag_value= STCTRL & 0x00010000;
    //while ( count_flag_value== 0)
    while ((STCTRL & COUNT_FLAG) == 0)  // wait until flag is set
    {
        ;   // do nothing
    }

    STCTRL = 0;                // stop the timer

    return;
}

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

int main(void)
{
        //Deciding to Toggle on port PB0
        SYSCTL_RCGC2_R |= 0x00000002;;      /* enable clock to GPIOB */
        GPIO_PORTB_LOCK_R = 0x4C4F434B;     /* unlock commit register */
        GPIO_PORTB_CR_R = 0x01;             /* make PORTB0 configurable */
        GPIO_PORTB_DIR_R = 0x01;            /* set PORTB0 pin as output (LED) pin */
        GPIO_PORTB_DEN_R = 0x01;            /* set PORTB pins 0 as digital pins */
        while(1)
        {
            GPIO_PORTB_DATA_R = 0x01;
            Delay(3150);
            GPIO_PORTB_DATA_R = 0x00;
            Delay(12750);
        }
}
