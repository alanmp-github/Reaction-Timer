// No built-in library file should be in this code
#include "myLib.h"
#include "lcd.h"

#define lcd_delay SysCtlDelay(67000 * 2) // changed

#define lcd_put_data(x) GPIOPinWrite(GPIO_PORTB_BASE, (0xFF), x)

#define lcd_set_RS() (GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_4), 0x01 << 4))
#define lcd_set_RW() (GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_5), 0x01 << 5))
#define lcd_set_EN() (GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_6), 0x01 << 6))
#define lcd_reset_RS() (GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_4), ~(0x01 << 4)))
#define lcd_reset_RW() (GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_5), ~(0x01 << 5)))
#define lcd_reset_EN() (GPIOPinWrite(GPIO_PORTC_BASE, (GPIO_PIN_6), ~(0x01 << 6)))

/*
 * lcd_portconfig();
 * Description: Sets LCD port pin as output
 * Datapins: PORTB pin 0 to pin8 (LCD is interfaced in 8 bit mode)
 * ControlPins: PORTC pin4,pin5 and pin6 is connected to RS, RW and EN respectively.
 * These connection is for TIVA booster board designed by WEL lab. Change Port connection for accroding to interface
 */

void lcd_portconfig(void)
{
	// Set CPU Clock to 40MHz. 400MHz PLL/2 = 200 DIV 5 = 40MHz
	//SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

	//* * * * * * Setting Ouput for LCD * * * * * * * *
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);

	// * * * * * * LCD_Control Pin* * * * * * *
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, 0x70);
}



/*
 * lcd_command()
 * Description: Sends command on databus
 * usage: lcd_command(0x01) - clears the display
 */
void lcd_command(char cmd)
{
    lcd_put_data(cmd);
    lcd_reset_RS();
    lcd_delay;
    lcd_set_EN();
    lcd_delay;
    lcd_reset_EN();
    lcd_delay;
}


/*
 * lcd_init();
 * Description: Set ports pins as outputs. Initialize LCD in 8-bit mode.
 *
 * This should be the first function that must be called in main when using LCD
 */
void lcd_init()
{
	lcd_portconfig();

	lcd_reset_RW();
	lcd_reset_EN();

	lcd_command(0x38); // function set command
	lcd_command(0x0f); // display switch command
	lcd_command(0x06); // input set command
	lcd_command(0x01); // clear screen command
	lcd_command(0x80); // Set cursor to second line starting
}



/*
 *lcd_char()
 *Description: Print single character
 *Example lcd_char('A'); prints letter A
 */
void lcd_char(char data)
{
	/* TASK 3 : Write the code here
	   Hint: Somehow Similar to the function lcd_command()*/
	lcd_put_data(data);
	lcd_set_RS();
	lcd_delay;
	lcd_set_EN();
	lcd_delay;
	lcd_reset_EN();
	lcd_delay;

	lcd_command(0x06);
}

/*
 * lcd_string()
 * Description: Print string
 * Example: lcd_string("Hello World");
 */
void lcd_string(char *g)
{
	while (*g)
		lcd_char(*g++);
}

/*
 * Name: lcd_cursor (row, column). For setting cursor position in 16 by 2 lcd
 * Description: Position the LCD cursor at "row", "column"
 * row: 1,2
 * column: 1 to 16
 * Example: lcd_cursor(2,14) - Places cursor at 2nd line 14th column
 */

void lcd_cursor(char row, char column)
{
	char i, iter = column & 0x0F;
	if (row == '1')
		lcd_command(0x80);
	else if (row == '2')
		lcd_command(0xC0);

	for (i = 0x0; i+1 <iter;i++)
	{
		lcd_char(0x14);
	}

	/* TASK 4 : Write the code to set the cursor position*/
}

/*
 * Name: lcd_print (value, digit). Print value (a numeric number).
 * Description: Print number
 * value: Numeric number
 * digit: number of digits in number. Maximum allowed digit is 7
 * Example: lcd_print(456,3) - Print 456 on LCD
 */

void lcd_print(char row, char column, uint32_t value, int digits)
{
    /* row    : Input Cursor position for the row position
       column : Input Cursor position for the column position
       value  : Input integer value to be displayed on LCD
       digits : Input number to depict the number of decimal places u want to show*/

    // To set the LCD cursor position
    lcd_cursor(row, column);
    //int length = 0;
    //int min;
    uint32_t temp_num = value;
    int i;
    char key[8];
    for(i=0;i<8;i++)
    {
        if(temp_num > 0)
        {
            key[7-i] = 0x30 + (char)(temp_num % 10);
            temp_num = temp_num / 10;
        }
        else
            key[7-i]='0';

    }

    for(i=0;i<digits;i++)
        lcd_char(key[8-digits+i]);

    /* TASK 5 : Write the code to convert the integer number into ASCII format so
       that can passed to the LCD for display */
    /* You can declare your own variables also*/
}
/*
void main(void)
{
    lcd_init();
    lcd_string("ALA-VAT-VIS");
    lcd_cursor('2', '1');
    lcd_string("ESD IIT Bombay");
    while(1);
}*/

