/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "firefly_dynamic_clustering.h"

#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/double.h"

#include "ns3/ipv4-address.h"
#include "ns3/ipv4.h"
#include "ns3/socket.h"
#include "ns3/inet-socket-address.h"
#include "ns3/packet.h"
#include "ns3/Constants.h"

#include "ns3/random-variable.h"

#include <cmath>
#include <iomanip>
#include <stdint.h>
#include <stdio.h>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (FireflyClustering);
//#define TEMPERATURE_READING

GlobalAnalysis FireflyClustering::__ga__ = GlobalAnalysis();

double FireflyClustering::MIN_READING;
double FireflyClustering::MAX_READING;
ColorGradientHelper FireflyClustering::colorHelper;

void FireflyClustering::initializeReadingGUIDependencies(type_of_readings type) {
	switch (type) {
		case (temperature): {
			MIN_READING = 10.0;
			MAX_READING = 30.0;
			ColorHandler color_args[] = {
				ColorHandler(0,40,255),
				ColorHandler(0,207,181),
				ColorHandler(20,220,0),
				ColorHandler(225,255,0),
				ColorHandler(255,0,0)
			};
			colorHelper = ColorGradientHelper(FireflyClustering::MIN_READING,FireflyClustering::MAX_READING,5,color_args);
			break;
		}
		case (light): {
			MIN_READING = 0.0;
			MAX_READING = 200.0;
			ColorHandler color_args[] = {
				ColorHandler(0,0,0),
				ColorHandler(255,255,255)
			};
			colorHelper = ColorGradientHelper(FireflyClustering::MIN_READING,FireflyClustering::MAX_READING,2,color_args);
			break;
		}
		case (humidity): {
			MIN_READING = 32.0;
			MAX_READING = 52.0;
			ColorHandler color_args[] = {
				ColorHandler(255,0,0),
				ColorHandler(255,0,0),
				ColorHandler(225,255,0),
				ColorHandler(20,220,0),
				ColorHandler(0,207,181),
				ColorHandler(0,40,255)
			};
			colorHelper = ColorGradientHelper(FireflyClustering::MIN_READING,FireflyClustering::MAX_READING,5,color_args);
			break;
		}
		case (energy): {
			MIN_READING = 2.41;
			MAX_READING = 2.83;
			ColorHandler color_args[] = {
				ColorHandler(255,0,0),
				ColorHandler(0,255,255)
			};
			colorHelper = ColorGradientHelper(FireflyClustering::MIN_READING,FireflyClustering::MAX_READING,2,color_args);
			break;
		}
	}
}

TypeId
FireflyClustering::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::FireflyClustering")
    .SetParent<Application> ()
    .AddConstructor<FireflyClustering> ()
    .AddAttribute ("Verbose",
                  "Produce usual output. Integer specifies level of verbosity.",
                  UintegerValue(5),
                  MakeUintegerAccessor (&FireflyClustering::m_verbose),
                  MakeUintegerChecker<uint8_t>())
   .AddAttribute ("AdaptativeInterval",
				  "Beacon interval shall be adjusted dynamically",
				  BooleanValue(false),
				  MakeBooleanAccessor (&FireflyClustering::m_adaptative_interval),
				  MakeBooleanChecker())
  .AddAttribute ("KHOPCARules",
				  "Use default KHOPCA behavior",
				  BooleanValue(false),
				  MakeBooleanAccessor (&FireflyClustering::m_default_khopca_rules),
				  MakeBooleanChecker())
   .AddAttribute ("MaxClusteringK",
				  "Maximum distance to cluster head.",
				  UintegerValue(3),
				  MakeUintegerAccessor (&FireflyClustering::m_max_clustering_k),
				  MakeUintegerChecker<uint8_t>())
   .AddAttribute ("Interval", "Wait  interval  seconds between each beacon.",
                  TimeValue (Seconds (120)),
                  MakeTimeAccessor (&FireflyClustering::m_interval),
                  MakeTimeChecker ())
   .AddAttribute ("DynamicCheckingInterval", "Interval between each checking of criteria for beaconing",
				  TimeValue (Seconds (6)), /* I NEED TO ADAPT THIS DYNAMICALY FIXME */
				  MakeTimeAccessor (&FireflyClustering::m_dynamic_interval),
				  MakeTimeChecker ())
   .AddAttribute ("ClusterThresh", "Max difference between readings to cluster.",
				  DoubleValue(20.0),
				  MakeDoubleAccessor(&FireflyClustering::m_cluster_delta_reading_thresh),
				  MakeDoubleChecker<double>())
   .AddAttribute ("MetricGatheringInterval", "Max difference between readings to cluster.",
				  DoubleValue(10.0),
				  MakeDoubleAccessor(&FireflyClustering::m_metric_gathering_interval),
				  MakeDoubleChecker<double>())
  ;
  return tid;
}

