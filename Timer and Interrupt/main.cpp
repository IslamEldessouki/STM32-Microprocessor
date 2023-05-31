/*
 * Islam Eldessouki
 */

#include "mbed.h"
#include "main.h"
#include "breadboard.h"
#include "CPwmGenerator1.h"
#include "CPwmGenerator2.h"
#include "CExponentialFilter.h"
#include "CPwmGenerator3.h"
#include "CPwmGenerator4.h"
#include "CShiftRegisterOutputExtender.h"
#include "CSevenSegmentDisplay.h"
#include "CPolledTimer.h"


BusIn keys(BB_BTN_0, BB_BTN_1, BB_BTN_2, BB_BTN_3,
		BB_BTN_4, BB_BTN_5, BB_BTN_6, BB_BTN_7);

AnalogIn poti(BB_POTI, 3.3);

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
 * Realisiert mit CPwmGenerator1 auf PC6 ein PWM-Signal. Zusätzlich
 * werden die 8 höchstwertigen Bits des Zählerstands auf den LEDs
 * ausgegeben.
 */
void task1() {
	// Die LEDs werden flexibler verwendet als in den vorherigen Laboren
	// und daher nicht als globales Objekt definiert sondern nach Bedarf
	// in den verschiedenen Tasks.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3);

	CPwmGenerator1 fixedGenerator;

	while (true) {
		// Bits 12-8 von CNT auf LEDs 3-0 anzeigen.
		leds = (TIM3->CNT >> 8) & 0xf;
	}
}

/**
 * Implementierung der Funktion task2. In der Funktion wird
 * vor der while-Schleife ein Objekt vom Typ CPwmGenerator2 erzeugt.
 * In der while-Schleife wird wie bei task1 der Wert der Bits
 * 12 bis 8 des Zählregisters von Timer 3 auf die LEDs 3 bis 0 kopiert.
 * Zusätzlich wird der Wert des ADC gelesen, wird der mit dem exponentiellen
 * Glättungsfilter undverwenden den gefilterten Wert geglaettet,
 * um den Tastgrad zwischen 0% und 100% einzustellen.
 */
void task2() {
	// Die LEDs werden flexibler verwendet als in den vorherigen Laboren
	// und daher nicht als globales Objekt definiert sondern nach Bedarf
	// in den verschiedenen Tasks.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3);

	CPwmGenerator2 fixedGenerator;
	AnalogIn poti(BB_POTI);
	poti.set_reference_voltage(3.3);
	CExponentialFilter filter(0.35);
	uint8_t filterPercent;

	while (true) {
		filter << poti.read_voltage();
		filterPercent = (filter / 3.3) * 100;
		fixedGenerator.setDutyCycle(filterPercent);

		// Bits 12-8 von CNT auf LEDs 3-0 anzeigen.
		leds = (TIM3->CNT >> 8) & 0xf;
	}
}

/**
 * Implementierung der Funktion task3. In der Funktion wird vor
 * der while-Schleife ein Objekt vom Typ CPwmGenerator3 mit einer
 * Periodendauer von 20 ms erzeugt. In der while-Schleife wird der
 * Wert des ADC gelesen, wird der mit Ihrem exponentiellen Glättungsfilter
 * gefiltert und wird das Ergebnis verwendet, damit den Tastgrad zwischen
 * 0% und 100% eingestellt wird.
 */
void task3() {
	// Die LEDs werden flexibler verwendet als in den vorherigen Laboren
	// und daher nicht als globales Objekt definiert sondern nach Bedarf
	// in den verschiedenen Tasks.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3);

	CPwmGenerator3 fixedGenerator(20000 - 1);
	AnalogIn poti(BB_POTI);
	poti.set_reference_voltage(3.3);
	CExponentialFilter filter(0.35);
	uint8_t filterPercent;

	while (true) {
		filter << poti.read_voltage();
		filterPercent = (filter / 3.3) * 100;
		fixedGenerator.setDutyCycle(filterPercent);

		// Bits 12-8 von CNT auf LEDs 3-0 anzeigen.
		leds = (TIM3->CNT >> 8) & 0xf;
	}
}

/**
 * Implementierung der Funktion task4. task4 ist eine Kopie von task3,
 * aber mit einem kleinen Unterschied. CPwmGenerator4 wird statt CPwmGenerator3
 * verwendet. Vor der while-Schleife wird ein Objekt vom Typ DigitalOut erzeugt,
 * das die LED 7 ansteuert und das bei dem Generator als
 * anzusteuernden Ausgang gesetzt wird.
 */
void task4() {
	// Die LEDs werden flexibler verwendet als in den vorherigen Laboren
	// und daher nicht als globales Objekt definiert sondern nach Bedarf
	// in den verschiedenen Tasks.
	BusOut leds(BB_LED_0, BB_LED_1, BB_LED_2, BB_LED_3);
	DigitalOut led7(BB_LED_7);

	CPwmGenerator4 fixedGenerator(20000 - 1);
	AnalogIn poti(BB_POTI);
	poti.set_reference_voltage(3.3);
	CExponentialFilter filter(0.35);
	uint8_t filterPercent;

	while (true) {
		filter << poti.read_voltage();
		filterPercent = (filter / 3.3) * 100;
		fixedGenerator.setDutyCycle(filterPercent);
		fixedGenerator.setPwmOut(&led7);

		// Bits 12-8 von CNT auf LEDs 3-0 anzeigen.
		leds = (leds & 0xf0) | ((TIM3->CNT >> 8) & 0xf);// mit Verbesserung
	}
}

