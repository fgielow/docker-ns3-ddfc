/*
 * ColorHandler.h
 *
 *  Created on: 10/07/2012
 *      Author: fernandogielow
 */

#ifndef COLORHANDLER_H_
#define COLORHANDLER_H_

#include <iostream>
#include <stdint.h>

namespace ns3 {

static const std::string hexDigits = "0123456789ABCDEF";

class ColorHandler {

private:
	std::string ConvertRGBtoHex(int);
	uint8_t red, green, blue;

public:
	uint8_t getRed();
	uint8_t getGreen();
	uint8_t getBlue();

//	ColorHandler();
	ColorHandler(uint8_t r=0,uint8_t g=0,uint8_t b=0);

	std::string getColorHex();
	void randomizeColor();

	void lerp(ColorHandler, double);
	ColorHandler lerpNew(ColorHandler, double);
};

}

#endif /* COLORHANDLER_H_ */
