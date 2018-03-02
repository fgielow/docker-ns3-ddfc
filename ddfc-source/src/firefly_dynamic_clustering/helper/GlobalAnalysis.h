/*
 * GlobalAnalysis.h
 *
 *  Created on: 27/08/2012
 *      Author: fgielow
 */

#ifndef GLOBALANALYSIS_H_
#define GLOBALANALYSIS_H_

#include "ns3/Constants.h"
#include "ns3/timer.h"
#include "ns3/firefly_dynamic_clustering.h"
#include <stdint.h>
#include <list>

namespace ns3 {

class FireflyClustering;

class GlobalAnalysis {
private:
	uint32_t numberOfNodes;

	bool edges[MAX_NODES_FOR_EDGES][MAX_NODES_FOR_EDGES];
	FireflyClustering *nodeAgents[MAX_NODES_FOR_EDGES];

	Timer t_check_metrics;
	void TimerCheckMetricsExpire();
	Time t_interval;
	bool is_running;
	bool closed_finishing_metrics;

	bool is_node_lone[MAX_NODES_FOR_EDGES];

	uint32_t added_edges_on_interval, removed_edges_on_interval;

	uint32_t colors[MAX_NODES_FOR_EDGES];
	void checkClusterMetrics();
	void checkClusterHeadsIndexingMetrics();
	bool paintVertex(uint32_t);
	uint32_t used_colors;
	uint32_t lone_nodes;
	std::list<double> current_cluster_readings;
	char deviation_string[500], amplitude_string[500], mean_string[500], quantity_of_nodes_for_each_distance_to_cluster_head_string[500];

	uint32_t number_of_cluster_heads;
	uint32_t number_of_inconsistent_points_routing_to_a_cluster_head;
	uint32_t turns_as_cluster_head[MAX_NODES_FOR_EDGES];
	uint32_t quantity_of_cluster_heads_for_each_numer_of_turns[MAX_NODES_FOR_EDGES]; // this is a unique metric to the entire simulation, CAREFUL, it only increases throughout time
	uint32_t quantity_of_nodes_for_each_distance_to_cluster_head[MAX_NODES_FOR_EDGES];
	uint32_t quantity_of_beacon_messages_sent;

public:
	GlobalAnalysis();
	void increaseNumberOfNodes(FireflyClustering*);
	void addEdgeFromTo(uint32_t,uint32_t);
	void rmEdgeFromTo(uint32_t,uint32_t);
	void startTimerPeriodicAfterWithInterval(double, double);
	bool checkClusteringPointCosistencyAmongNeighbors(uint32_t, uint32_t, uint32_t);
	void closeWholeSimulationMetricsAndFinish();
	void nodeHasSentBeacon(uint32_t);
};

} /* namespace ns3 */
#endif /* GLOBALANALYSIS_H_ */
