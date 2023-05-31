/*
 * CSevenSegmentDisplay.h
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */



#ifndef CSEVENSEGMENTDISPLAY_H_
#define CSEVENSEGMENTDISPLAY_H_

extern "C" void TIM1_UP_TIM10_IRQHandler();


#include "CShiftRegisterOutputExtender.h"

/**
 * Modelliert eine 4-stellige 7-Segment-Anzeige. Um eine "aktive" und
 * flackerfreie Darstellung zu erhalten, muss sichergestellt werden,
 * dass die Methode "activateNextDigit" mindestens alle 5 ms aufgerufen wird.
 */
class CSevenSegmentDisplay {

	/**
	 * Interrupt Handler
	 */
	friend void TIM1_UP_TIM10_IRQHandler();

private:
	CShiftRegisterOutputExtender* m_ssdSegments;
	DigitalOut* m_digits[4];
	uint8_t m_activeDigit;
	uint8_t m_values[4];
	uint8_t m_decimalDigit;
   static CSevenSegmentDisplay* m_instance ;
   int m_brightness;

	/**
	 * Hilfsmethode für das Zeitmultiplexverfahren. Aktiviert die nächste
	 * Anzeigestelle.
	 */
	void activateNextDigit();

	/**
	 * Diese neue Methode soll den Timer 10 so konfigurieren,
	 * dass er alle 5 ms einen Update-Interrupt auslöst.
	 * Die Schritte für das Konfigurieren des Timers 10 sind bis auf
	 * den Interrupt-Handler die gleichen wie für das Konfigurieren des Timers 3.
	 */
	void initTimer();

	/**
	 * konfiguriert den comper-Interrupt
	 */
	void compareInterrupt();

	/**
	 * Hilfsmethode, die die Helligkeit einstellt
	 */
	void setDutyCycle();
	/**
	 * Deaktiviert alle Stellen der Seben-Segment-Anzeige
	 */
	void deactivateAllDigits();
	/**
	 * Aktiviert alle Stellen der Seben-Segment-Anzeige
	 */
	void activateAllDigits();


public:
	static uint8_t patterns[];
	/**
	 * Erzeugt ein neues Objekt, das eine 4-stellige 7-Segment-Anzeige
	 * ansteuert, deren Segmente (und Dezimalpunkt) an das über
	 * ssdSegments ansteuerbare Schieberegister angeschlossen sind und
	 * deren Anzeigestellen über die übergebenen digitalen Ausgänge
	 * aktiviert werden können.
	 *
	 * @param ssdSegments die Modellierung des Schieberegisters zur
	 *  Ansteuerung der Segemente
	 * @param digits die Ausgänge zum Aktivieren der Anzeigestellen
	 */
	CSevenSegmentDisplay(CShiftRegisterOutputExtender* ssdSegments,
			DigitalOut* digits[4]);


	/**
	 * Ermöglicht den Zugriff auf die an der ausgewählten Anzeigestelle
	 * darzustellende Ziffer. Da eine Referenz auf den gespeicherten Wert
	 * zurückgeliefert wird, kann der Operator sowohl als Ziel einer
	 * Zuweisung als auch zum Abfragen des aktuell gespeicherten Wertes
	 * verwendet werden.
	 *
	 * @param position Index der Anzeigestelle (0 für Stelle ganz rechts)
	 * @return Referenz auf den an der ausgewählten Stelle anzuzeigenden Wert
	 */
	uint8_t& operator[] (int position);

	/**
	 * Sorgt dafür, dass an der angegeben Anzeigestelle (0 ganz rechts, 3 ganz links)
	 * zusätzlich der Dezimalpunkt leuchtet.
	 *
	 * @param digit Index der Anzeigestelle, die den Dezimalpunkt leuchtet.
	 */
	void setDecimalPoint(int digit);

	/**
	 * Zeigt den angegebenen Wert an.
	 * Wenn der Wert negativ oder größer 9999 ist, soll „EEEE“ angezeigt werden.
	 * Es wird kein Dezimalpunkt angezeigt.
	 *
	 * @param value der angegebene Wert.
	 */
	void setValue(int value);

	/**
	 * Stellt die Funktion der Methode setValue als ZuweisungsOperator zur Verfügung.
	 *
	 * @param value der angegebene Wert.
	 * @return Referenz auf das Object CSevenSegmentDisplay.
	 */
	CSevenSegmentDisplay& operator= (int value);

	/**
	 * Zeigt die den angegeben Wert mit korrekt gesetztem Dezimalpunkt
	 * und unter Ausnutzung aller Stellen an.
	 * Für Werte kleiner 0,001 wird 0,000 angezeigt und für negative
	 * Werte oder Werte größer „9999,“ soll „EEEE“ angezeigt werden.
	 * Die korrekte Positionierung des Dezimalpunkts
	 * wird mit einer Schleife  Implementiert.
	 *
	 * @param value der angegebene Wert.
	 */
	void setValue(float value);

	/**
	 * Stellt die Funktion der Methode setValue als ZuweisungsOperator zur Verfügung.
	 *
	 * @param value der angegebene Wert.
	 * @return Referenz auf das Object CSevenSegmentDisplay.
	 */
	CSevenSegmentDisplay& operator= (float value);

	/**
	 * Stellt die Helligkeit zwischen 5% und 100% ein
	 * (bezogen auf die maximal mögliche Helligkeit).
	 * Argumente kleiner 5 werden wie 5 behandelt, Argumente größer 100 wie 100.
	 *
	 * @param das Porezent vom Wert der Helligkeit
	 */
	void setBrightness(int percent);

	/**
	 * @return das Porezent vom Wert der Helligkeit
	 */
	int getBrightness();



};

#endif /* CSEVENSEGMENTDISPLAY_H_ */
