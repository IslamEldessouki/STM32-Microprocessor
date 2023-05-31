/*
 * CShiftRegisterOutputExtender.cpp
 *
 *  Created on: 17.10.2020
 *      Author: mnl
 */

#include "CShiftRegisterOutputExtender.h"

CShiftRegisterOutputExtender::CShiftRegisterOutputExtender(
		DigitalOut* serialData, DigitalOut* shiftRegisterClock,
		DigitalOut* bufferRegisterClock) {

	m_serialData = serialData;
	m_shiftRegisterClock = shiftRegisterClock;
	m_bufferRegisterClock = bufferRegisterClock;

}

void CShiftRegisterOutputExtender::prepareOutput(uint8_t value) {

	m_bufferRegisterClock->write(0);
	uint8_t i ;

	for(i = 0; i < 8; i++)
	{

		m_serialData->write(!!(value & (1 << (7 - i))));
		m_shiftRegisterClock->write(1);
		m_shiftRegisterClock->write(0);

	}

	m_preparedOutput = value;
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




