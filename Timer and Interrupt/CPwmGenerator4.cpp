/*
 * CPwmGenerator4.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: Islam Eldessouki
 */


#include "CPwmGenerator4.h"
#include "main.h"

void CPwmGenerator4::configureGpioPin() {
	// GPIO-Pin PC6auf alternate function 2 konfigurieren.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOC->AFR[0],GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);

	// Compare register 1 auf 10 setzen, PMW4 konfigutieren
	// und Ausgabe des OC1REF einschalten
	TIM3->CCR1 = 10 << 8;
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);
}

void CPwmGenerator4::configureTimer() {
	// Prescaler auf berechnete Wert setzen.
	TIM3->PSC = (20253 * m_period) / 1000000;
	TIM3->ARR = 16 * 256 - 1;

	// Update-Event zum Aktualisieren der gepufferten Register erzwingen,
	// das dadurch im SR gesetztes UIF loeschen und Timer starten.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

CPwmGenerator4::CPwmGenerator4(uint16_t period) {
	m_period = period + 1;
	// Timer mit Takt versorgen, sonst ist eine Konfiguration nicht möglich.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CCR1 = 0;
	m_pwmOut = 0;

	// Verwendete Peripheriegeräte konfigurieren.
	configureGpioPin();
	configureTimer();
	configureInterrupt();
	handleInterrupt();
}

void CPwmGenerator4::setDutyCycle(uint8_t percent)
{
	TIM3->CCR1 = (percent * TIM3->ARR) / 100 ;
	if(percent > 100)
	{
		TIM3->CCR1 = TIM3->ARR;
	}
}

/*
 * Interrupt-getrieben: Interrupts einschalten...
 */
void CPwmGenerator4::configureInterrupt()
{
	__NVIC_EnableIRQ(TIM3_IRQn);  // ...TIM3-Interrupts allgemein
	SET_BIT(TIM3->DIER, TIM_DIER_UIE); // ...speziell den Update-Interrupt
	SET_BIT(TIM3->DIER, TIM_DIER_CC1IE); // ...den CC1- Interrupt
	activeTIM3_IRQHandler = bind(&CPwmGenerator4::handleInterrupt, this);
}

/*
 * Interrupt-Handler fuer Timer3. Schaltet LED7 ein, wenn das
 * Update-Flag gesetzt ist, Schaltet LED7 aus, wenn das CC1IF gesetzt.
 */
void CPwmGenerator4::handleInterrupt()
{
	if(READ_BIT(TIM3->SR, TIM_SR_UIF))
	{
		CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
		*m_pwmOut = 1;
	}

	if(READ_BIT(TIM3->SR, TIM_SR_CC1IF))
	{
		CLEAR_BIT(TIM3->SR, TIM_SR_CC1IF);
		*m_pwmOut = 0;
	}

}

void CPwmGenerator4::setPwmOut(DigitalOut* pin)
{
	m_pwmOut = pin;
}