/**
 * Implementierung der Funktion task5. die ein Objekt vom Typ
 * CSevenSegmentDisplay erzeugt, als anzuzeigende Ziffernfolge
 * die letzten vier Stellen Ihrer Matrikelnummer einstellt
 * und dann nichts mehr macht
 */
void task5() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock,
			&ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	//die letzten vier Stellen meiner Matrikelnummer(5,1,5,9)
	ssd[0] = CSevenSegmentDisplay::patterns[5];
	ssd[1] = CSevenSegmentDisplay::patterns[1];
	ssd[2] = CSevenSegmentDisplay::patterns[5];
	ssd[3] = CSevenSegmentDisplay::patterns[9];
	ssd.setDecimalPoint(0);
	ssd.setBrightness(100);

	DigitalOut led0(BB_LED_0);
	while (true) {
		// Ausführung von Interrupt Handlern verhindern und LED ausschalten.
		__disable_irq();
		led0 = 0;
		// Auf einen Interrupt warten. Wenn er auftritt wird die CPU wieder
		// aktiviert, der Interrupt Handler wird aber noch nicht ausgeführt.
		// Der Interrupt "steht aus" ("is pending").
		__WFI();
		// LED einschalten und die Ausführung von Interrupt Handlern wieder
		// zulassen. Der Handler wird damit sofort nach __enable_irq()
		// aufgerufen.
		led0 = 1;
		__enable_irq();
	}



}// end of (task5)

/**
 * Implementierung der Funktion task5. task6 ist eine Kopie von task5,
 * aber mit einem kleinen Unterschied. In dieser Funktion soll
 * für die Einstellung der Helligkeit eine Ein-Knopf-Steuerung
 * realisiert werden. Hat die Anzeige maximale Helligkeit und wird
 * der Taster ganz links gedrückt, reduziert das Programm
 * die Helligkeit alle 50 ms um 1% solange der Knopf gedrückt
 * bleibt und das Minimum noch nicht erreicht ist. Hat die Anzeige
 * nicht die maximale Helligkeit und wird der Knopf ganz links gedrückt,
 * erhöht das Programm die Helligkeit alle 50 ms um 1% solange der
 * Knopf gedrückt bleibt und das Maximum noch nicht erreicht ist.
 */
void task6() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock, &ssdBufferRegisterClock );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CPolledTimer activeTimer(50);
	uint8_t percent = 50;

	//die letzten vier Stellen meiner Matrikelnummer(5,1,5,9)
	ssd[0] = CSevenSegmentDisplay::patterns[5];
	ssd[1] = CSevenSegmentDisplay::patterns[1];
	ssd[2] = CSevenSegmentDisplay::patterns[5];
	ssd[3] = CSevenSegmentDisplay::patterns[9];
	ssd.setDecimalPoint(0);
	ssd.setBrightness(percent);
	DigitalOut led7(BB_LED_7);
	while (true) {
		__disable_irq();
		__WFI();
		if(percent == 100)
		{
			while(keys == 0x80)
			{
				if(activeTimer.timeReached())
				{
					percent = percent - 1;
					ssd.setBrightness(percent);
					if(percent == 0)
					{
						break;
					}
				}
			}
		}
		else
		{
			while(keys == 0x80)
			{
				if(activeTimer.timeReached())
				{
					percent = percent + 1;
					ssd.setBrightness(percent);
					if(percent == 100)
					{
						break;
					}
				}
			}
		}
		led7 = keys & 0x80;
		__enable_irq();
	}
}// end of (task6)

/**
 * Funktion, die der Timer 3 IRQ Handler aufrufen soll. Ermöglicht
 * die Konfiguration unterschiedlicher effektiver IRQ Handler je
 * nach Nutzung des Timers.
 *
 * Die aufzurufende Funktion muss im Rahmen der Konfiguration des
 * Interrupts zugewiesen werden.
 */
function<void()> activeTIM3_IRQHandler;

/**
 * Interrupt-Handler für Timer 3. Der Handler ruft die Funktion auf,
 * die der globalen Variablen activeTIM3_IRQHandler zugewiesen wurde.
 */
extern "C" void TIM3_IRQHandler() {
	activeTIM3_IRQHandler();
}






int main() {

	keys.mode(PullDown);

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
	//task1();

	//Aufruf der Funktion task2
	//task2();

	//Aufruf der Funktion task3
	//task3();

	//Aufruf der Funktion task4
	//task4();

	//Aufruf der Funktion task5
	task5();

	//Aufruf der Funktion task6
	//task6();

	while (true) {
	}
}

