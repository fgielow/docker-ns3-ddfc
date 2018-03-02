/*
 * GlobalAnalysis.cc
 *
 *  Created on: 27/08/2012
 *      Author: fgielow
 */

#include "GlobalAnalysis.h"
#include "ns3/nstime.h"

#include <cmath>
//#include <string>
#include <stdio.h>

double getStandardDeviation(std::list<double> l, double *avg) {

	if (l.size() <= 1)
		return 0;

	double sum = 0;

	for (std::list<double>::iterator i = l.begin(); i != l.end(); i++) {
		sum += *i;
	}

	*avg = sum / l.size();

	double numerator = 0;
	for (std::list<double>::iterator i = l.begin(); i != l.end(); i++) {
		numerator += pow(*i - *avg,2);
	}

	return sqrt(numerator/l.size());
}

double getAmplitude(std::list<double> l) {

	if (l.size() <= 1)
		return 0;

	double min = l.front();
	double max = l.front();


	for (std::list<double>::iterator i = l.begin(); i != l.end(); i++) {
		if (*i < min)
			min = *i;
		if (*i > max)
			max = *i;
	}

	return max - min;
}

namespace ns3 {

void GlobalAnalysis::addEdgeFromTo(uint32_t from,uint32_t to) {
	edges[from][to] = true;
	added_edges_on_interval++;
}

void GlobalAnalysis::rmEdgeFromTo(uint32_t from,uint32_t to) {
	edges[from][to] = false;
	removed_edges_on_interval++;
}

void GlobalAnalysis::TimerCheckMetricsExpire() {

	uint32_t inconsist = 0;
	for (uint32_t i=0; i<numberOfNodes-1; i++) {
		for (uint32_t j=i+1; j<numberOfNodes; j++) {
			if (edges[i][j] != edges[j][i])
				inconsist++;
		}
	}

	checkClusterMetrics();
	checkClusterHeadsIndexingMetrics();

	std::cout <<
			"time:" << ns3::Simulator::Now().GetSeconds() <<
			":quantity_of_sent_beacons:" << quantity_of_beacon_messages_sent <<
			":edges_inconsistency:" << inconsist <<
			":added_edges:" << added_edges_on_interval <<
			":removed_edges:" << removed_edges_on_interval <<
			":used_colors_without_loners:" << used_colors - lone_nodes <<
			":lone_nodes:" << lone_nodes <<
			":deviation_of_clusters:" << deviation_string <<
			":amplitude_of_clusters:" << amplitude_string <<
			":average_of_clusters:" << mean_string <<

			/* indexing metrics start now */

			":cluster_heads_without_loners:" << number_of_cluster_heads <<
			":inconsistent_routing_entries:" << number_of_inconsistent_points_routing_to_a_cluster_head <<
			":quantity_of_nodes_for_each_distance_to_cluster_head:" << quantity_of_nodes_for_each_distance_to_cluster_head_string <<

			"\n";

	std::flush(std::cout);

	added_edges_on_interval = 0;
	removed_edges_on_interval = 0;

	t_check_metrics.Schedule (t_interval);
}

bool GlobalAnalysis::paintVertex(uint32_t ndx) {
	if (colors[ndx] > numberOfNodes) {
		colors[ndx] = used_colors;
		current_cluster_readings.push_back(nodeAgents[ndx]->getReading());

		bool lone = true;
		for (uint32_t j=0; j<numberOfNodes; j++) {
			if (edges[ndx][j] || edges[j][ndx]) {
				lone = false;
				paintVertex(j);
			}
		}

		is_node_lone[ndx] = lone;

		if (lone) {
			lone_nodes++;
		}

		return true;
	} else {
		return false;
	}
}


bool GlobalAnalysis::checkClusteringPointCosistencyAmongNeighbors(uint32_t prev, uint32_t ndx, uint32_t hops) {

	// if a cluster head was reached or I am the cluster-head, then it is working :)
	if (nodeAgents[ndx]->fcl.isClusterHead()) {
		return true;
	}

	// possible loops
	if (hops > (uint32_t)(2 * (nodeAgents[0]->fcl.MAX_POINT_K_CLUSTERING)))
		return false;

	// there must be one valid route
	bool aggregate_or = false;

	// self explanatory
	bool has_valid_neighbor_with_greater_point = false;

	for (uint32_t i=0; i<numberOfNodes; i++) {
		// if I can communicate with him, indeed (consistently), and he has a greater point than I have
		if (edges[ndx][i] && edges[i][ndx] && nodeAgents[ndx]->fcl.getPoint() < /* BEWARE <= */ nodeAgents[i]->fcl.getPoint()) {
			has_valid_neighbor_with_greater_point = true;
			//if there is ANY invalid, it is sufficient to quit! there can't be a coherent invalid routing possibility
			if (!checkClusteringPointCosistencyAmongNeighbors(ndx, i, hops+1)) {
				return false;
			// but there must be at least one valid, huh?
			} else {
				aggregate_or = true;
			}
		}
	}

	// drastic routing: OK, no neighbor with lower point, maybe with EQUAL points?
	if (!has_valid_neighbor_with_greater_point) {
		for (uint32_t i=0; i<numberOfNodes; i++) {
			// if I can communicate with him, indeed (consistently), and he has equal points
			if (i != prev && edges[ndx][i] && edges[i][ndx] && nodeAgents[ndx]->fcl.getPoint() == nodeAgents[i]->fcl.getPoint()) {
				//if there is ANY invalid, it is sufficient to quit! there can't be a coherent invalid routing possibility
				if (!checkClusteringPointCosistencyAmongNeighbors(ndx, i, hops+1)) {
					return false;
				// but there must be at least one valid, huh?
				} else {
					aggregate_or = true;
				}
			}
		}
	}

	// did I find at least one valid?
	if (aggregate_or) {
		return true;
	} else {
		return false;
	}

}

void GlobalAnalysis::closeWholeSimulationMetricsAndFinish() {

	if (closed_finishing_metrics)
		return;

	closed_finishing_metrics = true;

	std::cout << "FINISH:";

	/*
	 *
	 * BEGIN OF  number_of_cluster_heads_that_remained_X_turns  CALCULUS
	 *
	 */

	uint32_t max_turns = 0;

	// closing consideration, if they are still cluster-heads here, that must be considered this last time
	for (uint32_t ndx=0; ndx<numberOfNodes; ndx++) {
		if (nodeAgents[ndx]->fcl.isClusterHead() && !is_node_lone[ndx]) {
			quantity_of_cluster_heads_for_each_numer_of_turns[turns_as_cluster_head[ndx]]++;
		}
	}

	for (int i=MAX_NODES_FOR_EDGES-1; i>=0; i--) {
		if (quantity_of_cluster_heads_for_each_numer_of_turns[i]>0) {
			max_turns=i;
			break;
		}
	}

	std::cout << "number_of_cluster_heads_that_remained_X_turns:";
	for (uint32_t i=0; i<max_turns-1; i++) {
		std::cout << quantity_of_cluster_heads_for_each_numer_of_turns[i] << ",";
	}
	std::cout << quantity_of_cluster_heads_for_each_numer_of_turns[max_turns] << ":";

	/*
	 *
	 * END OF number_of_cluster_heads_that_remained_X_turns CALCULUS
	 *
	 */

	std::cout << "\n\n";
	std::flush(std::cout);
}

void GlobalAnalysis::nodeHasSentBeacon(uint32_t ndx) {
	quantity_of_beacon_messages_sent++;
}

void GlobalAnalysis::checkClusterHeadsIndexingMetrics() {

	number_of_cluster_heads=0;
	number_of_inconsistent_points_routing_to_a_cluster_head=0;

	for (uint32_t i=0; i<=nodeAgents[0]->fcl.MAX_POINT_K_CLUSTERING; i++) {
		quantity_of_nodes_for_each_distance_to_cluster_head[i]=0;
	}

//	uint32_t max_distance_to_cluster_head = 0;
	for (uint32_t ndx=0; ndx<numberOfNodes; ndx++) {
		if (!is_node_lone[ndx]) {
			if (nodeAgents[ndx]->fcl.isClusterHead()) {
				number_of_cluster_heads++;
				turns_as_cluster_head[ndx]++;
			} else {
				if (turns_as_cluster_head[ndx] > 0) {
					quantity_of_cluster_heads_for_each_numer_of_turns[turns_as_cluster_head[ndx]]++;
				}
				turns_as_cluster_head[ndx] = 0;
			}
		}

//		uint32_t hops_to_closest_cluter_head = 0;

		if (!is_node_lone[ndx] && !nodeAgents[ndx]->fcl.isClusterHead() && !checkClusteringPointCosistencyAmongNeighbors(-1, ndx, 0)) {
			number_of_inconsistent_points_routing_to_a_cluster_head++;
			std::cout << "XXX inconsistency found on node  " << ndx << "\n"; std::flush(std::cout);
		}

		// NOTE: LONE NODES ARE CHS WITH DISTANCE 0
		quantity_of_nodes_for_each_distance_to_cluster_head[nodeAgents[ndx]->fcl.MAX_POINT_K_CLUSTERING - nodeAgents[ndx]->fcl.point]++;
	}

	sprintf(quantity_of_nodes_for_each_distance_to_cluster_head_string,"%s","");

	for (uint32_t i=0; i<=nodeAgents[0]->fcl.MAX_POINT_K_CLUSTERING; i++) {
		sprintf(quantity_of_nodes_for_each_distance_to_cluster_head_string,"%s%s%d",
				quantity_of_nodes_for_each_distance_to_cluster_head_string,i > 0 ? "," : "",quantity_of_nodes_for_each_distance_to_cluster_head[i]);
	}

}

void GlobalAnalysis::checkClusterMetrics() {
	used_colors = 0;
	lone_nodes  = 0;

	for (uint32_t ndx=0; ndx<numberOfNodes; ndx++) {
		colors[ndx] = numberOfNodes + 1;
	}

	sprintf(deviation_string,"%s","");
	sprintf(amplitude_string,"%s","");
	sprintf(mean_string,"%s","");
	double avg;

	for (uint32_t ndx=0; ndx<numberOfNodes; ndx++) {
		uint32_t last_number_of_lones = lone_nodes;
		current_cluster_readings.clear();
		if (paintVertex(ndx)) {
			used_colors++;
			if (last_number_of_lones == lone_nodes) {// current node is NOT lone!
				sprintf(deviation_string,"%s%s%lf",deviation_string,used_colors > 1 ? "," : "",getStandardDeviation(current_cluster_readings, &avg));
				sprintf(mean_string,"%s%s%lf",mean_string,used_colors > 1 ? "," : "",avg);
				sprintf(amplitude_string,"%s%s%lf",amplitude_string,used_colors > 1 ? "," : "",getAmplitude(current_cluster_readings));
			}
		}
	}
}

void GlobalAnalysis::increaseNumberOfNodes(FireflyClustering *fc){
	nodeAgents[numberOfNodes++] = fc;
}

void GlobalAnalysis::startTimerPeriodicAfterWithInterval(double after, double interval) {

	if (is_running)
		return;

	std::cout << ">> gonna start " <<after << ", " <<interval << "\n";

	is_running = true;

	t_interval = Seconds(interval);
	t_check_metrics.SetFunction (&GlobalAnalysis::TimerCheckMetricsExpire, this);

	t_check_metrics.Schedule(Seconds(after));
}

GlobalAnalysis::GlobalAnalysis() {
	numberOfNodes = 0;
	is_running = false;
	closed_finishing_metrics = false;
	added_edges_on_interval = 0;
	removed_edges_on_interval = 0;

	for (uint32_t i=0; i<MAX_NODES_FOR_EDGES; i++) {
		quantity_of_cluster_heads_for_each_numer_of_turns[i] = 0;
		quantity_of_nodes_for_each_distance_to_cluster_head[i] = 0;
		turns_as_cluster_head[i] = 0;
		nodeAgents[i] = NULL;
		is_node_lone[i] = true;

	}

}

} /* namespace ns3 */
