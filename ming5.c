#define BUFFSIZE 18
void Init_UART();
void UARTIntHandler0(void);


char buffer[BUFFSIZE];
char packet[30];

void Init_PWM_1() {
  //PWM1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM); //Will use PWM
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0); // change GPIO port F, pin 0 as PWM0
	PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); //Create PWM gen 0
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 402); // Set the period for PWM Generator0
	PWMGenEnable(PWM_BASE, PWM_GEN_0); // Enable the PWM Generator0
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 1); //Set PWM width
	PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true);
}
void Init_UART() { // Enable the peripherals used by this example.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Enable processor interrupts.
	IntMasterEnable();
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); // Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)); // Enable the UART interrupt.
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
	}

	if (tempPacket == 'a' || tempPacket == 'A') {

		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 1);
	} else if (tempPacket == 'b' || tempPacket == 'B') {
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 130);

	} else if (tempPacket == 'c' || tempPacket == 'C') {
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 270);
	} else if (tempPacket == 'd' || tempPacket == 'D') {
		PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 400);
	}
}
int main(void) {

	/*SysCtlClockSet(
	 SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN
	 | SYSCTL_XTAL_8MHZ);*/
	SysCtlClockSet(
			SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_8MHZ); //20MHz

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);

	Init_PWM_1();

	Init_UART();

	while (1) {
	}

}
