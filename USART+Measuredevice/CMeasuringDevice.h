/*
 * CMeasurementDevice.h
 *
 *  Created on: Nov 4, 2020
 *      Author: mnl
 */

#ifndef CMEASURINGDEVICE_H_
#define CMEASURINGDEVICE_H_

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"

/**
 * Repräsentiert die realisierte Anwendung "Messgerät".
 */
class CMeasuringDevice {
private:
	CSevenSegmentDisplay* m_display;
	CDebouncer* m_keys;
	BusOut* m_leds;
	BufferedSerial* m_usart;
	AnalogIn* m_voltageInput;
	uint8_t keysPushed;
	uint8_t modeKeys;
	uint8_t singleMeasurement;
	uint8_t continuousMeasurement;
	float measurementValue;

public:
	/**
	 * Erzeugt ein neues Exemplar der Anwendung, das die angegebenen
	 * Hardware-Komponenten benutzt.
	 *
	 * Die Taster werden über ein Exemplar von CDebouncer entprellt.
	 * Da die Klasse CDebouncer keinen Default-Konstruktor hat, kann
	 * das Objekt nicht als Attribut definiert werden, sondern muss im
	 * Konstruktor auf dem Heap erzeugt und im Destruktor wieder
	 * freigegeben werden.
	 *
	 * @param display die 7-Segment-Anzeige
	 * @param keys die Eingänge, an die die Taster angeschlossen sind
	 * @param leds die Ausgänge, an die die LEDs angeschlossen sind
	 * @param usart die serielle Schnittstelle
	 * @param voltageInput der Analogeingang
	 */
	CMeasuringDevice(CSevenSegmentDisplay* display, BusIn* keys,
			BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput);

	/**
	 * Gibt die allokierten Ressourcen wieder frei.
	 */
	~CMeasuringDevice();

	/**
	 * Ruft die privaten poll...-Methoden zur Verarbeitung der
	 * verschiedenen Eingabequellen auf.
	 */
	void poll();
	/**
	 * Ermöglicht die gedrueckten Tasten einzusetzen.
	 * Ein sehr wichtiger Hinweis: die Taste muss einmal gedrueckt werden,
	 * damit die Modi aktifiert werden kann. Die Taste muss auch nochmal
	 * gedruckt werden, damit die Modi deaktiviert werden kann und eine andere Modi
	 * aktiviert werden kann.
	 */
	void pollKeys();
	/**
	 * Der gemessene Wert wird immer auf der 7-Segment-Anzeige dargestellt.
	 * Die einstellbaren Anzeige-Modi beeinflussen nur
	 * die Darstellung des Messwerts mit Hilfe der LEDs.
	 */
	void pollSegmentMode();
	/**
	 * Wenn die Taste 5 "Binär" gedrückt wird, werden die LEDs
	 * zur Darstellung des Messwerts als Binärzahl verwendet,
	 * wobei alle "LEDs an" für den Maximalwert stehen.
	 */
	void pollBinaryMode();
	/**
	 * Wenn die Taste 6 "BCD" gedrückt wird, werden die LEDs 7-4
	 * und 3-0 jeweils für die Anzeige einer BCD-Ziffer verwendet.
	 */
	void pollBCDMode();
	/**
	 * Wenn die Taste 7 "Balken" gedrückt, werden die LEDs als
	 * Balkenanzeige benutzt. Es leuchtet je nach Messwert entweder
	 * keine LED oder die LED ganz links oder die beiden linken
	 * LEDs oder die ersten drei LEDs von links, die ersten vier
	 * LEDs von links usw. bis alle LEDs leuchten.
	 * Der Messwert wird auf das Intervall (0..9) skaliert.
	 * vom Ergebnis wird der Ganzzahlanteil genommen, der
	 * damit im Intervall (0..8) liegt. Damit wird gewusst,
	 * wie viele 1-Werte in das Bitmuster für die Anzeige geschoben werden müssen.
	 */
	void pollBalkenMode();
	/**
	 * Der Messwert wird über die serielle Schnittstelle an den PC übertragen.
	 * Die Darstellung erfolgt als Fließkommazahl mit Einheit („V“).
	 * Der Messwert wird immer linksbündig in der gleichen Zeile angezeigt.
	 */
	void pollPCMode();
	/**
	 * Wenn die Taste „Start“ der Einzelmessung gedrückt wird,
	 * wird der aktuelle Wert des ADC ausgelesen und zur Anzeige gebracht.
	 */
	void pollSingleMeasurement();
	/**
	 * Wenn die Taste Start (Fortlaufende Messung) gedrückt wird,
	 * wird der Wert des ADC alle 100 ms neu ausgelesen und angezeigt.
	 * Wird während der fortlaufenden Messung die Taste "Hold" gedrückt,
	 * werden keine neuen Werte vom ADC geholt und es wird der zuletzt
	 * (vor Drücken der Taste "Hold") gemessene Wert permanent angezeigt.
	 * Erneutes Drücken der Taste "Hold" setzt die Messung fort.
	 * Durch Drücken der Taste "Stop" wird die periodische Messung
	 * beendet und die Anzeige auf 0 gestellt.
	 *
	 */
	void pollContinuousMeasurement();
	/**
	 * Ermöglicht Das Messgerät alternativ über die serielle Schnittstelle
	 * zu bedienen. Dazu werden Informationen über neu eingestellte Modi
	 * am PC ausgegeben und es ist möglich, die Modi über die Tastatur des PC einzustellen.
	 * Ein sehr wichtiger Hinweis: die Taste muss einmal gedrueckt werden,
	 * damit die Modi aktifiert werden kann. Die Taste muss auch nochmal
	 * gedruckt werden, damit die Modi deaktiviert werden kann und eine andere Modi
	 * aktiviert werden kann.
	 */
	void pcKeys();
};


#endif /* CMEASURINGDEVICE_H_ */
