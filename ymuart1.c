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

#define BUFFSIZE 32

void UARTIntHandler();

void Init_Timer();
void Init_GPIO();
void Init_ADC();
void Init_PWM();
void Init_UART();
void UARTIntHandler0(void);
void UARTPrint0(const unsigned char *pucBuffer);
void UARTSend0(const unsigned char *pucBuffer, unsigned long ulCount);

void Handler();
void Handler2();
void Init_Timer();
void ReadADC();
char buffer[BUFFSIZE];
double pwm = 1;
unsigned long ADC_resultValue = 1000;
int t = 1;
int main(void) {
//  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_8MHZ);
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

	Init_ADC();
	Init_PWM();
	Init_Timer();
	Init_UART();
	while (1) {

	}
}
void Init_PWM() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_2);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0);
	PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 400);
	PWMGenEnable(PWM_BASE, PWM_GEN_0);
	PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true);
}

void Init_Timer() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 100);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE, TIMER_A, Handler);
	TimerEnable(TIMER0_BASE, TIMER_A);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 1000);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER1_BASE, TIMER_A, Handler2);
	TimerEnable(TIMER1_BASE, TIMER_A);
}

void Handler() {
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, pwm);



}

void Handler2() {
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	ADCProcessorTrigger(ADC_BASE, 0);
	while (!ADCIntStatus(ADC_BASE, 0, false));
	ADCSequenceDataGet(ADC_BASE, 0, &ADC_resultValue);


	pwm = ((double) 400 / 1024) * ADC_resultValue;
	usnprintf(buffer, BUFFSIZE, "ADC : %d\n", (int)ADC_resultValue);
	UARTPrint0((unsigned char *) buffer);

}
void Init_ADC() {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC_BASE, 0);
	ADCProcessorTrigger(ADC_BASE, 0);
}
void UARTIntHandler0() {
	unsigned long ulStatus;
	static char tempPacket = 0;

	ulStatus = UARTIntStatus(UART0_BASE, true);

	UARTIntClear(UART0_BASE, ulStatus);

	while (UARTCharsAvail(UART0_BASE)) {
		tempPacket = UARTCharGetNonBlocking(UART0_BASE);
	}
}
void Init_UART(){
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

void UARTPrint0(const unsigned char *pucBuffer) {
	// Loop while there are more characters to send.
	while (*pucBuffer) { // Write the next character to the UART.
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
void UARTSend0(const unsigned char *pucBuffer, unsigned long ulCount) {
	// Loop while there are more characters to send.
	while (ulCount--) { // Write the next character to the UART.
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
