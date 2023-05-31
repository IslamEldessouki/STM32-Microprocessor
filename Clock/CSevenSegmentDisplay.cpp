/*
 * CSevenSegmentDisplay.cpp
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#include "CSevenSegmentDisplay.h"

CSevenSegmentDisplay* CSevenSegmentDisplay::m_instance ;


extern "C" void TIM1_UP_TIM10_IRQHandler()
{
	if(READ_BIT(TIM10->SR, TIM_SR_UIF))
	{
		CLEAR_BIT(TIM10->SR, TIM_SR_UIF);
		if(CSevenSegmentDisplay::m_instance->m_activeDigit > 3)
		{
			CSevenSegmentDisplay::m_instance->m_activeDigit = 0;
		}
		CSevenSegmentDisplay::m_instance->activateAllDigits();
		CSevenSegmentDisplay::m_instance->activateNextDigit();
	}

	if(READ_BIT(TIM10->SR, TIM_SR_CC1IF))
	{
		CLEAR_BIT(TIM10->SR, TIM_SR_CC1IF);
		CSevenSegmentDisplay::m_instance->deactivateAllDigits();
		CSevenSegmentDisplay::m_instance->m_activeDigit ++;
	}
}

uint8_t CSevenSegmentDisplay::patterns[] = {0xD7 ,0x11,0x8F,0x9B,0x59,0xDA,0xDE,0x91,0xDF,
		0xDB,0xDD,0x5E,0xC6,0x1F,0xCE,0xCC};

CSevenSegmentDisplay::CSevenSegmentDisplay(CShiftRegisterOutputExtender* ssdSegments,
		DigitalOut* digits[4])
{// Übergeben Werte in Attribten speichern
	m_ssdSegments = ssdSegments;
	for (int digit = 0; digit < 4; digit++) {
		m_digits[digit] = digits[digit];
	}

	// Array mit anzuzeigenden Ziffern initialisieren
	for (int digit = 0; digit < 4; digit++) {
		m_values[digit] = 0;
	}

	// Als Startzustand wird Stelle 0 aktiviert und alle Segmente werden
	// ausgeschaltet.
	*m_digits[0] = 1;
	for (int digit = 1; digit < 4; digit++) {
		*m_digits[0] = 0;
	}
	m_activeDigit = 0;
	m_ssdSegments->write(0xff);

	// Timer mit Takt versorgen, sonst ist eine Konfiguration nicht möglich.
	RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
	TIM10->CCR1 = 0;
	m_instance = this ; //weisen den Zeiger auf das erzeugte Objekt zu.

	compareInterrupt();
	initTimer();
}

void CSevenSegmentDisplay::activateNextDigit() {

	if(m_activeDigit == (~m_decimalDigit + 4 ) )
	{
		m_ssdSegments->prepareOutput(~(0x20));
		m_ssdSegments->updateOutput();
	}
	m_ssdSegments->prepareOutput(~m_values[m_activeDigit]);
	m_ssdSegments->updateOutput();
}

uint8_t& CSevenSegmentDisplay::operator[] (int position) {
	static uint8_t dummy = 0;
	if (position < 0 || position > 3) {
		return dummy;
	}
	return m_values[position];
}

void CSevenSegmentDisplay::setDecimalPoint(int digit)
{
	if(digit >= 0 && digit < 4)
	{
		m_decimalDigit = digit;
	}
	else
	{
		m_decimalDigit = 0;
	}
}

void CSevenSegmentDisplay::setValue(int value)
{
	if(value >= 0 && value < 9999)
	{
		m_values[3] = patterns[(value %10)];
		value = value / 10;
		m_values[2] = patterns[(value %10)];
		value = value / 10;
		m_values[1] = patterns[(value %10)];
		value = value / 10;
		m_values[0] = patterns[(value %10)];
	}
	else
	{
		m_values[0] = patterns[14];
		m_values[1] = patterns[14];
		m_values[2] = patterns[14];
		m_values[3] = patterns[14];
	}
}

CSevenSegmentDisplay& CSevenSegmentDisplay::operator= (int value)
{
	setValue(value);
	return *this;
}


void CSevenSegmentDisplay::setValue(float value)
{
	int intgerValue;
	float temp = abs(value);
	m_decimalDigit = 0;
	temp = temp - int(temp);

	while(temp != 0 && m_decimalDigit <3)
	{
		temp = temp * 10;
		m_decimalDigit = m_decimalDigit + 1;
		temp = temp - int(temp);
	}

	if(value < 0.001)
	{
		intgerValue = 0;
		m_decimalDigit = 3;
	}
	else{

		intgerValue = int (value * pow(10, m_decimalDigit ));
	}
	setValue(intgerValue);
}


CSevenSegmentDisplay& CSevenSegmentDisplay::operator= (float value)
{
	setValue(value);
	return *this;
}


void CSevenSegmentDisplay::initTimer()
{

	// Timer konfigurieren
	// Prescaler auf berechnete Wert setzen.
	TIM10->PSC = (20253 * 5000) / 1000000;
	TIM10->ARR = 16 * 256 - 1;

	// Update-Event zum Aktualisieren der gepufferten Register erzwingen,
	// das dadurch im SR gesetztes UIF loeschen und Timer starten.
	SET_BIT(TIM10->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM10->SR, TIM_SR_UIF);
	SET_BIT(TIM10->CR1, TIM_CR1_CEN);

	// Interrupt konfigurieren
	__NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);  // ...TIM10-Interrupts allgemein
	SET_BIT(TIM10->DIER, TIM_DIER_UIE); // ...speziell den Update-Interrupt
	SET_BIT(TIM10->DIER, TIM_DIER_CC1IE); // ...den CC1- Interrupt
}

void CSevenSegmentDisplay::compareInterrupt()
{
	// GPIO-Pin PC6auf alternate function 2 konfigurieren.
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODE6, 2 << GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOC->AFR[0],GPIO_AFRL_AFSEL6,
			GPIO_AF3_TIM10 << GPIO_AFRL_AFSEL6_Pos);

	// Compare register 1 auf 10 setzen, PMW4 konfigutieren
	// und Ausgabe des OC1REF einschalten
	TIM10->CCR1 = 10 << 8;
	MODIFY_REG(TIM10->CCMR1, TIM_CCMR1_OC1M, 6 << TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM10->CCER, TIM_CCER_CC1E);
}

void CSevenSegmentDisplay::setBrightness(int percent)
{
	if(percent >= 5 && percent <= 100)
	{
		m_brightness = percent;
	}
	else if(percent < 5)
	{
		m_brightness = 5;
	}
	else if(percent > 100)
	{
		m_brightness = 100;
	}

	setDutyCycle();
}

int CSevenSegmentDisplay::getBrightness()
{

	return m_brightness;
}

void CSevenSegmentDisplay::setDutyCycle()
{
	TIM10->CCR1 = (getBrightness() * TIM10->ARR) / 100 ;
}

void CSevenSegmentDisplay::deactivateAllDigits()
{
	*m_digits[m_activeDigit] = 0;
}

void CSevenSegmentDisplay::activateAllDigits()
{
	*m_digits[m_activeDigit] = 1;
}




