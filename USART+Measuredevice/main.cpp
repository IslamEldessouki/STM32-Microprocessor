/**
 * Islam Eldessouki
 */
#include "mbed.h"
#include "breadboard.h"
#include "CShiftRegisterOutputExtender.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"
#include "CHexShifter.h"
#include "CMeasuringDevice.h"


/**
 * Definieren ein globales Objekt vom Typ BusOut
 * mit dem die 8 LEDs(0..7) kontrolliert werden
 */
BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3, BB_LED_4, BB_LED_5, BB_LED_6, BB_LED_7);

/**
 * Definieren ein globales Objekt vom Typ BusIn
 * mit dem den Zustand der 8 Taster(0..7) kontrolliert werden
 */
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3, BB_BTN_4, BB_BTN_5, BB_BTN_6, BB_BTN_7);


// DigitalOut led1(LED1);

//Definieren Vom Ausgang, der mit dem Dateneingang des Schieberegisters verbunden ist
DigitalOut ssdSerialData(SSD_SER);

//Definieren Vom Ausgang, der mit dem Takteingang des Schieberegisters verbunden ist
DigitalOut ssdShiftRegisterClock(SSD_SCLK);

//Definieren Vom Ausgang, der mit dem Takteingang des Pufferregisters verbunden ist
DigitalOut ssdBufferRegisterClock(SSD_RCLK);

//Definieren von common anode der 4-stelligen 7-Segment-Anzeige
DigitalOut ssdDigit0(SSD_DIGIT_0, OpenDrainNoPull);
DigitalOut ssdDigit1(SSD_DIGIT_1, OpenDrainNoPull);
DigitalOut ssdDigit2(SSD_DIGIT_2, OpenDrainNoPull);
DigitalOut ssdDigit3(SSD_DIGIT_3, OpenDrainNoPull);

/**
 * Implementierung der Lösung zur Aufgabe 1.
 */
void task1() {

	BufferedSerial usart(PA_2, PA_3, 115200);
	while (true) {
		// Schreiben Sie in der Endlosschleife den immer wieder auszuführenden
		// Code
		char data[] = { '*' };
		usart.write(data, 1);
	}
}
/**
 * Implementierung der Funktion task2
 * taske2 hat die gleiche Implementierung der task3
 * aus der Durchführung von Labor 2
 * aber sie hat noch eine kleine Änderungen.
 * Das explizite Setzen der Werte der Anzeigestellen und
 * des Dezimalpunkts werden entfernet, so dass nur die
 * Definition von ssd Objekt, die Definition von digitTimer und
 * die Endlosschleife mit der Abfrage des Timers und der Aktualisierung
 * von ssd übrig bleiben. Vor der Schleife ein Objekt vom Typ BufferedSerial
 * wird für die Kommunikation mit dem Terminal-Emulator ergänzt.
 * Die Schnittstelle wird auf "non blocking I/O" gestellt.
 * Ein Objekt application vom Typ CHexShifter wird definiert.
 */
void task2() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);
	BufferedSerial usart(PA_2, PA_3, 115200);
	usart.set_blocking(false);
	CHexShifter application(&ssd, &keys, &leds, &usart);

	while (true) {

		if (digitTimer.timeReached()) {
			ssd.activateNextDigit();
		}

		application.poll();
	}
}
/**
 * Implementierung der Funktion task3
 * taske hat die gleiche Implementierung der task2
 * aber sie hat noch eine kleine Änderungen.
 * Den Typ des Objekts application wird auf CMeasuringDevice geändert.
 */
void task3() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);
	BufferedSerial usart(PA_2, PA_3, 115200);
	usart.set_blocking(false);
	AnalogIn poti(BB_POTI, 3.3);
	CMeasuringDevice application(&ssd, &keys, &leds, &usart, &poti);

	while (true) {

		if (digitTimer.timeReached()) {
			ssd.activateNextDigit();
		}
		application.poll();
	}
}

int main() {

	//Aufruf der Funktion mode zum keys-Objekt
	//um die Pull-Down Widerstände einzuschalten
	keys.mode(PullDown);

	//Aufruf der Funktion task1
	//task1();

	//Aufruf der Funktion task2
	//task2();

	//Aufruf der Funktion task3
	task3();

	while (true) {
	}
}

