/*
 * TemperatureHelper.cc
 *
 *  Created on: 11/07/2012
 *      Author: fernandogielow
 */

#include "ColorGradientHelper.h"

namespace ns3 {

ColorGradientHelper::ColorGradientHelper() {
}

ColorGradientHelper::ColorGradientHelper(double minT,double maxT): numberOfTones(0) {
	std::cout << "initialized with ----" << 0 << "---- colors \n";
	minTemperature = minT;
	maxTemperature = maxT;
}

ColorGradientHelper::ColorGradientHelper(double minT, double maxT, int numberOfArgs, ColorHandler args []){

	minTemperature = minT;
	maxTemperature = maxT;

	numberOfTones = numberOfArgs;

	for (int i=0; i< numberOfArgs; i++) {
		temperatureTones[i] = args[i];
	}

	std::cout << "initialized with " << numberOfTones << " colors \n";
	std::cout << maxT << "," << maxTemperature << "\n";
}

ColorHandler ColorGradientHelper::getColorOfReading(double reading) {
//	std::cout << numberOfTones << " tones " << minTemperature << " min " << maxTemperature << " max\n";
    double interval = (maxTemperature - minTemperature)/(numberOfTones-1);
    reading-=minTemperature;

    for (int i=1; i<numberOfTones; i++) {
//    	std::cout << reading << " < " << interval*i << "?\n";
    	if (reading < interval*i) {
    		return temperatureTones[i-1].lerpNew(temperatureTones[i], ((reading-(i-1)*interval))/interval);
    	}
    }

    return temperatureTones[numberOfTones-1];

}


} /* namespace ns3 */
