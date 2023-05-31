/*
 * CPwmGenerator2.cpp
 *
 *  Created on: Jan 4, 2021
 *      Author: Islam Eldessouki
 */


#include "CPwmGenerator2.h"

void CPwmGenerator2::configureGpioPin() {
	// GPIO-Pin PC6auf alternate function 2 konfigurieren.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOC->AFR[0],GPIO_AFRL_AFSEL6,
			GPIO_AF2_TIM3 << GPIO_AFRL_AFSEL6_Pos);

	// Compare register 1 auf 10 setzen, PMW2 konfigutieren
	// und Ausgabe des OC1REF einschalten
	TIM3->CCR1 = 10 << 8;
	MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM3->CCER, TIM_CCER_CC1E);
}

void CPwmGenerator2::configureTimer() {
	// Prescaler auf berechnete Wert fuer 256 Hz Output setzen.
	TIM3->PSC = 62500;
	TIM3->ARR = 16 * 256 - 1;

	// Update-Event zum Aktualisieren der gepufferten Register erzwingen,
	// das dadurch im SR gesetztes UIF loeschen und Timer starten.
	SET_BIT(TIM3->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
	SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

CPwmGenerator2::CPwmGenerator2() {
	// Timer mit Takt versorgen, sonst ist eine Konfiguration nicht möglich.
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CCR1 = 0;

	// Verwendete Peripheriegeräte konfigurieren.
	configureGpioPin();
	configureTimer();
}

void CPwmGenerator2::setDutyCycle(uint8_t percent)
{
	TIM3->CCR1 = (percent * TIM3->ARR) / 100 ;
	if(percent > 100)
	{
		TIM3->CCR1 = TIM3->ARR;
	}
}

