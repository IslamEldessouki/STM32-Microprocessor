/*
 * CShiftRegisterOutputExtender.cpp
 *
 *  Created on: 17.10.2020
 *      Author: mnl
 */

#include "CShiftRegisterOutputExtender.h"
#include "GpioUtil.h"

CShiftRegisterOutputExtender::CShiftRegisterOutputExtender(
		DigitalOut* serialData, DigitalOut* shiftRegisterClock,
		DigitalOut* bufferRegisterClock, DigitalOut* led) {

	m_serialData = serialData;
	m_shiftRegisterClock = shiftRegisterClock;
	m_bufferRegisterClock = bufferRegisterClock;
	m_led = led;

}

/*void CShiftRegisterOutputExtender::prepareOutput(uint8_t value) {

 *m_led = 1;
	m_bufferRegisterClock->write(0);
	uint8_t i ;

	for(i = 0; i < 8; i++)
	{
		m_serialData->write(!!(value & (1 << (7 - i))));
		m_shiftRegisterClock->write(1);
		m_shiftRegisterClock->write(0);
	}

	m_preparedOutput = value;
 *m_led = 0;
}*/

void CShiftRegisterOutputExtender::doShift(GPIO_TypeDef* clockPort, uint32_t clockMask,
		GPIO_TypeDef* dataPort, uint32_t dataMask, uint8_t value) {
	m_bufferRegisterClock->write(0);
	for(int i = 0; i < 8; i++)
	{
		uint8_t temp = !!(value & (1 << (7 - i)));
		if (temp) {
			dataPort->BSRR = dataMask;
		} else {

			dataPort->BSRR = dataMask << 16;
		}

		if (1) {
			clockPort->BSRR = clockMask;
		} else {
			clockPort->BSRR = clockMask << 16;
		}

		if (0) {
			clockPort->BSRR = clockMask;
		} else {

			clockPort->BSRR = clockMask << 16;
		}
	}
}
void CShiftRegisterOutputExtender::prepareOutput(uint8_t value) {
	*m_led = 1;
	// Informationen für den direkten Zugriff auf die GPIO-Register holen.
	GPIO_TypeDef* clockPort;
	uint32_t clockMask;
	gpioPortInfo(*m_shiftRegisterClock, clockPort, clockMask);
	GPIO_TypeDef* dataPort;
	uint32_t dataMask;
	gpioPortInfo(*m_serialData, dataPort, dataMask);
	doShift(clockPort, clockMask, dataPort, dataMask, value);
	m_preparedOutput = value;
	*m_led = 0;
}


void CShiftRegisterOutputExtender::updateOutput() {

	m_bufferRegisterClock->write(1);
	m_currentOutput = m_preparedOutput;
}

void CShiftRegisterOutputExtender::write(uint8_t value) {

	prepareOutput(value);
	updateOutput();
}

CShiftRegisterOutputExtender& CShiftRegisterOutputExtender::operator =(
		uint8_t value) {

	write(value);

	return *this;
}

uint8_t CShiftRegisterOutputExtender::read() const {

	return m_currentOutput;
}

CShiftRegisterOutputExtender::operator int() const {

	return read();
}


