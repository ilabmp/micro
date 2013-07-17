#define BUFFSIZE 18

void UARTSend0(const unsigned char *pucBuffer, unsigned long ulCount);
void msg(void);
void UARTIntHandler0();

char data[20];
char data_2[20];
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
unsigned char header = 0x88;
unsigned char leagth = 0x14;
char p[3];
char p_2 = 0;
char q[4];
char temp;
char haptic = 0;

void myTimer(void) {

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	static int temp_a = 0, temp_b = 0, temp_c = 0;

	if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_4) == GPIO_PIN_4 && temp_a == 0) {

		p[0] = 1;
		temp_a = 1;
		msg();

	} else if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_4) == 0 && temp_a == 1) {

		p[0] = 0;
		temp_a = 0;
		msg();
	} else if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_5) == GPIO_PIN_5
			&& temp_b == 0) {

		p[0] = 2;
		temp_b = 1;
		msg();

	} else if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_5) == 0 && temp_b == 1) {
		p[0] = 0;
		temp_b = 0;
		msg();
	} else if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6) == GPIO_PIN_6
			&& temp_c == 0) {

		p[0] = 3;
		temp_c = 1;
		msg();
	} else if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6) == 0 && temp_c == 1) {
		p[0] = 0;
		temp_c = 0;
		msg();

	}


}
void myTimer_2(void) {

	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	if (p_2 == 1) {
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PIN_5);
	} else if (p_2 == 2) {
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, GPIO_PIN_6);
	} else if (p_2 == 3) {
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);
	} else if (p_2 == 0) {
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, 0);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);
	}

}

void msg(void) {
	temp = 0;
	data[0] = header;
	data[1] = leagth;

	int i = 0;

	for (i = 0; i < 3; i++) {

		data[i * 2 + 2] = (p[i] & 0xFF00) >> 8;
		data[i * 2 + 3] = p[i] & 0x00FF;
	}
	for (i = 0; i < 4; i++) {

		data[i * 2 + 8] = (q[i] & 0xFF00) >> 8;
		data[i * 2 + 9] = q[i] & 0x00FF;
	}
	data[16] = (haptic & 0xFF00) >> 8;
	data[17] = haptic & 0x00FF;

	for (i = 0; i < 18; i++) {
		temp += data[i];

	}
	data[18] = (temp & 0xFF00) >> 8;
	data[19] = temp & 0x00FF;
	UARTSend0((unsigned char*) data, 20);
}
void msg_2(void) {



	p_2 = ((data[2] & 0x00FF) << 8) + (data[3] & 0x00FF);



}

void Timer_A() {
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER); // 32-bits periodic timer
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 1000);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE, TIMER_A, myTimer);
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer_B() {
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER); // 32-bits periodic timer
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 1000);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER1_BASE, TIMER_A, myTimer_2);
	TimerEnable(TIMER1_BASE, TIMER_A);
}
void UARTSend0(const unsigned char *pucBuffer, unsigned long ulCount) {	// Loop while there are more characters to send.
	while (ulCount--) { // Write the next character to the UART.
		UARTCharPut(UART0_BASE, *pucBuffer++);
	}
}
void UARTIntHandler0() {
	unsigned long ulStatus;
	static int i = 0;
	ulStatus = UARTIntStatus(UART0_BASE, true);

	UARTIntClear(UART0_BASE, ulStatus);

	while (UARTCharsAvail(UART0_BASE)) {
		if (i < 21) {
			data_2[i] = UARTCharGet(UART0_BASE);
			i++;
		} else if (i == 21)
			i = 0;

	}
	msg_2();
}
int main(void) {

	/*SysCtlClockSet(
	 SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN
	 | SYSCTL_XTAL_8MHZ);*/
	SysCtlClockSet(
			SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_8MHZ); //20MHz

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);// Timer 사용
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_4);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_5);
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_6);
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // Enable processor interrupts.
	IntMasterEnable();
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); // Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE)); // Enable the UART interrupt.
	IntEnable(INT_UART0);

	UARTIntRegister(UART0_BASE, UARTIntHandler0);

	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	Timer_A();
	Timer_B();
	while (1) {
	}

}

