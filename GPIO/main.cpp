/**
 * Author: Islam Eldessouki
 */

#include "mbed.h"
#include "stdint.h"
#include"CDebouncer.h"
#include "breadboard.h"
#include "CPolledTimer.h"

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



/**
 * Implementierung der Funktion task1
 *  alle 8 LEDs(0..7) alle 100 ms ein- und wieder ausschaltet werden
 *  die Funktion write der Klasse-BusOut gibt den 8 LEDs(0..7) die Werte aus
 *  das Hexadecimal number system wird als LEDs-Werte benutzt
 */
void task1() {

	while (true) {

		leds.write(0xFF);
		thread_sleep_for(100);
		leds.write(0x00);
		thread_sleep_for(100);

	}
}// end of (task1)

/**
 * Implementierung der Funktion task2
 * die Funktion write gibt den 8 LEDs den gleichen Wert der 8 Taster
 * d.h. eine LED leuchtet, solange der Taster unterhalb der LED gedrückt ist
 */
void task2() {

	while (true) {

		leds.write(keys);

	}
}// end of (task2)

/**
 * Implementierung der Funktion task3
 * Durch diese Funktion schaltet  ein Drücken des Tasters unterhalb einer LED diese LED um
 * d.h. LED ausschaltet, wenn sie eingeschaltet ist,
 * und einschaltet, wenn sie ausgeschaltet ist.
 *
 * previousKeysState speichert alten Zustand der Taster.
 * newKeysState speichert aktuellen Zustand der Taster.
 * keysPushed berechnet neu gedrueckten Tasten, 1 bedeutet, Taste wurde seit letzter
 * Iteration gedrueckt.
 */
void task3(){

	uint8_t previousKeysState = 0x00 ;
	uint8_t newKeysState = 0x00 ;
	uint8_t keysPushed = 0x00;

	while (true) {

		previousKeysState = leds.read();
		newKeysState = keys.read();
		keysPushed = previousKeysState ^ newKeysState;
		leds.write(keysPushed);

	}

}// end of (task3)


/**
 * Implementierung der Funktion task4
 * task4 hat das gleiche Verhalten der task3(),
 * aber Sie hat noch eine Eigenschaft (Entprellung)
 * d.h. man nach einer Änderung des Eingangswertes eine bestimmte Zeit
 * (Entprellzeit) wartet, bevor der Eingang erneut abgefragt wird.
 * man kann dafür ein Objekt vom Typ Dekorierer-Klasse CDebouncer benutzen,
 * die Adresse der Keys und Wartezeit als Parameter nimmt.
 * die read-Methode und der überladene operator int () liefern
 * die gleichen Werte wie die Methode der Klasse BusIn, aber werden die Werte entprellt
 */
void task4() {

	uint8_t previousKeysState = 0x00 ;
	uint8_t newKeysState = 0x00 ;
	uint8_t keysPushed = 0x00;
	CDebouncer keysDebouncer(&keys,150);


	while (true) {

		previousKeysState = leds.read();
		newKeysState = keysDebouncer.read();
		//newKeysState = int(keysDebouncer);
		keysPushed = previousKeysState ^ newKeysState;
		leds.write(keysPushed);

	}


}// end of (task4)

/**
 * Implementierung der Funktion task5
 * die task5() wird Implementiert, damit die LED 0 leuchtet,
 * dann nach einer Sekunde nur die LED1, nach einer weiteren Sekunde die LED2 usw.
 * dann Nach der LED7 soll wieder die LED0 leuchten.
 * Bitmuster speichert den Zustand der LEDs
 * das Licht laeuft in der LEDs, indem man den Wert der LEDs um 1 Bit verschiebt.
 * man kann dafür den Schiebeoperator (<<) benutzen.
 */
void task5() {
	uint8_t Bitmuster = 0x01;

	while (true) {


		leds.write(Bitmuster);
		thread_sleep_for(1000);

		Bitmuster = Bitmuster << 1;
		if(Bitmuster == 0x00)
		{
			Bitmuster = 0x01;
		}



	}
}// end of (task5)

/**
 * Implementierung der Funktion task6
 * task6() implementiert das gleiche Verhalten der task5(),
 * aber sie hat noch einen Vorteil
 * die Wartezeit kann ausgenutzt werden, indem man die vergangene Zeit messen kann
 * und dann wird die als Nebentätigkeit benutzt.
 *
 * waitTime speichert die gewuenchte Wartezeit.
 * actualTime speichert die aktuelle Zeit.
 * targetTime speichert den Zielzeitpunkt.
 */
void task6() {
	uint8_t Bitmuster = 0x01;
	int64_t waitTime = 1000;

	int64_t targetTime = std::chrono::duration_cast<std::chrono::milliseconds>(HighResClock::now()
	.time_since_epoch()).count() + waitTime;


	while (true) {
		leds.write(Bitmuster);

		int64_t actualTime = std::chrono::duration_cast<std::chrono::milliseconds>(HighResClock::now()
		.time_since_epoch()).count();

		if (actualTime >= targetTime){

			Bitmuster = Bitmuster << 1;
			if (Bitmuster == 0x00) {
				Bitmuster = 0x01;
			}
			else
			{
				leds.write(Bitmuster);
			}

			targetTime = actualTime + waitTime;
		}
	}
}// end of (task6)

/**
 * Implementierung der Funktion task7
 * task7() implementiert das gleiche Verhalten der task6(),
 * aber sie hat noch einen Vorteil.
 * Sie kann mit mehreren Wartezeiten funktionieren.
 * Man kann das realisieren, indem man die Klasse CPolledTimer benutzt.
 * Die Klasse CPolledTimer hat die benötigten Variablen
 * targetTime und waitTime als Attributen.
 * Und die Timer-Funktionalität wird durch die  Methode timeReached() realisiert.
 */
