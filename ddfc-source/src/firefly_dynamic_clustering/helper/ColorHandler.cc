/*
 * ColorHandler.cc
 *
 *  Created on: 10/07/2012
 *      Author: fernandogielow
 */

#include "ColorHandler.h"
#include "ns3/random-variable.h"

namespace ns3 {


//ColorHandler::ColorHandler(): red(0), green(0), blue(0) {
//}


ColorHandler::ColorHandler(uint8_t r,uint8_t g,uint8_t b) {
	red = r;
	green = g;
	blue = b;
}

std::string ColorHandler::ConvertRGBtoHex(int num) {

	std::string rgb;

	for (int i=(3*2) - 1; i>=0; i--) {
		rgb += hexDigits[((num >> i*4) & 0xF)];
	}

	return rgb;
}

std::string ColorHandler::getColorHex() {
	int rgbNum = ((red & 0xff) << 16)
		| ((green & 0xff) << 8)
		| (blue & 0xff);

	 return ConvertRGBtoHex(rgbNum);
}

void ColorHandler::randomizeColor() {
	ns3::RandomVariable gen = ns3::UniformVariable(0, 255);

	red = gen.GetInteger();
	green = gen.GetInteger();
	blue = gen.GetInteger();
}

void ColorHandler::lerp(ColorHandler to, double percentageOfTo) {

	if (percentageOfTo < 0.0)
		percentageOfTo = 0.0;
	if (percentageOfTo > 1.0)
		percentageOfTo = 1.0;

	red = (1-percentageOfTo)*red + percentageOfTo * to.getRed();
	blue = (1-percentageOfTo)*blue + percentageOfTo * to.getBlue();
	green = (1-percentageOfTo)*green + percentageOfTo * to.getGreen();
}

ColorHandler ColorHandler::lerpNew(ColorHandler to, double percentageOfTo) {

	if (percentageOfTo < 0.0)
		percentageOfTo = 0.0;
	if (percentageOfTo > 1.0) {
		percentageOfTo = 1.0;
	}

	double red_ = (1-percentageOfTo)*red + percentageOfTo * to.getRed();
	double blue_ = (1-percentageOfTo)*blue + percentageOfTo * to.getBlue();
	double green_ = (1-percentageOfTo)*green + percentageOfTo * to.getGreen();

	return ColorHandler(red_,green_,blue_);
}


uint8_t ColorHandler::getRed() {
	return red;
}

uint8_t ColorHandler::getGreen() {
	return green;
}

uint8_t ColorHandler::getBlue() {
	return blue;
}

} //namespace ns3
