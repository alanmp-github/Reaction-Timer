#include "myLib.h"
#include "lcd.h"

volatile uint8_t flag=1;
uint32_t status,pre_time,post_time,avg_time,e_time,avg_time;
//uint32_t ADC_buf_rbk;
void hardwareInit(void);
void s2ISR(void);

void main(void)
{

    hardwareInit();

    while (1)
    {
        uint8_t i;
        avg_time=0;

        for(i=0;i<5;i++)
        {
            lcd_command(0x01); //Clear LCD
            lcd_string("Press S1"); //Print string
            while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)); //Wait for SW1 press


            /*--SW1 is pressed--*/
            TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF); //Load timer
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,(0x01<<3)); //ON Green LED
            lcd_command(0x01); //Clear LCD
            lcd_string("Press S2 @ Red"); //Print String
            SysCtlDelay(37000000); //Wait for some time
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3,(0x00<<3)); //OFF Green LED
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,(0x01<<1)); //ON RED LED

            GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
            GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);

            pre_time=TimerValueGet(TIMER0_BASE, TIMER_A);   //Read current timer value
            TimerEnable(TIMER0_BASE, TIMER_A); ////Enable timer //Run timer
            while(flag); //Wait for SW2 Press

            /*--SW2 is pressed--*/
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,(0x00<<1)); //OFF RED LED
            TimerDisable(TIMER0_BASE, TIMER_A); //Disable timer
            e_time=(pre_time-post_time)/100/200; //Calculate elapsed time
            avg_time=avg_time+e_time;

            /*Display Elapsed Time*/
            lcd_command(0x01); //Clear LCD
            lcd_char('*');
            lcd_print('1','2',i+1,1);
            lcd_print('2','1',e_time,7);
            SysCtlDelay(37000000);

            flag=1; //Setting flag again

        }
        lcd_command(0x01); //Clear LCD
        lcd_string("S2 to continue.."); //Print string
        while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)); //Wait for SW2 press
        lcd_command(0x01); //Clear LCD
        lcd_string("Average time(ms)"); //Print string
        lcd_print('2','1',avg_time/5,7);


        while(1);
    }
}

void hardwareInit(void)
{
    // Set CPU Clock to 20MHz. 400MHz PLL/2 = 200 DIV 10 = 20MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);


    // ADC Configuration
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Enable Port F
    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0); // Set PF0&4 as input pin (Switches)
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_1); // Set PF1&3 as output pin (LEDs)

    lcd_init();

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerClockSourceSet(TIMER0_BASE,TIMER_CLOCK_SYSTEM);
    TimerPrescaleSet(TIMER0_BASE, TIMER_A , 200-1 ); //Timer Frequency: 100 kHz

    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    GPIOIntRegister(GPIO_PORTF_BASE, s2ISR);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE);
    IntPrioritySet(INT_GPIOF, 0);
    //GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);
    IntEnable(INT_GPIOF);
    IntMasterEnable();
    //period=SysCtlClockGet();//SysClock=20 MHz
}

void s2ISR(void)
{
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0)
    {
        GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
        GPIOIntDisable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);

        post_time=TimerValueGet(TIMER0_BASE, TIMER_A);
        flag=0;
        status=GPIOIntStatus(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    }

}
