/*
 * CMovingAverageFilter.cpp
 *
 *  Created on: Dec 5, 2020
 *      Author: Islam Eldessouki
 */

#include "CMovingAverageFilter.h"

CMovingAverageFilter::CMovingAverageFilter(uint16_t size)
{
	m_valuesSize = size;
	m_values = new uint16_t[m_valuesSize];

	for(m_nextIndex = 0; m_nextIndex < m_valuesSize; ++m_nextIndex)
	{
		m_values[m_nextIndex] = 0;
	}


}

CMovingAverageFilter& CMovingAverageFilter::operator<< (uint16_t value)
{

	m_sum = 0;
	for(m_nextIndex = 0; m_nextIndex < m_valuesSize-1; ++m_nextIndex)
	{
		m_values[m_nextIndex] = m_values[m_nextIndex + 1];
	}

	m_values[m_nextIndex - 1] = value;

	for(m_nextIndex = 0; m_nextIndex < m_valuesSize; ++m_nextIndex)
	{
		m_sum = m_sum + m_values[m_nextIndex];
	}

	return *this;

}

CMovingAverageFilter::operator uint16_t ()const
										{


	uint16_t average = m_sum / m_valuesSize;

	return average;


										}





