/*
 * CMeasurementDevice.cpp
 *
 *  Created on: Dec 15, 2020
 *      Author: Islam Eldessouki
 */

#include "mbed.h"
#include "CSevenSegmentDisplay.h"
#include "CDebouncer.h"
#include "CPolledTimer.h"
#include "CHexShifter.h"
#include "CMeasuringDevice.h"

CMeasuringDevice::CMeasuringDevice(CSevenSegmentDisplay* display, BusIn* keys,
		BusOut* leds, BufferedSerial* usart, AnalogIn* voltageInput)
{
	m_keys = new CDebouncer(keys,150);
	keysPushed = 0x00;
	m_leds = leds;
	m_usart = usart;
	m_display = display;
	m_voltageInput = voltageInput;
	measurementValue = 0;

}

CMeasuringDevice::~CMeasuringDevice()
{
	delete m_keys;
}


void CMeasuringDevice::poll()
{
	pollKeys();
	pcKeys();
	pollSingleMeasurement();
	pollContinuousMeasurement();
	pollSegmentMode();
}

void CMeasuringDevice::pollKeys()
{
	uint8_t newKeysState = 0x00 ;
	newKeysState = m_keys->read();
	keysPushed = keysPushed ^ newKeysState;
	modeKeys = keysPushed & 0xF0;
	singleMeasurement = keysPushed & 0x08;
	continuousMeasurement = keysPushed & 0x07;
}

void CMeasuringDevice::pollSegmentMode()
{
	m_display->setValue(measurementValue);
	pollBinaryMode();
	pollBCDMode();
	pollBalkenMode();
	pollPCMode();
}

void CMeasuringDevice::pollBinaryMode()
{
	if(modeKeys == 0x20 || modeKeys == 0x00)
	{
		if(measurementValue == m_voltageInput->get_reference_voltage())
		{
			m_leds->write(0xff);
		}
		else{
			m_leds->write(measurementValue);
		}
	}
}

void CMeasuringDevice::pollBCDMode()
{
	if(modeKeys == 0x40)
	{
		uint8_t beforDecimalPoint = measurementValue * 10;
		uint8_t afterDecimalPoint = beforDecimalPoint % 10;
		beforDecimalPoint = beforDecimalPoint / 10;
		m_leds->write((beforDecimalPoint << 4) | afterDecimalPoint);
	}
}

void CMeasuringDevice::pollBalkenMode()
{
	if(modeKeys == 0x80)
	{
		float interval[10];

		for(int i = 0; i < 10; ++i)
		{
			interval[i] = ((m_voltageInput->get_reference_voltage()) / 9) * i;
		}

		for(int i = 0; i < 9; ++i)
		{
			if(measurementValue >= interval[i] &&
					measurementValue < interval[i+1] )
			{
				m_leds->write(0xff00 >> i);
			}
		}
	}
}

void CMeasuringDevice::pollPCMode()
{
	if(modeKeys == 0x10)
	{
		m_leds->write(0xff);
		char allData[] = {'0','1','2','3','4','5','6','7','8','9'};
		char data[6];
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 10; ++j)
			{
				if(m_display->set(i) == CSevenSegmentDisplay::patterns[j])
				{
					data[i] = allData[j];
					break;
				}
			}
		}

		for(int i = 4; i > 1; --i)
		{
			data[i] = data[i-1];
		}
		data[1] = '.';
		data[5] = 'V' ;
		m_usart->write(data, 6);
	}
}

void CMeasuringDevice::pollSingleMeasurement()
{
	if(singleMeasurement == 0x08)
	{
		keysPushed = keysPushed & 0xf8;
		measurementValue = m_voltageInput->read_voltage();
	}
}

void CMeasuringDevice::pollContinuousMeasurement()
{
	if(continuousMeasurement == 0x04 && singleMeasurement == 0x00)
	{
		float lastMeaurementValue = 0;
		float sum = 0;

		for(int i=0; i<10; ++i)
		{
			lastMeaurementValue = m_voltageInput->read_voltage();
			thread_sleep_for(10);
			pollKeys();
			if(continuousMeasurement == 0x06 && singleMeasurement == 0x00)
			{
				goto stop;
			}

			sum += lastMeaurementValue;
		}

		measurementValue = sum / 10;

		stop: measurementValue = lastMeaurementValue;

		if(continuousMeasurement == 0x05 && singleMeasurement == 0x00)

		{
			measurementValue = 0;
		}
	}
}

void CMeasuringDevice::pcKeys()
{
	char allData[] = {'b','c','i','p','e','f','h','s'};
	char data[1];
	m_usart->read(data, 1);

	for(int i=0; i < 8; ++i)
	{
		if(data[0] == allData[i])
		{
			keysPushed ^= (0x80 >> i);
			break;
		}
	}
}



