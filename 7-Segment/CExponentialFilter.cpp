/*
 * CExponentialFilter.cpp
 *
 *  Created on: Dec 6, 2020
 *      Author: Islam Eldessouki
 */

#include "CExponentialFilter.h"

CExponentialFilter::CExponentialFilter(float factor)
{
	m_factor = factor;
	m_last = 0;
}

CExponentialFilter& CExponentialFilter::operator<<(float value)
{
	m_last = m_last + ( m_factor * ( value - m_last));
	return *this;
}

CExponentialFilter::operator float () const
		{

	return m_last;

		}

