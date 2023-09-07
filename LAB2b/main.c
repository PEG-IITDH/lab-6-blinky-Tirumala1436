#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

/**
 * main.c
 */
uint8_t input=0x00;
uint8_t input_prev=0x01;
int count=0;
#define RED_LED 1
#define BLUE_LED 2
#define GREEN_LED 3


int i,j;
void delay(int stalls)
{
    for(i=0;i<stalls;i++)
    {
        for(j=0;j<stalls;j++);
    }
}

int main(void)
{
    SYSCTL_RCGC2_R |= 0x00000020;;      /* enable clock to GPIOF */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTF_CR_R = 0x01;             /* make PORTF0 configurable */
    GPIO_PORTF_DIR_R = 0x0E;            /* set PORTF: 1,2,3 as output, 0,4 as input */
    GPIO_PORTF_DEN_R = 0x1F;            /* set PORTF pins 0-4 as digital pins */
    GPIO_PORTF_PUR_R = 0x11;            /* enable pull-up for pins 0 and 4 */
    GPIO_PORTF_DATA_R = 0x2;     //default state= Red LED ON
    while(1)
    {
        input = GPIO_PORTF_DATA_R & 0x01;
        if(input==0x00 && input_prev==0x01)
        {
            delay(5);                           //debouncing implementation
            if((GPIO_PORTF_DATA_R & 0x01)==0x00)
            {
            count++;
            if(count==1)
            {
                GPIO_PORTF_DATA_R = (0x8);      //Green LED ON
            }
            else if(count==2)
            {
                GPIO_PORTF_DATA_R = (0x4);      //Blue LED ON
            }
            else if(count==3)
            {
                count=0;
                GPIO_PORTF_DATA_R = (0x2);      //Red LED ON
            }
            }
        }
        input_prev=input;
    }
}
