#define TARGET_IS_TM4C123_RA1		// Required for Direct ROM Calls
#define DEBUG_LEVEL
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

/* UART includes */
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

/* I2C includes  */
#include "driverlib/i2c.h"

#include "mpu9150.h"

#define LED_RED 	GPIO_PIN_1
#define LED_BLUE 	GPIO_PIN_2
#define LED_GREEN 	GPIO_PIN_3

uint8_t ConfigureUART(void);
uint8_t ConfigureSystem(void);
uint8_t ConfigureI2C(void);

int main()
{
	ConfigureSystem();
	ConfigureUART();

	MPU9150.address = 0x68;

	ConfigureI2C();
	//uint16_t x,y,z;

	MPU9150.init();
	UARTprintf("\nTest read: 0x%x", MPU9150.read(0x75));

	for (;;) {
		#ifdef DEBUG_LEVEL2
		UARTprintf("\nLED Loop");
		#endif

		MPU9150.getRawAccelData();
		UARTprintf("\nRaw Unsigned -> X: %6d, Y: %6d, Z: %6d",
						MPU9150.ui16_rawAccel[0],
						MPU9150.ui16_rawAccel[1],
						MPU9150.ui16_rawAccel[2]);

		UARTprintf("\nRaw Signed   -> X: %6d, Y: %6d, Z: %6d",
						MPU9150.i16_rawAccel[0],
						MPU9150.i16_rawAccel[1],
						MPU9150.i16_rawAccel[2]);


		// set the red LED pin high, others low
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, LED_RED|LED_BLUE);
		ROM_SysCtlDelay(5000000);
		ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_GREEN|LED_BLUE, 0);
		ROM_SysCtlDelay(5000000);
	}

}

uint8_t ConfigureI2C(void)
{
	//
	// The I2C0 peripheral must be enabled before use.
	// Give gating clock to I2C peripheral
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

	//
	// For this example I2C0 is used with PortB[3:2]. Hence enable gating clock
	// to GPIO_B block too
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//
	// Configure the pin muxing for I2C0 functions on port B2 and B3.
	ROM_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	ROM_GPIOPinConfigure(GPIO_PB3_I2C0SDA);

	//
	// Select the I2C function for these pins.  This function will also
	// configure the GPIO pins pins for I2C operation, setting them to
	// open-drain operation with weak pull-ups.  Consult the data sheet
	// to see which functions are allocated per pin.
	// TODO: change this to select the port/pin you are using.
	//
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);


	//
	// Enable and initialize the I2C0 master module.  Use the system clock for
	// the I2C0 module.  The last parameter sets the I2C data transfer rate.
	// If false the data rate is set to 100kbps and if true the data rate will
	// be set to 400kbps.  For this example we will use a data rate of 100kbps.
	//
	ROM_I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
	UARTprintf("\nI2C Initialized, Peripheral: I2C0, Pins: PB2 -> SCL, PB3 -> SDA");
	return 0;
}
uint8_t ConfigureSystem(void)
{

	// System clocks
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	// Enable Clock gating to GPIO_F
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	// COnfigure GPIO pins for RGB LED
	ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE|LED_GREEN);

	return 0;
}

uint8_t ConfigureUART(void)
{
	//
	// Enable the GPIO Peripheral used by the UART.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	//
	// Enable UART0
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	//
	// Configure GPIO Pins for UART mode.
	//
	ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//
	// Use the internal 16MHz oscillator as the UART clock source.
	//
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	//
	// Initialize the UART for console I/O. Speed: 115200
	//
	UARTStdioConfig(0, 115200, 16000000);

	UARTprintf("\nUART Initialized, Speed: 115200, Peripheral: UART0, Pins: PA0 -> RX, PA1 -> TX");

	return 0;
}