bool FireflyClustering::checkAddEdge(uint32_t src) {
	// if not there, add edge
	if (m_neighbors.find(src) == m_neighbors.end()) {
		#ifdef PYVIZ
			addNewEdge(src,GetNode()->GetId());
		#endif
		m_neighbors.insert(src);
		__ga__.addEdgeFromTo(src,GetNode()->GetId());
		return true;
	}
	return false;
}

bool FireflyClustering::checkRemoveEdge(uint32_t src) {
	// if there, remove edge
	if (m_neighbors.find(src) != m_neighbors.end()) {
		#ifdef PYVIZ
			removeEdge(src,GetNode()->GetId());
		#endif
		m_neighbors.erase(src);
		__ga__.rmEdgeFromTo(src,GetNode()->GetId());
		return true;
	}
	return false;
}

void FireflyClustering::updateNeighborReadings(uint32_t src,double individualR,double aggregatedR,uint8_t aggregatedN) {
	neighbor_reading_t nr_;
	nr_.aggregated_reding = aggregatedR;
	nr_.number_of_aggregated_readings = aggregatedN;
	nr_.individual_reading = individualR;

	std::map <uint32_t,neighbor_reading_t>::iterator it = m_neighbors_readings.find(src);
	if (it == m_neighbors_readings.end())
		m_neighbors_readings.insert(std::pair <uint32_t, neighbor_reading_t> (src,nr_));
	else
		it->second = nr_;
}

double FireflyClustering::getAverageReading() {
	double avg = getReading();
	uint32_t total_readings=1;
	for (std::set<uint32_t>::iterator it=m_neighbors.begin(); it!=m_neighbors.end();it++) {
		avg += m_neighbors_readings[*it].aggregated_reding * m_neighbors_readings[*it].number_of_aggregated_readings;
		total_readings += m_neighbors_readings[*it].number_of_aggregated_readings;
	}
	avg/=total_readings;
	m_aggregated_reading_cached = avg;
	return avg;
}

void FireflyClustering::ReceiveBeacon(Ptr<Packet> packet) {

	FireflyBeaconHeader fireflyBeacon;
	packet->RemoveHeader (fireflyBeacon);

	uint32_t src = fireflyBeacon.getSrc();
	uint8_t aggregatedN = fireflyBeacon.getNumberOfReadings();
	uint8_t hisPoint = fireflyBeacon.getClusteringPoint();
	double hisReading = fireflyBeacon.getIndividualReading();
	double aggregatedReading = fireflyBeacon.getAggregatedReading();

	updateNeighborReadings(src, hisReading, aggregatedReading, aggregatedN);
	// gonna send periodically, anyways.. why so much processing here?
	double avg = m_aggregated_reading_cached; //getAverageReading();

	// should cluster
	if (std::abs(hisReading - avg) < m_cluster_delta_reading_thresh &&
			std::abs(getReading() - aggregatedReading) < m_cluster_delta_reading_thresh) {
		if (checkAddEdge(src))
			has_received_neighbor_beaconing = true;
		// process clustering logic
		fcl.registerNeighbor(src,hisPoint,aggregatedN);
	// should not cluster
	} else {
		if (checkRemoveEdge(src))
			has_received_neighbor_beaconing = true;
		// process clustering logic
		fcl.removeNeighbor(src);
	}

	if (m_verbose>7) {
		std::cout << GetNode()->GetId() << " received one packet from " << fireflyBeacon.getSrc() << " reading " << fireflyBeacon.getIndividualReading() << " at " << Simulator::Now().GetSeconds() << "s\n";
	}

}

void FireflyClustering::Receive (Ptr<Socket> socket)
{
	Ptr<Packet> packet = socket->Recv();

	FireflyTypeHeader typeHeader;
	packet->RemoveHeader (typeHeader);

	if (!typeHeader.IsValid()) {
		if (m_verbose>2)
			std::cout << "FIREFLY message " << packet->GetUid () << " with unknown type received: " << typeHeader.Get () << ". Drop \n";
		return; // drop
	}

	switch (typeHeader.Get())
	{
	case FIREFLY_BEACON:
	  {
		ReceiveBeacon(packet);
		break;
	  }
	default: { break; }
	}
}

