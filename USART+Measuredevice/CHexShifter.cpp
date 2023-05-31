/*
 * CHexShifter.cpp
 *
 *  Created on: Dec 12, 2020
 *      Author: Islam Eldessouki
 */
#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"
#include "CHexShifter.h"

CHexShifter::CHexShifter(CSevenSegmentDisplay* display, BusIn* keys,
		BusOut* leds, BufferedSerial* usart)
{
	m_keys = new CDebouncer(keys,150);
	m_leds = leds;
	m_usart = usart;
	m_display = display;
	m_display->setValue(0);
	m_insertLeft = true;

}

CHexShifter::~CHexShifter()
{
	delete m_keys;
}

void CHexShifter::poll()
{
	pollUsart();
	pollKeys();
}

void CHexShifter::pollUsart()
{
	char data[1];

	if(m_usart->readable())
	{
		m_usart->read(data, 1);
		m_usart->write(data, 1);
		processChar(data[0]);
	}
}

void CHexShifter::processChar(char c)
{
	char data[] = {'0','1','2','3','4','5','6','7','8','9',
			'A','B','C','D','E','F','a','b','c','d','e','f'};
	int i = 0;
	do
	{
		if(data[i] == c)
		{
			break;
		}
		++i;
		if(i == 22)
		{
			i = -1;
		}
	}while(i != -1);

	if(i == -1)
	{
		processDigit(0);
	}
	else if(i >= 0 && i <= 15)
	{
		processDigit(i);
	}
	else if(i > 15 && i <= 21)
	{
		i = i - 6;
		processDigit(i);
	}
}

void CHexShifter::processDigit(uint8_t value)
{

	if(m_insertLeft)
	{
		for(int i = 3; i > 0; --i)
		{
			m_display->set(i) = m_display->set(i-1);
		}
		m_display->set(0) = CSevenSegmentDisplay::patterns[value];
	}

	else if(!m_insertLeft)
	{
		for(int i = 0; i < 3; ++i)
		{
			m_display->set(i)  = m_display->set(i+1);
		}
		m_display->set(3) = CSevenSegmentDisplay::patterns[value];
	}

	sendDisplayedDigits();
}

void CHexShifter::sendDisplayedDigits() const
{
	char allData[] = {'0','1','2','3','4','5','6','7','8','9',
				'A','b','C','d','E','F'};
	char data[4];
	for(int i = 0; i < 4; ++i)
		{
		for(int j = 0; j < 16; ++j)
			{
				if(m_display->set(i) == CSevenSegmentDisplay::patterns[j])
				{
					data[i] = allData[j];
				}
			}
		}
	m_usart->read(data, 4);
	m_usart->write(data, 4);
}

void CHexShifter::pollKeys()
{
	uint8_t previousKeysState = 0x00 ;
	uint8_t newKeysState = 0x00 ;
	uint8_t keysPushed = 0x00;
	previousKeysState = m_leds->read();
	newKeysState = m_keys->read();
	keysPushed = previousKeysState ^ newKeysState;
	keysPushed = keysPushed & 0x01;
	m_leds->write(keysPushed);

	if(keysPushed == 0x01)
	{
		m_insertLeft = false;
	}

	else
	{
		m_insertLeft = true;
	}
}




