#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

/**
 * main.c
 */
uint8_t input=0x00;
#define RED_LED 1
#define BLUE_LED 2
#define GREEN_LED 3


int main(void)
{
    //Deciding to Toggle on port PF0
    SYSCTL_RCGC2_R |= 0x00000020;;      /* enable clock to GPIOF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x01;             /* make PORTF0 configurable */
    GPIO_PORTF_DIR_R = 0x0E;            /* set PORTF: 1,2,3 as output, 0,4 as input */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 0-4 as digital pins */
    GPIO_PORTF_PUR_R = 0x11;            /* enable pull-up for pins 0 and 4 */
    while(1)
    {
        //single & is bit-wise operation
        //using SW2(PF0) as input switch; Note: it is active_low
        input = GPIO_PORTF_DATA_R & 0x01;
        if(input==0x00)
        {
            GPIO_PORTF_DATA_R |= (1<<RED_LED);//Turn ON Red LED
        }
        else
        {
            GPIO_PORTF_DATA_R &= (0<<RED_LED);//Turn OFF Red LED
        }
    }
}


