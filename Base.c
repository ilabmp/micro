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
void Init_ADC();

//Timer 관련
void Init_Timer();
void Handler1();
void Handler2();

//UART 관련
void Init_UART();
void UARTIntHandler0(void);
void UARTIntHandler1(void);
void UARTPrint0(const unsigned char *pucBuffer);
void UARTPrint1(const unsigned char *pucBuffer);
void UARTSend0(const unsigned char *pucBuffer, unsigned long ulCount);
void UARTSend1(const unsigned char *pucBuffer, unsigned long ulCount);

int index_j;
char buffer[BUFFSIZE];
char packet[30];
int index = 0;
unsigned long mydata_int[8];
char mydata_char[3][20];
char mydata[20][20];
char myIMU[40];
int size[3];
unsigned short count_start = 0;
volatile int yaw = 0;
volatile int pitch = 0;
unsigned long ADC_resultValue0 = 0;
unsigned long ADC_resultValue1 = 0;
unsigned long ADC_resultValue2 = 0;
int main(void) {

  //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_8MHZ);	//8MHz
	SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); //20MHz
	//SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);  //50Mhz

	Init_GPIO();
	Init_PWM();
	Init_UART();
	Init_Timer();
	Init_ADC();

	while (1) {

	}
}
void Init_ADC() {

	/**** ADC0 ***/
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC_BASE, 0);
	ADCProcessorTrigger(ADC_BASE, 0);
	//값읽기
	ADCProcessorTrigger(ADC_BASE, 0);
	while (!ADCIntStatus(ADC_BASE, 0, false))
		;
	ADCSequenceDataGet(ADC_BASE, 0, &ADC_resultValue0);


	/**** ADC1 ***/
	ADCSequenceConfigure(ADC_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 1, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH1);
	ADCSequenceEnable(ADC_BASE, 1);
	ADCProcessorTrigger(ADC_BASE, 1);
	//값 읽기
	ADCProcessorTrigger(ADC_BASE, 1);
	while (!ADCIntStatus(ADC_BASE, 1, false))
		;
	ADCSequenceDataGet(ADC_BASE, 1, &ADC_resultValue1);

	/**** ADC2 ***/
	ADCSequenceConfigure(ADC_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 2, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH2);
	ADCSequenceEnable(ADC_BASE, 2);
	ADCProcessorTrigger(ADC_BASE, 2);
	//값 읽기
	ADCProcessorTrigger(ADC_BASE, 2);
	while (!ADCIntStatus(ADC_BASE, 2, false))
		;
	ADCSequenceDataGet(ADC_BASE, 2, &ADC_resultValue2);

}
void Init_Timer() {
	//타이머0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 15);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE, TIMER_A, Handler1);
	TimerEnable(TIMER0_BASE, TIMER_A);
	//타이머1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 30);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER1_BASE, TIMER_A, Handler2);
	TimerEnable(TIMER1_BASE, TIMER_A);

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
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 399); //Set PWM width
	PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true); //Start PWM

	//PWM 2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); //Enable the GPIO port B
	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_0); // GPIO port B, pin 0 a PWM2
	PWMGenConfigure(PWM_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN); // Configuration the PWM gen0
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, 400); // Set the Period of the PWM Generator0
	PWMGenEnable(PWM_BASE, PWM_GEN_1); // Enable the PWM Generator0
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_2, 399); //Set PWM width
	PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true); //Start PWM

}
void Handler1() {
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}

void Init_GPIO() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_7);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_4);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_4);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);
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

	// Enable the peripherals used by this example.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	// Enable processor interrupts.
	IntMasterEnable();

	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	// Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	// Enable the UART interrupt.
	IntEnable(INT_UART1);

	UARTIntRegister(UART1_BASE, UARTIntHandler1);

	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

}

void Handler2() {
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	usnprintf(buffer, BUFFSIZE, "!%d%c%c%d|\n", 44, 'a', 'b', 32);
	UARTPrint((unsigned char *) buffer);
}
void UARTIntHandler1(void) {
	int i, k = 0;
	unsigned long ulStatus;
	char temp_char = 0;
	char before_temp_char = 0;
	ulStatus = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, ulStatus);

	while (UARTCharsAvail(UART1_BASE)) {
		temp_char = UARTCharGetNonBlocking(UART1_BASE);
		if ((temp_char != 0x0A) && (before_temp_char != 0x0D)) {
			myIMU[count_start] = temp_char;
			count_start++;
		} else if ((temp_char == 0x0A) && (before_temp_char == 0x0D)) {
			for (i = 0; i < count_start; i++) {
				if (myIMU[i] == ',') {
					size[index_j] = k;
					index_j++;
					k = 0;
				} else if (myIMU[i] == '*') {
					index_j = 0;
					k = 0;
				} else {
					mydata[index_j][k] = myIMU[i];
					k++;
				}
			}
			memset(mydata_char, 0, sizeof(mydata_char));
			strcpy(mydata_char[0], mydata[0]); //pitch
			strcpy(mydata_char[1], mydata[1]);
			strcpy(mydata_char[2], mydata[2]); //yaw

			/*초기화*/
			count_start = 0;
			memset(size, 0, sizeof(size));
			memset(myIMU, 0, sizeof(myIMU));
			memset(mydata, 0, sizeof(mydata));


			yaw = atoi(mydata_char[2]);
			pitch = atoi(mydata_char[0]);



		}
		before_temp_char = temp_char;
	}
}

void UARTIntHandler0() {
	unsigned long ulStatus;
	static char tempPacket = 0;

	ulStatus = UARTIntStatus(UART0_BASE, true);

	UARTIntClear(UART0_BASE, ulStatus);

	while (UARTCharsAvail(UART0_BASE)) {
		tempPacket = UARTCharGet(UART0_BASE);
	}
}

void UARTPrint0(const unsigned char *pucBuffer) {
	// Loop while there are more characters to send.
	while (*pucBuffer) { // Write the next character to the UART.
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
void UARTPrint1(const unsigned char *pucBuffer) {
	// Loop while there are more characters to send.
	while (*pucBuffer) { // Write the next character to the UART.
		UARTCharPut(UART1_BASE, *pucBuffer++);
	}
}

void UARTSend0(const unsigned char *pucBuffer, unsigned long ulCount) {
	// Loop while there are more characters to send.
	while (ulCount--) { // Write the next character to the UART.
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
void UARTSend1(const unsigned char *pucBuffer, unsigned long ulCount) {
	// Loop while there are more characters to send.
	while (ulCount--) { // Write the next character to the UART.
		UARTCharPut(UART1_BASE, *pucBuffer++);
	}
}
