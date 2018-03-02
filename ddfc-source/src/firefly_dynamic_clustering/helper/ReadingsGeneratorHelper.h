/*
 * ReadingsGeneratorHelper.h
 *
 *  Created on: 11/07/2012
 *      Author: fernandogielow
 */

#ifndef READINGSGENERATORHELPER_H_
#define READINGSGENERATORHELPER_H_

#include <queue>

namespace ns3 {

typedef struct {
	double time_;
	double value_;
} reading_t;

class ReadingsGeneratorHelper {
public:
	ReadingsGeneratorHelper(double, double);
	ReadingsGeneratorHelper();
	void registerReading(double, double);
	double getReading(double);
	double normalizeReading(double);
	void __debug__generateReadings(int,double);

private:
	std::queue<reading_t> readings;
	reading_t last_reading;
	double minReading, maxReading;
	bool first_reading;
};

double lerp(double, double, double);

} /* namespace ns3 */


#endif /* READINGSGENERATORHELPER_H_ */
