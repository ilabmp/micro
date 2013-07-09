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

void Init_PWM();
void Init_Timer();
void Init_GPIO();

//Timer 관련
void Init_Timer();
void Handler1();

//UART 관련
void Init_UART();
void UARTIntHandler0(void);

char buffer[BUFFSIZE];
static char button;
int main(void) {

  //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_8MHZ);  //8MHz
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); //20MHz
	//SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);  //50Mhz

	Init_GPIO();
	Init_PWM();
	Init_UART();
	Init_Timer();
	while (1) {

	}
}

void Init_Timer() {
	//타이머0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 50);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE, TIMER_A, Handler1);
	TimerEnable(TIMER0_BASE, TIMER_A);
}
void Init_PWM() {
	//PWM 0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM); //PWM Device Set
	SysCtlPWMClockSet(SYSCTL_PWMDIV_2); // PWM Generation clock Set
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Enable the GPIO port F
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0); // GPIO port F, pin 0 a PWM0
	PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); // Configuration the PWM gen0
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 400); // Set the Period of the PWM Generator0
	PWMGenEnable(PWM_BASE, PWM_GEN_0); // Enable the PWM Generator0

	PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true); //Start PWM

}

void Init_GPIO() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
}
void Init_UART() {
	// Enable the peripherals used by this example.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
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

void Handler1() {
	//타이머 실 함수
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if(button=='a' || button=='A'){
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 1); //Set PWM width
	}
	else if(button=='b' || button=='B'){
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 130); //Set PWM width
	}
	else if(button=='c' || button=='C'){
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 260); //Set PWM width
	}
	else if(button=='d' || button=='D'){
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 399); //Set PWM width
	}
	else{

	}
}

void UARTIntHandler0() {
	unsigned long ulStatus;
	ulStatus = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ulStatus);

	while (UARTCharsAvail(UART0_BASE)) {
		button = UARTCharGet(UART0_BASE);
	}
}
