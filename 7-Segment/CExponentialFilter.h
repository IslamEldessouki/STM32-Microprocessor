/*
 * CExponentialFilter.h
 *
 *  Created on: Dec 6, 2020
 *      Author: Islam Eldessouki
 */

#ifndef CEXPONENTIALFILTER_H_
#define CEXPONENTIALFILTER_H_

#include <stdint.h>

/**
 * Der neue geglättete Wert ergibt sich aus dem alten
 * geglätteten Wert plus der Differenz aus neuem und altem
 * Wert gewichtet mit dem Faktor ɑ.
 * Je größer der Faktor ist, desto mehr Einfluss hat der
 * neue Wert auf den geglätteten Wert.
 */
class CExponentialFilter

{
private:
	float m_factor;
	float m_last;

public:
	/**
	 * Erzeugt einen neuen Filter mit dem angegebenen Wert für den Faktor (ɑ).
	 *
	 * @param factor der Faktor der exponentielle Glättung
	 */
	CExponentialFilter(float factor);

	/**
	 * Gibt neue Werte in den Filter hinein.
	 *
	 * @param value der neue angegebene Wert.
	 * @return Referenz auf das Object CMovingAverageFilter.
	 */
	CExponentialFilter& operator<<(float value);

	/**
	 *  Liefert den gefilterten Wert.
	 */
	operator float () const;
};






#endif /* CEXPONENTIALFILTER_H_ */
