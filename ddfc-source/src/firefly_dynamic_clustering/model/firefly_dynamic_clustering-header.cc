/*
 * FireflyBeaconHeader.cc
 *
 *  Created on: 12/07/2012
 *      Author: fernandogielow
 */

#include "firefly_dynamic_clustering-header.h"

namespace ns3 {

#ifndef DOUBLE_PRECISION
#define DOUBLE_PRECISION 100
#endif

double int_64_to_double(int64_t n) {
	return ((double)n)/DOUBLE_PRECISION;
}

int64_t double_to_int_64(double n) {
	return n*DOUBLE_PRECISION;
}




NS_OBJECT_ENSURE_REGISTERED (FireflyTypeHeader);

FireflyTypeHeader::FireflyTypeHeader (MessageType t) :
  m_type (t), m_valid (true)
{
}

FireflyTypeHeader::FireflyTypeHeader () :
  m_type (NONE), m_valid (true)
{
}

TypeId
FireflyTypeHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::FireflyTypeHeader")
    .SetParent<Header> ()
    .AddConstructor<FireflyTypeHeader> ()
  ;
  return tid;
}

TypeId
FireflyTypeHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
FireflyTypeHeader::GetSerializedSize () const
{
  return 1;
}

void
FireflyTypeHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 ((uint8_t) m_type);
}

uint32_t
FireflyTypeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t type = i.ReadU8 ();
  m_valid = true;
  switch (type)
    {
    case NONE:
    	break; /* ADDED LATER */
    case FIREFLY_BEACON:
	  {
		m_type = (MessageType) type;
		break;
	  }
    default:
      m_valid = false;
      break;
    }
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
FireflyTypeHeader::Print (std::ostream &os) const
{
  switch (m_type)
    {
    case FIREFLY_BEACON:
      {
        os << "BEACON";
        break;
      }
    default:
      os << "UNKNOWN_TYPE";
      break;
    }
}

bool
FireflyTypeHeader::operator== (FireflyTypeHeader const & o) const
{
  return (m_type == o.m_type && m_valid == o.m_valid);
}




NS_OBJECT_ENSURE_REGISTERED (FireflyBeaconHeader);

TypeId
FireflyBeaconHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::FireflyBeaconHeader")
    .SetParent<Header> ()
    .AddConstructor<FireflyBeaconHeader> ()
  ;
  return tid;
}

FireflyBeaconHeader::FireflyBeaconHeader(uint32_t src, double individual_reading, double aggregated_reading, uint8_t number_of_aggregated, uint8_t point) :
		src_m(src),
		individual_reading_m(double_to_int_64(individual_reading)),
		aggregated_reading_m(double_to_int_64(aggregated_reading)),
		number_of_aggregated_m(number_of_aggregated),
		clustering_point_m(point) {
}

void FireflyBeaconHeader::Serialize (Buffer::Iterator start) const {
	  start.WriteU32(src_m);
	  start.WriteU64(individual_reading_m);
	  start.WriteU64(aggregated_reading_m);
	  start.WriteU8(number_of_aggregated_m);
	  start.WriteU8(clustering_point_m);
}

uint32_t FireflyBeaconHeader::Deserialize (Buffer::Iterator start) {
	  Buffer::Iterator i = start;

	  src_m =                  i.ReadU32();
	  individual_reading_m =   i.ReadU64();
	  aggregated_reading_m =   i.ReadU64();
	  number_of_aggregated_m = i.ReadU8();
	  clustering_point_m =     i.ReadU8();

	  uint32_t dist = i.GetDistanceFrom (start);
	  NS_ASSERT (dist == GetSerializedSize ());
	  return dist;
}

void FireflyBeaconHeader::Print (std::ostream &os) const {
	os << "individual_reading " << individual_reading_m << " aggregated_reading " << aggregated_reading_m << " number_os_aggregated " << number_of_aggregated_m;
}

uint32_t FireflyBeaconHeader::GetSerializedSize () const{
	return 22; // size / 8
}

TypeId
FireflyBeaconHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

double FireflyBeaconHeader::getIndividualReading() {
	return int_64_to_double(individual_reading_m);
}

double FireflyBeaconHeader::getAggregatedReading() {
	return int_64_to_double(aggregated_reading_m);
}

uint8_t FireflyBeaconHeader::getNumberOfReadings(){
	return number_of_aggregated_m;
}

uint8_t FireflyBeaconHeader::getClusteringPoint() {
	return clustering_point_m;
}

uint32_t FireflyBeaconHeader::getSrc() {
	return src_m;
}


} /* namespace ns3 */
