# Embedded Lab UART Communication
## Objectives
Program your microontroller to transmit:<br>
"F0" if SW1 is pressed<br>
"AA" if SW2 is pressed <br>
Over UART with baud rate 9600 and odd parity. <br>
Your program should also listen for incoming data on the UART with the same baud and parity config; if "AA" is received LED should be GREEN; if "F0" is recieved, the LED should be BLUE and if any error is detected LED should be RED. Test this by communicating with your neighboring group.<br>
   
## Theory
### What is UART ?
UART, which stands for Universal Asynchronous Receiver/Transmitter, is a communication protocol widely used for serial communication between devices. It enables the transfer of data between devices using two wires: one for transmitting (TX) and the other for receiving (RX). UART is asynchronous, meaning that data is transmitted without a shared clock signal, and devices synchronize based on agreed-upon parameters such as baud rate, data bits, parity, and stop bits.

Key points about UART:

#### Asynchronous Communication: 
UART does not rely on a shared clock signal between devices. Instead, both the transmitting and receiving devices agree on communication parameters.

#### Basic Components:<br>
UART communication involves two main components—a transmitter and a receiver. The transmitter converts parallel data into serial form for transmission, and the receiver converts the received serial data back into parallel form.

#### Baud Rate: 
Baud rate defines the speed at which data is transmitted and received. Both communicating devices must operate at the same baud rate for successful communication.

#### Data Framing: 
Each piece of data is framed by start and stop bits, indicating the beginning and end of a data byte. This framing helps the receiver identify and extract the transmitted data.

#### Configurable Parameters: 
UART communication parameters include data bits (usually 8 bits per byte), parity (optional error-checking mechanism), and stop bits (typically 1 or 2 bits).

#### Half-Duplex or Full-Duplex: 
UART supports both half-duplex and full-duplex communication. In half-duplex mode, devices take turns transmitting and receiving, while in full-duplex mode, they can simultaneously transmit and receive.

#### Widespread Usage: 
UART is a fundamental communication protocol and is commonly used in various applications, including serial communication between microcontrollers, sensors, and peripheral devices.

### Calculation
 We calculate Baudrate Register parameters using 9600.
 We get integer and fractional part for baud rate speciification to register by  given expression<br>
   > 16000000/(16*9600)=104.1666 <br>
 

Integer part taken as 104 and we calculated fractional part using below expression
  > 0.167*64+0.5=11
## Code
```C
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

#define Sw_Bits 0x11
#define Red 0X02
#define Blue 0X04
#define Green 0X08
#define S1_data 0xF0
#define S2_data 0xAA

void PortF_Config(void);
void UART_Config(void);
void PortF_Handler(void);
void UART_Handler(void);
void Systick_Handler(void);


#define INT_EN (1<<1)       //bit 1 of CSR to generate interrupt to the NVIC when SysTick counts to 0
#define Clk_SRC (1<<2)      //bit 2 of CSR to select system clock
#define STCTRL *((volatile long *) 0xE000E010)    // control and status
#define STRELOAD *((volatile long *) 0xE000E014)    // reload value
#define STCURRENT *((volatile long *) 0xE000E018)    // current value

#define COUNT_FLAG  (1 << 16)   // bit 16 of CSR automatically set to 1
                                //   when timer expires
#define ENABLE      (1 << 0)    // bit 0 of CSR to enable the timer
#define CLKINT      (1 << 2)    // bit 2 of CSR to specify CPU clock


void main(void)
{
    SYSCTL_RCGCUART_R |= (1<<7);
    SYSCTL_RCGCGPIO_R |= (1<<5);
    SYSCTL_RCGCGPIO_R |= (1<<4);
    UART_Config();
    PortF_Config();

        while(1){}
}


void PortF_Config(void)
{
    GPIO_PORTF_LOCK_R = 0x4C4F434B;     //Unlock PortF register
    GPIO_PORTF_CR_R = 0x1F;             //Enable Commit function

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
    NVIC_PRI7_R &= 0xFF3FFFFF;
}
void UART_Config(void)
{

    GPIO_PORTE_AFSEL_R |= (1<<1)|(1<<0);
    GPIO_PORTE_PCTL_R |= 0x11;
    GPIO_PORTE_LOCK_R = 0x4C4F434B;     /* unlock commit register */
    GPIO_PORTE_CR_R = 0XFF;             /* make PORTF0 configurable */
    GPIO_PORTE_DIR_R = 0X20;            /* set PORTF: 1,2,3 as output, 0,4 as input */
    GPIO_PORTE_DEN_R = 0XFF;          /* set PORTF pins 0-4 as digital pins */
    GPIO_PORTE_PUR_R = 0x11;
    UART7_CTL_R |= 0x0;
    UART7_IBRD_R = 0x68;
    UART7_FBRD_R = 0xB;
    UART7_LCRH_R |= 0x62;
    UART7_CC_R |= 0X0;
    UART7_CTL_R |= (1<<9) | (1<<8) | (1<<0);
    UART7_IM_R &= ((0<<4)|(0<<5)|(0<<8));       //Mask Tx, Rx and Parity interrupts
    UART7_ICR_R &= ((0<<4)|(0<<5)|(0<<8));      //Clear Tx, Rx and Parity interrupts
    UART7_IM_R |= (1<<4);                       //Enable Rx interrupt
    NVIC_EN1_R |= (1<<31);                      //Interrupts enabled for UART7
    NVIC_PRI7_R &= 0xFF3FFFFF;

}
void PortF_Handler()
{
    GPIO_PORTF_IM_R &= ~Sw_Bits;

    if(GPIO_PORTF_RIS_R & 0x10)         //Usr Sw 1
    {
        UART7_DR_R = 0xF0;
    }
    else if (GPIO_PORTF_RIS_R & 0x01)   //Usr Sw 2
    {
        UART7_DR_R = 0xAA;
    }

}

void UART_Handler(void)
{
    UART7_IM_R &= (0<<4);       //Mask UART Rx interrupt

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
        else if(UART7_RSR_R & 0x0000000F)    //Any error detected
        {
            GPIO_PORTF_DATA_R = Red;
        }
    }



    UART7_ECR_R &= 0xFFFFFFF0;        //Clear UART errors

    STCURRENT=0x00;                         //Reinitialise Systick Counter to Zero
    STRELOAD = 16*1000000/2;                //Run Systick for 0.5 second
    STCTRL |= (ENABLE | INT_EN | Clk_SRC);  //Enable Systick, Enable Interrupt Generation, Enable system clock

    GPIO_PORTF_ICR_R = Sw_Bits;

}

void Systick_Handler(void)
{
    GPIO_PORTF_DATA_R &= 0x00;               //Turn off LED

    GPIO_PORTF_ICR_R = Sw_Bits;
    GPIO_PORTF_IM_R |= Sw_Bits;
    UART7_IM_R |= (1<<4);                   //unMask UART Rx interrupt
}

```

## Observation
We got these observations


![AA](LAB8AA.png)


![F0](LAB8F0.png)

## References
tm4c123gh6pm data sheet.






