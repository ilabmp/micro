void myTimer_b(void) {
  TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	static unsigned long ulValue;
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
	if (ulValue >= 500)
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, 0);
	else if (ulValue < 500)
		GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_0, GPIO_PIN_0);

}

//발광타이머 &&LED
void Init_Timer_a() {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Timer 사용
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER); // 32-bits periodic timer
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / 10);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER0_BASE, TIMER_A, myTimer_a);
	TimerEnable(TIMER0_BASE, TIMER_A);
}

//수광타이머
void Init_Timer_b() {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1); // Timer 사용
	TimerConfigure(TIMER1_BASE, TIMER_CFG_32_BIT_PER); // 32-bits periodic timer
	TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 1000);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntRegister(TIMER1_BASE, TIMER_A, myTimer_b);
	TimerEnable(TIMER1_BASE, TIMER_A);
}
void Init_PWM() {
	//PWM
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM); //Will use PWM
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_0); // change GPIO port F, pin 0 as PWM0
	PWMGenConfigure(PWM_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN); //Create PWM gen 0
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_0, 402); // Set the period for PWM Generator0
	PWMGenEnable(PWM_BASE, PWM_GEN_0); // Enable the PWM Generator0
	PWMPulseWidthSet(PWM_BASE, PWM_OUT_0, 1); //Set PWM width
	PWMOutputState(PWM_BASE, PWM_OUT_0_BIT, true);
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
//	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_0);
	Init_Timer_a();
	Init_Timer_b();
	Init_PWM();
	unsigned long ulValue;
	//
	// Enable the first sample sequence to capture the value of channel 0 when
	// the processor trigger occurs.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC); //Will use ADC
	ADCSequenceConfigure(ADC_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC_BASE, 0, 0,
			ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC_BASE, 0);
	SysCtlADCSpeedSet(SYSCTL_ADCSPEED_500KSPS);
	//
	// Trigger the sample sequence.
	//
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
	while (1) {
	}

}
