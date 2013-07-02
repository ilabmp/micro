int turn = 0;
int j;
int ulValue=0;
void myTimer1(void) {
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	static int k = 0;
	if (k)
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_PIN_0);
	else
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0);
	k = 1 - k;
	if (turn == 0) {
		j = 1;
		turn = 1;
	} else if (turn == 1) {
		j = 50;
		turn = 2;
	} else if (turn == 2) {
		j = 100;
		turn = 3;
	} else if (turn == 3) {
		j = 150;
		turn = 4;
	} else if (turn == 4) {
		j = 200;
		turn = 5;
	} else if (turn == 5) {
		j = 250;
		turn = 6;
	} else if (turn == 6) {
		j = 300;
		turn = 7;
	} else if (turn == 7) {
		j = 350;
		turn = 8;
	} else if (turn == 8) {
		j = 399;
		turn = 9;
	} else if (turn == 9) {
		j = 350;
		turn = 10;
	} else if (turn == 10) {
		j = 300;
		turn = 11;
	} else if (turn == 11) {
		j = 250;
		turn = 12;
	} else if (turn == 12) {
		j = 200;
		turn = 13;
	} else if (turn == 13) {
		j = 150;
		turn = 14;
	} else if (turn == 14) {
		j = 100;
		turn = 15;
	} else if (turn == 15) {
		j = 50;
		turn = 16;
	} else if (turn == 16) {
		j = 1;
		turn = 0;
	}
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_1, j); //Set PWM width

}

void myTimer2(void) {

	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ADCProcessorTrigger(ADC_BASE, 0);
	//
	// Wait until the sample sequence has completed.
	//
	while (!ADCIntStatus(ADC_BASE, 0, false)) {
	}
	//
	// Read the value from the ADC.
	//
	ADCSequenceDataGet(ADC_BASE, 0, &ulValue);
	if (ulValue=>500)
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 1);


}



int main(void) {

SysCtlClockSet(
		SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ); //20MHz
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG); // 내부 LED 초기화
GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0); //해당 포트를 출력으로 쓰겠다.
SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM); //Will use PWM
SysCtlPWMClockSet(SYSCTL_PWMDIV_2); // Set Clock for PWM
GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1); // change GPIO Port F, pin 0 as PWM0
PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); //Create PWM gen 0
PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 400); // Set the period for PWM Generator0
PWMGenEnable(PWM_BASE, PWM_GEN_0);	// Enable the PWM Generator0

PWMOutputState(PWM_BASE, PWM_OUT_1_BIT, true);

GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_1);
//--타이머1시작---
SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Timer 사용
TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER); // 32-bits periodic timer
TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 10);
TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
TimerIntRegister(TIMER0_BASE, TIMER_A, myTimer1); //pwm타이머1
TimerEnable(TIMER0_BASE, TIMER_A);
//---타이머1끝----

//--타이머2시작---
SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // Timer 사용
TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER); // 32-bits periodic timer
TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 10);
TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
TimerIntRegister(TIMER1_BASE, TIMER_A, myTimer2); //수광타이머
TimerEnable(TIMER1_BASE, TIMER_A);
//---타이머2끝----

while (1) //무한루프걸어줌
{
}

unsigned long ulValue;
//
// Enable the first sample sequence to capture the value of channel 0 when
// the processor trigger occurs.
//
ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
ADCSequenceStepConfigure(ADC_BASE, 0, 0,
		ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
ADCSequenceEnable(ADC_BASE, 0);
//
// Trigger the sample sequence.
//
ADCProcessorTrigger(ADC_BASE, 0);
//

}
