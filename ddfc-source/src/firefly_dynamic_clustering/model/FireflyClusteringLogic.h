/*
 * FireflyClusteringLogic.h
 *
 *  Created on: 29/08/2012
 *      Author: fgielow
 */

#ifndef FIREFLYCLUSTERINGLOGIC_H_
#define FIREFLYCLUSTERINGLOGIC_H_

#include <stdint.h>
#include <map>

namespace ns3 {

typedef struct {
	uint8_t points;
	uint8_t numberOfNeighbors;
} node_cluster_info;

class FireflyClusteringLogic {
	friend class GlobalAnalysis;

public:
	FireflyClusteringLogic(uint32_t ndx=0, uint8_t max_c=3, bool khopca=false);
	uint8_t getPoint();
	void registerNeighbor(uint32_t, uint8_t, uint8_t);
	void removeNeighbor(uint32_t);
	bool updatePoint(uint8_t);

	bool isClusterHead();

private:
	uint32_t myaddr;
	uint8_t point;
	uint8_t MAX_POINT_K_CLUSTERING;
	std::map<uint32_t, node_cluster_info> neighbors;

	bool KHOPCA_behavior;

	bool checkRule1();
	bool checkRule2();
	bool checkRule3();
	bool checkRule4(uint8_t, bool);

	uint8_t decreasePoint();
	uint8_t setPoint(uint8_t);

};

} /* namespace ns3 */
#endif /* FIREFLYCLUSTERINGLOGIC_H_ */
