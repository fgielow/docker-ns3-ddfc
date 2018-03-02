/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef __FIREFLY_DYNAMIC_CLUSTERING_H__
#define __FIREFLY_DYNAMIC_CLUSTERING_H__

#include "ns3/application.h"
#include "ns3/simulator.h"
#include "ns3/socket.h"
#include "ns3/timer.h"

#include "ns3/ColorGradientHelper.h"
#include "ns3/ReadingsGeneratorHelper.h"
#include "ns3/GlobalAnalysis.h"
#include "ns3/Constants.h"

#include "ns3/FireflyClusteringLogic.h"

#include "ns3/firefly_dynamic_clustering-header.h"

#include <set>
#include <map>

#define PYVIZ


namespace ns3 {

class GlobalAnalysis;

typedef struct {
	double individual_reading;
	double aggregated_reding;
	uint8_t number_of_aggregated_readings;
} neighbor_reading_t;

//class Socket;

class FireflyClustering : public Application
{
public:

	static double MIN_READING;
	static double MAX_READING;

	FireflyClustering();

	static TypeId GetTypeId (void);
	static void initializeReadingGUIDependencies(type_of_readings);

#ifdef PYVIZ
	static int32_t getNewEdges_begin(); // for PyViz plugin, implement later
	static int32_t getNewEdges_end(); // for PyViz plugin, implement later
	static int32_t getEdgesToBeRemoved_begin(); // for PyViz plugin, implement later
	static int32_t getEdgesToBeRemoved_end(); // for PyViz plugin, implement later
	static bool PyVizLocked();
#endif

	void AddReading(double,double);
	double getReading();
	std::string getColor();
	std::string getLabel();

	static ColorGradientHelper colorHelper;

	FireflyClusteringLogic fcl;

private:

	static GlobalAnalysis __ga__;
	uint32_t MYADDR;

#ifdef PYVIZ
	static int32_t pyviz_edges_to_add_begin[MAX_NODES_FOR_EDGES];
	static int32_t pyviz_edges_to_add_end[MAX_NODES_FOR_EDGES];
	static int32_t pyviz_edges_to_add_num;

	static int32_t pyviz_edges_to_remove_begin[MAX_NODES_FOR_EDGES];
	static int32_t pyviz_edges_to_remove_end[MAX_NODES_FOR_EDGES];
	static int32_t pyviz_edges_to_remove_num;

	static bool pyviz_lock;
	static void addNewEdge(int32_t, int32_t);
	static void removeEdge(int32_t, int32_t);
#endif


	bool has_clustering_logic_updated_point;
	bool has_received_neighbor_beaconing;
	bool has_already_doubled_adaptative_interval;

	std::set<uint32_t> m_neighbors; // neighbors which satisfy clustering threshold
	std::map<uint32_t,neighbor_reading_t> m_neighbors_readings; // all spatial neighbors!!!!!!!!

	ReadingsGeneratorHelper readingsGenerator;

	Time m_interval;
	Time m_dynamic_interval;
	double m_metric_gathering_interval;
	uint8_t m_max_clustering_k;
	uint8_t m_verbose;
	double m_cluster_delta_reading_thresh;

	double m_aggregated_reading_cached;
	Timer m_beacon_timer;
	Timer m_dynamic_beacon_checking_timer;
	bool m_adaptative_interval;
	bool m_default_khopca_rules;

	Ptr<ns3::Socket> beacon_sink;
	Ptr<ns3::Socket> beacon_source;

	// inherited from Application base class.
	virtual void StartApplication (void);
	virtual void StopApplication (void);
	virtual void DoDispose (void);

	void Receive (Ptr<Socket> socket);
	void ReceiveBeacon(Ptr<Packet>);
	void SendBeacon();

	void initializeBeacon();
	void TimerBeaconExpire();
	void TimerDynamicallyCheckBeaconExpire();
	int dynamicIntervalTimerLock;

	inline bool checkAddEdge(uint32_t);
	inline bool checkRemoveEdge(uint32_t);
	inline void updateNeighborReadings(uint32_t,double,double,uint8_t);
	inline void createSockets();

	inline double getAverageReading();

}; // class FireflyClustering

} // namespace ns3

#endif /* __FIREFLY_DYNAMIC_CLUSTERING_H__ */