FireflyClustering::FireflyClustering () :
//		m_interval (Seconds (1)),
//		m_verbose (3),
//		m_cluster_delta_reading_thresh(6.0), // initialized on TypeId thing
		readingsGenerator(ReadingsGeneratorHelper(MIN_READING,MAX_READING)),
		m_beacon_timer (Timer::CANCEL_ON_DESTROY),
		m_dynamic_beacon_checking_timer(Timer::CANCEL_ON_DESTROY) {

#ifdef PYVIZ
	pyviz_lock = false;
	pyviz_edges_to_add_num=0;
	pyviz_edges_to_remove_num=0;
#endif

	has_clustering_logic_updated_point = false;

} // fireflyclustering constructor

void FireflyClustering::AddReading(double reading,double time) {
	readingsGenerator.registerReading(reading, time);
}

double FireflyClustering::getReading() {
	return readingsGenerator.getReading(ns3::Simulator::Now().GetSeconds());
}

std::string FireflyClustering::getLabel() {
	std::ostringstream out;
	out << std::setiosflags(std::ios::fixed) << std::setprecision(1);
	out << (GetNode()->GetId()) <<
			"\n(" << getReading() << ")" <<
			"\n[" << m_aggregated_reading_cached << "]" <<
			"\n{" << (int) fcl.getPoint() << "}";

//	printf(">>%d<<",fcl.getPoint());
	return out.str();
}

std::string FireflyClustering::getColor() {
	return "#" + colorHelper.getColorOfReading(getReading()).getColorHex();
}

void FireflyClustering::createSockets() {
	beacon_sink = Socket::CreateSocket (GetNode(), TypeId::LookupByName ("ns3::UdpSocketFactory"));
	InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), BEACON_PORT);
	beacon_sink->Bind (local);
	beacon_sink->SetRecvCallback (MakeCallback (&FireflyClustering::Receive, this));

	beacon_source = Socket::CreateSocket (GetNode(), TypeId::LookupByName ("ns3::UdpSocketFactory"));
	InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), BEACON_PORT);
	beacon_source->SetAllowBroadcast (true);
	beacon_source->Connect (remote);
}

void FireflyClustering::initializeBeacon() {
	m_beacon_timer.SetFunction (&FireflyClustering::TimerBeaconExpire, this);
	m_beacon_timer.Schedule (MilliSeconds (UniformVariable ().GetInteger (0, 100)));

	if (m_adaptative_interval) {
		m_dynamic_beacon_checking_timer.SetFunction(&FireflyClustering::TimerDynamicallyCheckBeaconExpire, this);
		m_dynamic_beacon_checking_timer.Schedule(MilliSeconds(0));
	}

}

void FireflyClustering::StartApplication() {

	dynamicIntervalTimerLock = 0;
	MYADDR = GetNode()->GetId();

	if (m_verbose>6) {
		std::cout << "node started: " << MYADDR << " at " << Simulator::Now().GetSeconds() << "s\n";
	}

	createSockets();
	initializeBeacon();

//	readingsGenerator.__debug__generateReadings(36,600.0);

	m_aggregated_reading_cached = getReading();

//	std::cout << "m_default_khopca_rules is " << m_default_khopca_rules << "\n";
	fcl = FireflyClusteringLogic(GetNode()->GetId(), (uint8_t) m_max_clustering_k, (bool) m_default_khopca_rules);

	__ga__.increaseNumberOfNodes(this);
//	__ga__.startTimerPeriodicAfterWithInterval(m_interval.GetDouble()/10e8/2,m_metric_gathering_interval);
	__ga__.startTimerPeriodicAfterWithInterval(0,m_metric_gathering_interval);
}

void FireflyClustering::TimerDynamicallyCheckBeaconExpire() {

	/* added */
	dynamicIntervalTimerLock--;

	if (dynamicIntervalTimerLock > 0)
		return;
	/* end added */

	// changed a lot
	if (std::abs(getReading() - m_aggregated_reading_cached) > 1.5 * m_cluster_delta_reading_thresh) {
//		Time t = m_beacon_timer.GetDelayLeft();
		m_beacon_timer.Cancel();
		m_beacon_timer.Schedule(Seconds(0));
	} else if (has_clustering_logic_updated_point || has_received_neighbor_beaconing) {

		/* added */ dynamicIntervalTimerLock = 2; /* end added */

		Time t = m_beacon_timer.GetDelayLeft();
		m_beacon_timer.Cancel();

		if (!has_already_doubled_adaptative_interval) {
			m_beacon_timer.Schedule(Seconds(t.GetSeconds()/2));
		} else {
			m_beacon_timer.Schedule(Seconds(t.GetSeconds()/8));
		}

		has_clustering_logic_updated_point = false;
		has_received_neighbor_beaconing = false;

	} else if (!has_already_doubled_adaptative_interval
	/* added */ && dynamicIntervalTimerLock < 0 /* end added */) {

		/* added */ dynamicIntervalTimerLock = 3; /* end added */

		Time t = m_beacon_timer.GetDelayLeft();
		m_beacon_timer.Cancel();
		m_beacon_timer.Schedule(Seconds(t.GetSeconds() * 3));
		has_already_doubled_adaptative_interval = true;
	}

	m_dynamic_beacon_checking_timer.Schedule(m_dynamic_interval);
}

