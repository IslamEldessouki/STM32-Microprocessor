/*
 * CMovingAverageFilter.h
 *
 *  Created on: Dec 5, 2020
 *      Author: Islam Eldessouki
 */

#ifndef CMOVINGAVERAGEFILTER_H_
#define CMOVINGAVERAGEFILTER_H_

#include <stdint.h>

/**
 * Bildet der gleitende Mittelwert über die letzten N Messwerte.
 * Dazu werden N Messwerte gespeichert.
 * Jeder neue Messwert ersetzt den jeweils ältesten Messwert.
 * Bei der Abfrage des gefilterten Wertes wird die Summe aller
 * Werte gebildet und durch N geteilt.
 */
class CMovingAverageFilter{

private:
	uint16_t* m_values;
	uint16_t m_valuesSize = 16;
	uint16_t m_nextIndex = 0;
	uint32_t m_sum;

public:
	/**
	 * Erzeugt einen neuen Filter, der den gleitenden
	 * Mittelwert über N Werte bildet.
	 *
	 * @param size die Größe des Arrays, das die angegebenen Werte enthält.
	 */
	CMovingAverageFilter(uint16_t size);

	/**
	 * Gibt neue Werte in den Filter hinein.
	 *
	 * @param value der neue angegebene Wert.
	 * @return Referenz auf das Object CMovingAverageFilter.
	 */
	CMovingAverageFilter& operator<< (uint16_t value);

	/**
	 *  Liefert den gefilterten Wert.
	 */
	operator uint16_t () const;

};




#endif /* CMOVINGAVERAGEFILTER_H_ */
