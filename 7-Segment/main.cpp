/**
 * Islam Eldessouki
 */
#include "mbed.h"
#include "breadboard.h"
#include "CShiftRegisterOutputExtender.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"
#include "CMovingAverageFilter.h"
#include "CExponentialFilter.h"
/**
 * Definieren ein globales Objekt vom Typ BusIn
 * mit dem den Zustand der 8 Taster(0..7) kontrolliert werden
 */
BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3, BB_BTN_4, BB_BTN_5, BB_BTN_6, BB_BTN_7);



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

//Definieren ein globales Objekt vom Typ DigitalOut
//für die erste led.
DigitalOut led0(BB_LED_0);




/**
 * Implementierung der Funktion task1
 * Die Funktion weist zunächst den im Ausgangsprojekt
 * vordefinierten DigitalOut Objekten ssdDigit0 bis ssdDigit3 zu.
 * Mit dem wert 1 wird die Anzeige aktiviert
 * und mit dem Wert 0 wird die inaktiviert.
 * Die Funktion erzeugt dann ein Objekt ssdSegments,
 * die vom Typ CShiftRegisterOutputExtender ist.
 * In der Endlosschleife werden die Werte
 * der Taster als uint8_t gelesen, dann wird der Wert bitweise negiert
 * und wird das Ergebnis dem Objekt ssdSegments zugewiesen.
 */
void task1() {

	ssdDigit0 = 1;
	ssdDigit1 = 0;
	ssdDigit2 = 0;
	ssdDigit3 = 0;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock,
			&ssdBufferRegisterClock );
	uint8_t keysValue;

	while (true) {

		keysValue = keys;
		keysValue = ~keysValue;
		ssdSegments.write(keysValue);

	}
}// end of (task1)


/**
 * Implementierung der Funktion task2
 * taske2 hat die gleiche Implementierung der task1,
 * aber sie hat noch eine kleine Änderungen.
 * Ein Array uint8_t patterns[16] wird als lokale Variable erstellt
 * und initialisiert so, dass es die Index-Werte 0-9 auf die entsprechenden
 * 7-Segment-Muster für die Ziffern 0-9 und die Werte 10-15 auf
 * die Segment-Muster für die Buchstaben A-F abbildet.
 * Nur der Wert der vier niederwertigsten Bit der Taster wird ausgewählt.
 */
void task2() {

	ssdDigit0 = 1;
	ssdDigit1 = 0;
	ssdDigit2 = 0;
	ssdDigit3 = 0;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock,
			&ssdBufferRegisterClock );
	uint8_t keysValue;
	uint8_t patterns[16] = {0xD7 ,0x11,0x8F,0x9B,0x59,0xDA,0xDE,0x91,0xDF,
			0xDB,0xDD,0x5E,0xC6,0x1F,0xCE,0xCC};


	while (true) {

		keysValue = keys & 0x0F;
		patterns[keysValue] = patterns[keysValue];
		ssdSegments.write(~patterns[keysValue]);

	}

}// end of (task2)

/**
 * Implementierung der Funktion task3
 * taske3 hat die gleiche Implementierung der task2,
 * aber sie hat noch eine kleine Änderungen.
 * Zusätzlich wird ein Objekt ssd vom Typ CSevenSegmentDisplay definiert.
 * Die von ssd anzuzeigenden Ziffern werden auf die letzten
 * vier Stellen meiner Matrikelnummer gesetzt.
 * Ein Objekt digitTimer vom Typ CPolledTimer wird
 * mit einer Taktperiode von 5 ms definiert.
 * In der Endlosschleife wird geprüft, ob der Timer abgelaufen ist.
 * Wenn ja, wird die Methode activateNextDigit von ssd aufgerufen.
 */
void task3() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;

	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);

	//die letzten vier Stellen meiner Matrikelnummer(5,1,5,9)
	ssd[0] = CSevenSegmentDisplay::patterns[5];
	ssd[1] = CSevenSegmentDisplay::patterns[1];
	ssd[2] = CSevenSegmentDisplay::patterns[5];
	ssd[3] = CSevenSegmentDisplay::patterns[9];

	while (true) {

		if(digitTimer.timeReached())
		{
			ssd.setDecimalPoint(0);
			ssd.activateNextDigit();

		}


	}


}// end of (task3)

/**
 * Implementierung der Funktion task4
 * task4 hat die gleiche Implementierung der task3,
 * aber sie hat noch eine kleine Änderungen.
 * Ein weiterer CPolledTimer mit Namen showNext und einer
 * Periode von einer Sekunde wird definiert.
 * Wenn showNext Timer abgelaufen ist, wird der nächste
 * anzuzeigende Wert aus der Folge 0,0001234; 0,001234; 0,01234; 0,123;
 * 1,234; 12,34; 123,4; 1234,0; 12340,0 gesetzt.
 * D. h. Es wird mit dem Wert 0,0001234 gestart,
 * diesen Wert bei jeder Iteration mit 10 multipliziert
 * und wird der wieder auf 0,0001234 gesetzt, nachdem 12340,0 angezeigt wurde.
 */