void FireflyClustering::TimerBeaconExpire() {

	has_clustering_logic_updated_point = fcl.updatePoint(m_neighbors.size());
	SendBeacon();

//	m_beacon_timer.Cancel ();
	Time t = Time (0.1 * MilliSeconds (UniformVariable ().GetInteger (0, 100)));
	m_beacon_timer.Schedule (m_interval - t);

	has_already_doubled_adaptative_interval = false;

}

void FireflyClustering::SendBeacon() {

	__ga__.nodeHasSentBeacon(GetNode()->GetId());

	Ptr<Packet> packet = Create<Packet> ();

	FireflyBeaconHeader fireflyBeacon = FireflyBeaconHeader(MYADDR,getReading(),getAverageReading(),m_neighbors.size(), fcl.getPoint());
	packet->AddHeader(fireflyBeacon);

	FireflyTypeHeader typeHeader(FIREFLY_BEACON);
	packet->AddHeader(typeHeader);

	beacon_source->Send(packet);
}

void FireflyClustering::StopApplication() {
	if (m_verbose>1) {
		std::cout << "node stopped: " << MYADDR << " at " << Simulator::Now().GetSeconds() << "s\n";
	}
}

void FireflyClustering::DoDispose() {
	if (m_verbose>8) {
		std::cout << "node disposed: " << MYADDR << " at " << Simulator::Now().GetSeconds() << "s\n";
	}

	// this is really the best place to leave this?? FIXME
	__ga__.closeWholeSimulationMetricsAndFinish();

}


#ifdef PYVIZ
int32_t FireflyClustering::pyviz_edges_to_add_begin[MAX_NODES_FOR_EDGES];
int32_t FireflyClustering::pyviz_edges_to_add_end[MAX_NODES_FOR_EDGES];
int32_t FireflyClustering::pyviz_edges_to_add_num;

int32_t FireflyClustering::pyviz_edges_to_remove_begin[MAX_NODES_FOR_EDGES];
int32_t FireflyClustering::pyviz_edges_to_remove_end[MAX_NODES_FOR_EDGES];
int32_t FireflyClustering::pyviz_edges_to_remove_num;

bool FireflyClustering::pyviz_lock;

 void FireflyClustering::addNewEdge(int32_t begin, int32_t end) {
//	pyviz_lock = true;
	pyviz_edges_to_add_begin [pyviz_edges_to_add_num] = begin;
	pyviz_edges_to_add_end   [pyviz_edges_to_add_num] = end;
	pyviz_edges_to_add_num++;
	NS_ASSERT (pyviz_edges_to_add_num < MAX_NODES_FOR_EDGES - 1);
//	pyviz_lock = false;
}
 void FireflyClustering::removeEdge(int32_t begin, int32_t end) {
//	 pyviz_lock = true;
	pyviz_edges_to_remove_begin [pyviz_edges_to_remove_num] = begin;
	pyviz_edges_to_remove_end   [pyviz_edges_to_remove_num] = end;
	pyviz_edges_to_remove_num++;
	NS_ASSERT (pyviz_edges_to_remove_num < MAX_NODES_FOR_EDGES - 1);
//	pyviz_lock = false;
}
 int32_t FireflyClustering::getNewEdges_begin(){
	if (pyviz_edges_to_add_num>0)
		return (pyviz_edges_to_add_begin[pyviz_edges_to_add_num-1]);
	return -1;
}
 int32_t FireflyClustering::getNewEdges_end(){
	if (pyviz_edges_to_add_num>0) {
		pyviz_edges_to_add_num--;
		return (pyviz_edges_to_add_end[pyviz_edges_to_add_num]);
	}
	return -1;
}
 int32_t FireflyClustering::getEdgesToBeRemoved_begin(){
	if (pyviz_edges_to_remove_num>0)
		return (pyviz_edges_to_remove_begin[pyviz_edges_to_remove_num-1]);
	return -1;
}
 int32_t FireflyClustering::getEdgesToBeRemoved_end(){
	if (pyviz_edges_to_remove_num>0){
		pyviz_edges_to_remove_num--;
		return (pyviz_edges_to_remove_end[pyviz_edges_to_remove_num]);
	}
	return -1;
}
 bool FireflyClustering::PyVizLocked() {
	 return pyviz_lock;
 }
#endif

} //namespace ns3