void task7() {
	CPolledTimer twoHz(500);
	CPolledTimer threeHz(333);
	CPolledTimer fourHz(250);
	while (true) {
		if (twoHz.timeReached()) {
			leds = leds ^ (1 << 0);
		}
		if (threeHz.timeReached()) {
			leds = leds ^ (1 << 1);
		}
		if (fourHz.timeReached()) {
			leds = leds ^ (1 << 2);
		}
	}
}// end of (task7)

/**
 * Implementierung der Funktion task8
 *  task8() implementiert das gleiche Verhalten der Funktion task6(),
 *  aber statt der Implementierung des Timer-Funktionalität als Teil
 *  der main-Funktion ein Objekt chasingLightTimer vom Typ CPolledTimer verwendet.
 *  das Objekt chasingLightTimer nimmt eine Wartezeit von 1000 milli Sekunde.
 */
void task8() {
	uint8_t Bitmuster = 0x01;
	CPolledTimer chasingLightTimer(1000);

	while (true) {

		leds.write(Bitmuster);


		if(chasingLightTimer.timeReached())
		{

			Bitmuster = Bitmuster << 1;
			if(Bitmuster == 0x00)
			{
				Bitmuster = 0x01;
			}
			else
			{
				leds.write(Bitmuster);
			}
			chasingLightTimer = CPolledTimer(1000);
		}

	}
}// end of (task8)


/**
 * Implementierung der Funktion task9
 * diese Funktion realisiert die Anwendung „konfigurierbares Lauflicht“ realisiert.
 * das Verhalten eines Lauflichts kann über den Wert einer Variablen gesteuert werden.
 * diese Variable kann die Werte -7 bis +7 annehmmen.
 * ein negativer Wert bedeutet, dass das Lauflicht von rechts nach links läuft,
 * und ein positiver Wert bedeutet, dass Lauflicht von links nach rechts läuft.
 * der absolute Wert der Variablen bestimmt die Zeit zwischen
 * zwei Zustandswechseln nach der Formel „absoluter Wert * 250 ms“
 * und damit bestimmt die Geschwindigkeit des Lauflichts.

 * die LED-Gruppe (Lauflicht) stellt das Lauflicht dar.
 * Die Gruppe (Geschwindigkeit/Richung) stellt den Wert der Steuervariablen
 * als vorzeichenbehaftete 4-bit Binärzahl (Zweierkomplement) dar.
 * Mit den Tastern der Gruppe (Direkteinstellung) können
 * die darüber liegenden LEDs getoggelt werden.
 *  Die Taste (+) erhöht den Wert der Steuervariablen um 1,
 *  und die Taste (-) vermindert den um 1.
 * wenn die Variable bereits den Minimal- oder Maximalwert hat,
 * dann hat ein weiterer Tastendruck keine Wirkung.
 *  Die Taste (↔) wechselt die Laufrichtung, ohne die Geschwindigkeit zu verändern.
 *
 */
void task9()
{
	CPolledTimer chasingLightTimer(250);
	CDebouncer keysDebouncer(&keys,150);
	uint8_t previousKeysState = keysDebouncer.read();
	uint8_t newKeysState = 0X00;
	uint8_t directChange = 0x00;

	int8_t controlValue = 0x01;
	uint8_t movingLights = 0x80;
	uint8_t controlLights = 0x00;

	leds = movingLights | controlLights;

	while(true)
	{
		//Lauflicht-Code-Teil
		if(chasingLightTimer.timeReached())
		{
			// wenn der Wert der SteuerVariable ungleich Null, läuft das Licht
			if(controlValue !=0)
			{
				controlLights = controlValue & 0x0F;

				// wenn der Wert der SteuerVariable > Null, läuft das Licht von links nach rechts
				if(controlValue > 0)
				{
					if(movingLights == 0x10)
					{
						movingLights = 0x80;
					}
					else{
						movingLights = movingLights >> 1;
					}
				}

				// wenn der Wert der SteuerVariable < Null, läuft das Licht von rechts nach links
				else{
					if(movingLights == 0x80)
					{
						movingLights = 0x10;
					}
					else{
						movingLights = movingLights << 1 ;
					}
				}

				leds =movingLights | controlLights;
				chasingLightTimer.updateWaitTime(250*abs(controlValue));
			}

			// wenn der Wert der SteuerVariable gleich Null, läuft kein Licht
			else{
				leds = 0;
			}
		}

		//Direkeinstellung-Code-Teil
		newKeysState = keysDebouncer.read();
		directChange = controlValue & 0x0F;
		directChange = (directChange ^ (~previousKeysState & newKeysState)) & 0x0F ;

		if(directChange > 7)
		{
			controlValue = directChange ^ 0xF0;

		}
		else{
			controlValue = directChange & 0x0F;
		}

		thread_sleep_for(200);
		previousKeysState = newKeysState;

		// Inkrementierung der Direkeinstellung
		if(keysDebouncer.read() == 0x10)
		{
			if(controlValue < 7)
			{
				controlValue ++;
				thread_sleep_for(200);
			}
		}

		// Dekrementierung der Direkeinstellung
		if(keysDebouncer.read() == 0x20)
		{
			if(controlValue > -7)
			{
				controlValue --;
				thread_sleep_for(200);
			}
		}

		// Negierung der Direkeinstellung
		if(keysDebouncer.read() == 0x40)
		{
			controlValue = (-1 * controlValue);
			thread_sleep_for(200);
		}
	}


}// end of (task9)



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
	//task7();

	//Aufruf der Funktion task8
	//task8();

	//Aufruf der Funktion task9
	task9();

	while (true) {


	}
}// end of (main)