void task4() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;

	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);
	CPolledTimer showNext(1000);

	float value ;
	value = 0.0001234;


	while (true) {

		if(showNext.timeReached())
		{
			ssd.setValue(value);
			value = value * 10;
			if(value > 12340.0 )
			{
				value = 0.0001234;
			}
			//Aktualisierung der Wartezeit vom Timer
			showNext.updateWaitTime(1000);
		}

		if(digitTimer.timeReached())
		{
			ssd.activateNextDigit();

		}
	}
}// end of (task4)


/**
 * Implementierung der Funktion task5
 * task5 hat die gleiche Implementierung der task3,
 * aber sie hat noch eine kleine Änderungen.
 * das explizite Setzen der Werte der Anzeigestellen
 * und des Dezimalpunkts werden entfernet, so dass nur die Definition
 * von ssd, die Definition von digitTimer und die Endlosschleife
 * mit der Abfrage des Timers und der Aktualisierung von ssd übrig bleiben.
 * Ein Objekts poti vom Typ AnalogIn wird
 * mit einer Referenzspannung von 3,3 V definiert.
 * Zusätzlich wird ein Object CPolledTimer mit Namen
 * measurementTimer und einer Taktperiode von 20 ms definiert.
 * Nach Ablauf des Timers measurementTimer wird der Anzeige
 * die am Potentiometer gemessene Spannung zugewiesen.
 */
void task5() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;

	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);
	AnalogIn poti(BB_POTI);
	poti.set_reference_voltage(3.3);
	CPolledTimer measurementTimer(20);
	float potiValue;
	ADC1->HTR = 0xfff * 3 /4;
	ADC1->LTR = 0xfff / 4;
	ADC1->CR1 |= ADC_CR1_AWDEN;

	while (true) {

		ADC1->SR = 0;
		if(measurementTimer.timeReached())
		{
			potiValue = poti.read_voltage();
			ssd.setValue(potiValue);

			//Grenzwert-Überwachung
			if(ADC1->SR & ADC_SR_AWD)
			{
				led0.write(1);

			}

			else
			{
				led0.write(0);
			}
			measurementTimer.updateWaitTime(20);
		}

		if(digitTimer.timeReached())
		{

			ssd.activateNextDigit();

		}
	}
}// end of (task5)


/**
 * Implementierung der Funktion task6
 * task6 hat die gleiche Implementierung der task5,
 * aber sie hat noch eine kleine Änderungen.
 * Ein gleitender Mittelwertfilter wird mit Namen filter definiert.
 * In der Endlosschleife wird den gemessenen Wert nicht mehr
 * direkt der Anzeige zugewiesen sondern wird der an den Filter
 * übergeben und die Anzeige mit dem neuen gefilterten Wert aktualisiert.
 */
void task6() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;

	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);
	AnalogIn poti(BB_POTI);
	poti.set_reference_voltage(3.3);
	CPolledTimer measurementTimer(20);
	CMovingAverageFilter filter(300);
	float potiValue;
	ADC1->HTR = 0xfff * 3 /4;
	ADC1->LTR = 0xfff / 4;
	ADC1->CR1 |= ADC_CR1_AWDEN;

	while (true) {

		ADC1->SR = 0;
		if(measurementTimer.timeReached())
		{
			potiValue = poti.read_u16();
			filter << potiValue;
			ssd.setValue(filter);

			//Grenzwert-Überwachung
			if(ADC1->SR & ADC_SR_AWD)
			{
				led0.write(1);
			}

			else
			{
				led0.write(0);
			}
			measurementTimer.updateWaitTime(20);
		}

		if(digitTimer.timeReached())
		{

			ssd.activateNextDigit();

		}
	}
}// end of (task6)

/**
 * Implementierung der Funktion task7
 * task7 hat die gleiche Implementierung der task6,
 * aber sie hat noch eine kleine Änderungen.
 * der Typ des Filters wird zu CExponentialFilter geaendert.
 * der Messwert wird in Volt als Eingabewert für den Filter verwendet.
 *
 */
void task7() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;

	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer digitTimer(5);
	AnalogIn poti(BB_POTI);
	poti.set_reference_voltage(3.3);
	CPolledTimer measurementTimer(20);
	CExponentialFilter filter(0.35);
	float potiValue;
	ADC1->HTR = 0xfff * 3 /4;
	ADC1->LTR = 0xfff / 4;
	ADC1->CR1 |= ADC_CR1_AWDEN;

	while (true) {

		ADC1->SR = 0;
		if(measurementTimer.timeReached())
		{
			potiValue = poti.read_voltage();
			filter << potiValue;
			ssd.setValue(filter);

			//Grenzwert-Überwachung
			if(ADC1->SR & ADC_SR_AWD)
			{
				led0.write(1);
			}
			else
			{
				led0.write(0);
			}
			measurementTimer.updateWaitTime(20);
		}

		if(digitTimer.timeReached())
		{

			ssd.activateNextDigit();

		}
	}
}// end of (task7)



int main() {

	//Aufruf der Funktion mode zum keys-Objekt
	//um die Pull-Down Widerstände einzuschalten
	keys.mode(PullDown);

	//Aufruf der Funktion task1
	//task1();

	//Aufruf der Funktion task2
	//task2();

	//Aufruf der Funktion task3
	//task3();

	//Aufruf der Funktion task4
	//task4();

	//Aufruf der Funktion task5
	//task5();

	//Aufruf der Funktion task6
	//task6();

	//Aufruf der Funktion task7
	task7();


	while (true) {
	}
}

