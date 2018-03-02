/*
 * FireflyClusteringLogic.cc
 *
 *  Created on: 29/08/2012
 *      Author: fgielow
 */

#include "FireflyClusteringLogic.h"
#include "ns3/Constants.h"

#include <iostream>

namespace ns3 {

FireflyClusteringLogic::FireflyClusteringLogic(uint32_t ndx, uint8_t max_c, bool khopca) {
	point = 0;
	myaddr = ndx;
	MAX_POINT_K_CLUSTERING = max_c;
	KHOPCA_behavior = khopca;
//	std::cout << ">>" << khopca << "," << KHOPCA_behavior << "\n";
//	std::cout << ">>>>" << ndx << "::" << (int) max_c << "," << (int) MAX_POINT_K_CLUSTERING; std::flush(std::cout) << "\n";
}

bool FireflyClusteringLogic::isClusterHead() {
	if (point == MAX_POINT_K_CLUSTERING)
		return true;
	return false;
}

uint8_t FireflyClusteringLogic::decreasePoint() {
	if (point > 0)
		setPoint(point - 1);
	return point;
}

uint8_t FireflyClusteringLogic::setPoint(uint8_t val) {
	point = val;

	//if (point < 0)
	//	point = 0;
	if (point > MAX_POINT_K_CLUSTERING)
		point = MAX_POINT_K_CLUSTERING;

	return point;
}

uint8_t FireflyClusteringLogic::getPoint() {
//	std::cout << "}}}}" << myaddr << "::" << (int) MAX_POINT_K_CLUSTERING; std::flush(std::cout) << "\n";
	return point;
}

bool FireflyClusteringLogic::checkRule1() {
	bool isLowest = true;
	uint8_t maxPoint = point;

	for (std::map<uint32_t,node_cluster_info>::iterator it = neighbors.begin(); it != neighbors.end(); it++) {
		if (point >/*=*/ it->second.points/* || (point == it->second.points && it->first > myaddr)*/) {
			isLowest = false;
			break;
		}

		if (it->second.points > maxPoint)
			maxPoint = it->second.points;
	}

	if (isLowest && neighbors.size() > 0) {
		setPoint(maxPoint-1);
//		std::cout << "node " << (int) myaddr << " applied rule 1 \n"; std::flush(std::cout);
		return true;
	}
	return false;

}

bool FireflyClusteringLogic::checkRule2() {
	bool everyNeighborMin = true;

	for (std::map<uint32_t,node_cluster_info>::iterator it = neighbors.begin(); it != neighbors.end(); it++) {
		if (it->second.points > 0) {
			everyNeighborMin = false;
			break;
		}
	}

	if (everyNeighborMin) {
		setPoint(MAX_POINT_K_CLUSTERING);
//		std::cout << "node " << (int) myaddr << " applied rule 2 \n"; std::flush(std::cout);
		return true;
	}
	return false;

}

bool FireflyClusteringLogic::checkRule3() {

	if (point == MAX_POINT_K_CLUSTERING)
		return false;

	bool notHeadAndHasGreaterPointsThanNeighbors = true;

	for (std::map<uint32_t,node_cluster_info>::iterator it = neighbors.begin(); it != neighbors.end(); it++) {
		if (it->second.points > /*=*/ /* BEWARE FIXME */  point) {
			notHeadAndHasGreaterPointsThanNeighbors = false;
			break;
		}
	}

	if (notHeadAndHasGreaterPointsThanNeighbors) {
		decreasePoint();
//		std::cout << "node " << (int) myaddr << " applied rule 3 \n"; std::flush(std::cout);
		return true;
	}
	return false;

}

bool FireflyClusteringLogic::checkRule4(uint8_t currentNumberOfNeighbors, bool khopca) {

	if(point != MAX_POINT_K_CLUSTERING)
		return false;

	bool headAndOtherHeadWithGreaterIdNear = false;

	for (std::map<uint32_t,node_cluster_info>::iterator it = neighbors.begin(); it != neighbors.end(); it++) {
		if (it->second.points == MAX_POINT_K_CLUSTERING &&
			(
				khopca /* if khopca I do not verify anything else */ ||
				/* not khopca cases below */
				(it->second.numberOfNeighbors > currentNumberOfNeighbors) ||
				(it->second.numberOfNeighbors == currentNumberOfNeighbors && it->first > myaddr)
			)
		) {
			headAndOtherHeadWithGreaterIdNear = true;
			break;
		}
	}

	if (headAndOtherHeadWithGreaterIdNear) {
		decreasePoint();
//		std::cout << "node " << (int) myaddr << " applied rule 4 \n"; std::flush(std::cout);
		return true;
	}
	return false;

}

bool FireflyClusteringLogic::updatePoint(uint8_t currentNumberOfNeighbors) {

	if (!KHOPCA_behavior) {
		bool ret = checkRule1();

		if (!ret)
			ret = checkRule2();

		if (!ret)
			ret = checkRule3();

		if (!ret)
			ret = checkRule4(currentNumberOfNeighbors,KHOPCA_behavior);

		return ret;
	} else {
		/* HERE IS DEFAULT KHOPCA */
		bool ret = checkRule1();
		ret = ret || checkRule2();
		ret = ret || checkRule3();
		ret = ret || checkRule4(currentNumberOfNeighbors,KHOPCA_behavior);

		return ret;
	}
}

void FireflyClusteringLogic::registerNeighbor(uint32_t n, uint8_t p, uint8_t neigh) {
	node_cluster_info nci;
	nci.points = p;
	nci.numberOfNeighbors = neigh;

	std::map<uint32_t,node_cluster_info>::iterator it = neighbors.find(n);
	if (it != neighbors.end()) {
		it->second.points = p;
		it->second.numberOfNeighbors = neigh;
	} else {
		neighbors.insert(std::make_pair(n,nci));
	}
}

void FireflyClusteringLogic::removeNeighbor(uint32_t n) {
	neighbors.erase(n);
}

} /* namespace ns3 */
