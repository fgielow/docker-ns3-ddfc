/*
 * ReadingsGeneratorHelper.cc
 *
 *  Created on: 11/07/2012
 *      Author: fernandogielow
 */

#include "ReadingsGeneratorHelper.h"
#include "ns3/random-variable.h"

namespace ns3 {

double lerp(double from, double to, double percentageOfTo) {
	return (1-percentageOfTo)*from + percentageOfTo*to;
}

ReadingsGeneratorHelper::ReadingsGeneratorHelper(double minR, double maxR): first_reading(true) {
//	std::cout << "ME TOOOOOO!!!!\n";
	minReading = minR;
	maxReading = maxR;

//	last_reading.time_ = 0.0;
//	last_reading.value_ = (minReading+maxReading)/2;
}

ReadingsGeneratorHelper::ReadingsGeneratorHelper() : first_reading(true) {
	minReading = 0.0;
	maxReading = 100.0;

//	std::cout << "I have been called!!!!!\n";
//	last_reading.time_ = 0.0;
//	last_reading.value_ = (minReading+maxReading)/2;
}

void ReadingsGeneratorHelper::registerReading(double time, double reading) {
	if (first_reading) {
		last_reading.time_ = 0.0;
		last_reading.value_ = reading;
		first_reading = false;
	}

	reading_t r_;
	r_.time_ = time;
	r_.value_ = reading;

	readings.push(r_);
}

double ReadingsGeneratorHelper::getReading(double time) {

	reading_t front_ = readings.front();

	while (time > front_.time_ && readings.size() > 0) {
		last_reading = readings.front();
		readings.pop();
	}

	if (readings.size()>0) {
		return lerp(
				last_reading.value_,
				front_.value_,
				(time - last_reading.time_) / (front_.time_ - last_reading.time_)
			);
//		std::cout << "last reading " << last_reading.time_ << " time " << time << " front " << readings.front().time_ << " gives " << ((time - last_reading.time_) / (readings.front().time_ - last_reading.time_)) << "\n";
	} else {
		return 0.0;
	}
}

double ReadingsGeneratorHelper::normalizeReading(double r_) {
	if (r_ < minReading) {
		return minReading;
	}
	if (r_ > maxReading) {
		return maxReading;
	}
	return r_;
}

void ReadingsGeneratorHelper::__debug__generateReadings(int numberOfReadingsToGenerate, double totalTime) {

	ns3::RandomVariable delta = ns3::UniformVariable(-6.0, 6.0);

	double lastReading_l = (minReading+maxReading)/2;

	for (int i=0; i<=numberOfReadingsToGenerate; i++) {
		lastReading_l = normalizeReading(lastReading_l + delta.GetValue());
		registerReading(i*(totalTime/numberOfReadingsToGenerate),lastReading_l);
	}
}


} /* namespace ns3 */
