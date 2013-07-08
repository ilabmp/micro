#include "../../../inc/hw_ints.h"
#include "../../../inc/hw_memmap.h"
#include "../../../inc/hw_types.h"
#include "../../../driverlib/debug.h"
#include "../../../driverlib/pwm.h"
#include "../../../driverlib/timer.h"
#include "../../../driverlib/gpio.h"
#include "../../../driverlib/adc.h"
#include "../../../driverlib/interrupt.h"
#include "../../../driverlib/sysctl.h"
#include "../../../driverlib/pwm.h"
#include "../../../utils/ustdlib.c"
#include "../../../driverlib/uart.h"
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

#define BUFFSIZE 18

void Init_GPIO();


//UART 관련
void Init_UART();
void UARTIntHandler0(void);
void UARTPrint0(const unsigned char *pucBuffer);


char buffer[BUFFSIZE];

int main(void) {

  //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_8MHZ);  //8MHz
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); //20MHz
	//SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);  //50Mhz

	Init_GPIO();
	Init_UART();
	while (1) {

	}
}

void Init_GPIO() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

}
void Init_UART() {
	// Enable the peripherals used by this example.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Enable processor interrupts.
	IntMasterEnable();

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	// Enable the UART interrupt.
	IntEnable(INT_UART0);

	UARTIntRegister(UART0_BASE, UARTIntHandler0);

	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);


}
void UARTIntHandler0() {
	unsigned long ulStatus;
	static char tempPacket = 0;

	ulStatus = UARTIntStatus(UART0_BASE, true);

	UARTIntClear(UART0_BASE, ulStatus);

	while (UARTCharsAvail(UART0_BASE)) {
		tempPacket = UARTCharGet(UART0_BASE);
		UARTCharPut(UART0_BASE, tempPacket);
	}
}
void UARTPrint0(const unsigned char *pucBuffer) {
	// Loop while there are more characters to send.
	while (*pucBuffer) { // Write the next character to the UART.
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
