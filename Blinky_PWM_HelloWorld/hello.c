#define TARGET_IS_TM4C123_RA1

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"

/* UART includes */
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#define LED_RED GPIO_PIN_1
#define LED_BLUE GPIO_PIN_2
#define LED_GREEN GPIO_PIN_3

uint8_t ConfigureUART(void);
uint8_t ConfigurePWM(void);

int main()
{
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, LED_RED|LED_BLUE);

   ConfigureUART();
   uint32_t clockRate = ROM_SysCtlClockGet();
   UARTprintf("\nClock Frequency : %u", clockRate);

   ConfigurePWM();
   static uint16_t i = 0;
   for (;;) {
	i++;
	if (i==400)
		i=0;
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, i);
	// set the red LED pin high, others low
	//ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE, LED_RED|LED_BLUE);
	ROM_SysCtlDelay(5000000);
	//ROM_GPIOPinWrite(GPIO_PORTF_BASE, LED_RED|LED_BLUE, 0);
	ROM_SysCtlDelay(5000000);
	UARTprintf("\nTest");
    }

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

uint8_t ConfigurePWM(void)
{
	ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_4);		// 80Mhz dividev by 4 = 20MHz

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 400);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 10);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);

	return 0;
}
