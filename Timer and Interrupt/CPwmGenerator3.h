/*
 * CPwmGenerator3.h
 *
 *  Created on: Jan 5, 2021
 *      Author: Islam Eldessouki
 */

#ifndef CPWMGENERATOR3_H_
#define CPWMGENERATOR3_H_

#include "mbed.h"

/**
 * Modelliert unter Nutzung des Timer 3 einen Generator für ein
 * PWM-Signal auf PC6. Die Periode ist mit einer Ausgangsfrequenz des
 * Prescalers von 256 Hz und einem Counter-Modulo von 16*256 fest
 * auf eine bestimmte Periode eingestellt, das Ein-/ausverhältnis auf 10:6.
 */
class CPwmGenerator3 {
private:
	uint16_t m_period;

	/**
	 * Konfiguriert PC6 als Alternate Function-Ausgang und selektiert
	 * den von Timer 3/Compare Register 1 generierten Wert als das
	 * auszugebende Signal.
	 */
	void configureGpioPin();

	/**
	 * Konfiguriert den Timer 3 inklusive des Compare Register 1 so,
	 * dass ein Signal wie in der Klassenbeschreibung dargestellt
	 * erzeugt wird.
	 */
	void configureTimer();


public:
	/**
	 * Erzeugt ein PWM Signal mit einer 20 ms Periodendauer
	 *
	 * @param die Periodendauer des Signals in micro sekunde.
	 */
	CPwmGenerator3(uint16_t period);
	/**
	 * Stellt diese Methode den Tastgrad ein. Der Wert des Arguments
	 * muss zwischen 0 und 100 liegen, Werte größer 100 werden wie 100 behandelt.
	 *
	 * @param da Porezent vom gefiltertem Wert Von ADC(0 bis 100).
	 */
	void setDutyCycle(uint8_t percent);
};




#endif /* CPWMGENERATOR3_H_ */
