#include "mbed.h"
#include "breadboard.h"
#include "CShiftRegisterOutputExtender.h"
#include "CSevenSegmentDisplay.h"
#include "CRtc.h"
#include "CPolledTimer.h"

// Legen Sie die globalen Objekte, die die Peripheriekomponenten
// repräsentieren, hier an (vor den Funktionen).

DigitalOut led0(BB_LED_0);

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
 * Die Implementierung der Funktion task1, die die 7-Segment-Anzeige ansteuert.
 * Die 7-Segment-Anzeige zeigt die letzten Stellen meiner Matrikelnummer
 * an. Die im letzten Labor entwickelte, Interrupt-gesteuerte Version
 * von CSevenSegmentDisplay wird verwendet. Die Helligkeit der Anzeige wird
 * bei 90% eingestellt.
 */
void task1() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock,
			&ssdBufferRegisterClock, &led0 );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);

	//die letzten vier Stellen meiner Matrikelnummer(5,1,5,9)
	ssd[0] = CSevenSegmentDisplay::patterns[5];
	ssd[1] = CSevenSegmentDisplay::patterns[1];
	ssd[2] = CSevenSegmentDisplay::patterns[5];
	ssd[3] = CSevenSegmentDisplay::patterns[9];
	ssd.setDecimalPoint(0);
	ssd.setBrightness(90);

	while (true) {

	}
}

/**
 * Die Implementierung der Funktion task2, die im Super-Loop die von der
 * Echtzeituhr gelieferte Zeit als Stunden und Minuten im 24-Stunden-Format
 * auf der 7-Segment-Anzeige anzeigt. Das Einstellen der Uhrzeit soll
 * über die vier rechten Taster möglich sein. So lange der Taster ganz
 * rechts gedrückt ist, wird die Einerstelle der Minutenanzeige im
 * Sekundentakt erhöht. Nur die Einerstelle wird erhöht, es erfolgt kein
 * Übertrag. Das Drücken des zweiten Tasters von rechts bewirkt entsprechend
 * eine Erhöhung der Zehnerstelle der Minutenanzeige. Das Drücken des dritten
 * Tasters von rechts bewirkt entsprechend ein Erhöhen der Einerstelle
 * der Stundenanzeige und das Drücken des vierten Tasters von rechts ein
 * Erhöhen der Zehnerstelle der Stundenanzeige.
 */
void task2() {

	DigitalOut* digits [4] = {(&ssdDigit0),(&ssdDigit1),(&ssdDigit2),(&ssdDigit3)} ;
	CShiftRegisterOutputExtender ssdSegments(&ssdSerialData,&ssdShiftRegisterClock,
			&ssdBufferRegisterClock, &led0 );
	CSevenSegmentDisplay ssd(&ssdSegments, digits);
	CRtc realTimeClock;
	realTimeClock.updateTime(59700);
	CPolledTimer activeTimer(1000);
	int mins_one;
	int mins_ten;
	int hours_one;
	int hours_ten;
	ssd.setDecimalPoint(0);
	ssd.setBrightness(90);
	while (true) {
		mins_one = (realTimeClock.secondsSinceMidnight() / 60 % 60) % 10;
		mins_ten = (realTimeClock.secondsSinceMidnight() / 60 % 60) / 10;
		hours_one = (realTimeClock.secondsSinceMidnight() / 3600 % 24) % 10;
		hours_ten = (realTimeClock.secondsSinceMidnight() / 3600 % 24) / 10;
		ssd = (hours_ten * 1000) + (hours_one * 100) + (mins_ten * 10) + mins_one;
		// Erhöhen der Einerstelle der Minutenanzeige.
		if(keys == 0x01)
		{
			if(activeTimer.timeReached())
			{
				if(mins_one == 9)
				{
					realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() - (9*60));
				}else
				{
					realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() + 60);
				}
			}
		}
		// Erhöhen der der Zehnerstelle der Minutenanzeige.
		else if(keys == 0x02)
		{
			if(activeTimer.timeReached())
			{
				if(mins_ten == 5)
				{
					realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() - (5*600));
				}else
				{
					realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() + 600);
				}
			}
		}
		// Erhöhen der Einerstelle der Stundenanzeige.
		else if(keys == 0x04)
		{
			if(activeTimer.timeReached())
			{
				if(hours_ten >= 0 && hours_ten <= 1)
				{
					if(hours_one == 9)
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() - (9*3600));
					}else
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() + 3600);
					}
				}else
				{
					if(hours_one == 4)
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() - (4*3600));
					}else
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() + 3600);
					}
				}
			}
		}
		// Erhöhen der Zehnerstelle der Stundenanzeige.
		else if(keys == 0x08)
		{
			if(activeTimer.timeReached())
			{
				if(hours_one >= 0 && hours_one <= 4)
				{
					if(hours_ten == 2)
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() - (2*36000));
					}else
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() + 36000);
					}
				}else
				{
					if(hours_ten == 1)
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() - 36000);
					}else
					{
						realTimeClock.updateTime(realTimeClock.secondsSinceMidnight() + 36000);
					}
				}
			}
		}
	}
}



int main() {

	//Aufruf der Funktion mode zum keys-Objekt
	//um die Pull-Down Widerstände einzuschalten
	keys.mode(PullDown);

	// Aufruf der Funktion, die die Lösung der gerade bearbeiteten Aufgabe
	// implementiert.
	//task1();

	//Aufruf der Funktion task2
	task2();


	while (true) {
	}
}

