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

}
void Init_ADC() {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC_BASE, 0);
	ADCProcessorTrigger(ADC_BASE, 0);
}
