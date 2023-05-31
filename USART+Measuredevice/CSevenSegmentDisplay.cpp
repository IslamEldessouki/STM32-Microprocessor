/*
 * CSevenSegmentDisplay.cpp
 *
 *  Created on: 18.10.2020
 *      Author: mnl
 */

#include "CSevenSegmentDisplay.h"

// ToDo: Tragen Sie hier (statt der 0) die Werte aus dem von Ihnen
// in der Vorbereitung erstellten Array mit Segment-Mustern ein.
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


}

void CSevenSegmentDisplay::activateNextDigit() {

	for (int digit = 0; digit < 4; digit++) {

		m_activeDigit = digit;
		*m_digits[m_activeDigit] = 1;
		if(m_activeDigit == (~m_decimalDigit + 4 ) )
		{
			m_ssdSegments->prepareOutput(~(0x20));
			m_ssdSegments->updateOutput();
			thread_sleep_for(1);
		}
		m_ssdSegments->prepareOutput(~m_values[m_activeDigit]);
		m_ssdSegments->updateOutput();
		thread_sleep_for(5);
		*m_digits[m_activeDigit] = 0;


	}


}

uint8_t& CSevenSegmentDisplay::set(int position) {
	static uint8_t dummy = 0;
	if (position < 0 || position > 3) {
		return dummy;
	}
	return m_values[position];
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


