#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define Sw_Bits 0x11
#define Red 0x02
#define Blue 0x04
#define Green 0x08

void PortF_Config(void);
void UART_Config(void);
void PortF_Handler(void);
void UART_Handler(void);


int main(void)
{

    UART_Config();
    PortF_Config();

    while(1){}
}


void PortF_Config(void)
{
    SYSCTL_RCGC2_R |= 0x00000020;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;     //Unlock PortF register
    GPIO_PORTF_CR_R = 0x01;             //Enable Commit function

    GPIO_PORTF_PUR_R = 0x11;            //Pull-up for user switches
    GPIO_PORTF_DEN_R = 0x1F;            //Enable all pins on port F
    GPIO_PORTF_DIR_R = 0x0E;            //Define PortF LEDs as output and switches as input

    GPIO_PORTF_IS_R &= ~Sw_Bits;
    GPIO_PORTF_IBE_R &= ~Sw_Bits;
    GPIO_PORTF_IEV_R &= ~Sw_Bits;
    GPIO_PORTF_IM_R &= ~Sw_Bits;
    GPIO_PORTF_ICR_R |= Sw_Bits;
    GPIO_PORTF_IM_R |= Sw_Bits;

    NVIC_EN0_R |= (1<<30);
}

void UART_Config(void)
{
    SYSCTL_RCGCUART_R |= 0x80;          //UART module 7  is enabled
    SYSCTL_RCGC2_R |= 0x10;             //Port E receives clock

    GPIO_PORTE_AFSEL_R |= 0x03;
    GPIO_PORTE_PCTL_R |= 0x11;
    GPIO_PORTE_DIR_R |= 0x02;
    GPIO_PORTE_DEN_R = 0x03;

    UART7_CTL_R &= 0xFFFFFFFE;
    UART7_IBRD_R = 104;
    UART7_FBRD_R = 11;
    UART7_LCRH_R |= 0x62;
    UART7_CC_R &= 0xFFFFFFF0;
    UART7_CTL_R |= (1<<0) | (1<<8) | (1<<9);

    UART7_IM_R &= 0x00;                          //Mask Tx, Rx and Parity interrupts
    UART7_ICR_R &= 0x00;                        //Clear Tx, Rx and Parity interrupts
    UART7_IM_R |= (1<<4);                       //Enable Rx interrupt
    NVIC_EN1_R |= (1<<31);                      //Interrupts enabled for UART7

}
void PortF_Handler()
{
    GPIO_PORTF_IM_R &= ~Sw_Bits;
    int i;

    if(GPIO_PORTF_RIS_R & 0x10)         //Usr Sw 1
    {
        for(i=0;i<16000;i++){}
        if(~(GPIO_PORTF_DATA_R)&0x10)
        {
            UART7_DR_R = 0xF0;
        }
    }
    else if (GPIO_PORTF_RIS_R & 0x01)   //Usr Sw 2
    {
        for(i=0;i<16000;i++){}
        if(~(GPIO_PORTF_DATA_R)&0x01)
        {
            UART7_DR_R = 0xAA;
        }
     }
    GPIO_PORTF_ICR_R = Sw_Bits;
    GPIO_PORTF_IM_R |= Sw_Bits;
}

void UART_Handler(void)
{
    UART7_IM_R &= 0x00;       //Mask UART Rx interrupt

    if(UART7_FR_R & (1<<6))    //Rx flag register set
    {
        if(UART7_DR_R == 0xAA)
        {
            GPIO_PORTF_DATA_R = Green;
        }
        else if(UART7_DR_R == 0xF0)
        {
            GPIO_PORTF_DATA_R = Blue;
        }
        else if(UART7_RSR_R & 0x0F)    //Any error detected
        {
            GPIO_PORTF_DATA_R = Red;
        }
    }

    int i;
    for(i=0;i<800000;i++){}
    GPIO_PORTF_DATA_R &= 0x00;              //Turn off LED
    UART7_ECR_R &= ~(0xF);
    UART7_ICR_R &= 0x00;
    UART7_IM_R |= (1<<4);                   //unMask UART Rx interrupt
}
