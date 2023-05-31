/*
 * CShiftRegisterOutputExtender.h
 *
 *  Created on: 17.10.2020
 *      Author: mnl
 */

#ifndef CSHIFTREGISTEROUTPUTEXTENDER_H_
#define CSHIFTREGISTEROUTPUTEXTENDER_H_

#include "mbed.h"

/**
 * Modelliert die Realisierung zusätzlicher Ausgänge durch ein
 * gepuffertes 8-bit Schieberegister.
 */
class CShiftRegisterOutputExtender {
private:
	DigitalOut* m_serialData;
	DigitalOut* m_shiftRegisterClock;
	DigitalOut* m_bufferRegisterClock;
	uint8_t m_preparedOutput;
	uint8_t m_currentOutput;
	DigitalOut* m_led;

public:
	/*
	 * Erzeugt ein neues Objekt.
	 *
	 * @param serialData Zeiger auf den Ausgang, der mit dem Dateneingang
	 * 	des Schieberegisters verbunden ist
	 * @param ShiftRegisterClock Zeiger auf den Ausgang, der mit dem
	 *  Takteingang des Schieberegisters verbunden ist
	 * @param bufferRegisterClock Zeiger auf den Ausgang, der mit dem
	 *  Takteingang des Pufferregisters verbunden ist
	 */
	CShiftRegisterOutputExtender(DigitalOut* serialData,
			DigitalOut* shiftRegisterClock, DigitalOut* bufferRegisterClock, DigitalOut* led);

	/**
	 * Bereitet die Ausgabe des angegenbenen Wertes an den Ausgängen des
	 * Schieberegisters vor. D.h. die Methode schiebt den Wert in das
	 * Schieberegister, übernimmt ihn aber noch nicht in das Pufferregister.
	 *
	 * Bit 7 des übergebenen Wertes soll an QH ausgegeben werden, Bit 6
	 * an QG usw. Bit 0 wird an QA ausgegeben.
	 *
	 * @param value der in das Schieberegister zu übernehmende Wert
	 */
	void prepareOutput(uint8_t value);

	/**
	 * Übernimmt den Wert im Schieberegister in das Pufferregister und
	 * aktualisiert damit die Ausgänge.
	 */
	void updateOutput();

	/**
	 * Gibt den angegebenen Wert auf den Ausgängen aus. Die Methode
	 * kombiniert den Aufruf von prepareOutput und updateOutput.
	 *
	 * @param value der auszugebende Wert
	 */
	void write(uint8_t value);

	/**
	 * Stellt Funktion der Methode write als überladenen Zuweisungsoperator
	 * zur Verfügung.
	 *
	 * @param value der auszugebende Wert
	 */
	CShiftRegisterOutputExtender& operator= (uint8_t value);

	/**
	 * Liefert den aktuell an den Ausgängen ausgegeben Wert zurück.
	 *
	 * @return der Wert
	 */
	uint8_t read() const;

	/**
	 * Stellt die Funktion der Methode read als cast-Operator zur Verfügung.
	 *
	 * @return der aktuell an den Ausgängen ausgegebene Wert
	 */
	operator int () const;

	/**
	 * Hilfsmethode der Funktion prepareOutput(), die unter Nutzung
	 * der BSRR-Funktionalität implementiert wird.
	 *
	 * @param Zeiger auf GPIO-Takt
	 * @param Zeiger auf GPIO-Maske
	 * @param Zeiger auf Daten-Takt
	 * @param Zeiger auf Daten-Maske
	 */
	void doShift(GPIO_TypeDef* clockPort, uint32_t clockMask,
			GPIO_TypeDef* dataPort, uint32_t dataMask, uint8_t value);
};

#endif /* CSHIFTREGISTEROUTPUTEXTENDER_H_ */
