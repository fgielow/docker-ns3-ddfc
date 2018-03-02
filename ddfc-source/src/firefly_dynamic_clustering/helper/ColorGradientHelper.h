/*
 * TemperatureHelper.h
 *
 *  Created on: 11/07/2012
 *      Author: fernandogielow
 */

#ifndef TEMPERATUREHELPER_H_
#define TEMPERATUREHELPER_H_

#include "ns3/ColorHandler.h"
//#include <iostream>
//#include <stdint.h>

//#include <iostream>

#define MAX_NUMER_OF_TONES 10

namespace ns3 {

class ColorGradientHelper {
public:
	ColorGradientHelper();
	ColorGradientHelper(double,double);
	ColorGradientHelper(double,double, int, ColorHandler args []);
	ColorHandler getColorOfReading(double);

private:
	int numberOfTones;
	ColorHandler temperatureTones[MAX_NUMER_OF_TONES];
	double minTemperature, maxTemperature;
};


} /* namespace ns3 */

#endif /* TEMPERATUREHELPER_H_ */
