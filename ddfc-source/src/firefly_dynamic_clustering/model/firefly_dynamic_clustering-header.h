/*
 * FireflyBeaconHeader.h
 *
 *  Created on: 12/07/2012
 *      Author: fernandogielow
 */

#ifndef FIREFLYBEACONHEADER_H_
#define FIREFLYBEACONHEADER_H_

#include "ns3/header.h"
#include "ns3/enum.h"

namespace ns3 {



enum MessageType
{
  NONE = 0,
  FIREFLY_BEACON  = 1
};

/**
* \ingroup aodv
* \brief AODV types
*/
class FireflyTypeHeader : public Header
{
public:
  /// c-tor
	FireflyTypeHeader (MessageType t);
	FireflyTypeHeader ();

  ///\name Header serialization/deserialization
  //\{
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;
  //\}

  /// Return type
  MessageType Get () const { return m_type; }
  /// Check that type if valid
  bool IsValid () const { return m_valid; }
  bool operator== (FireflyTypeHeader const & o) const;
private:
  MessageType m_type;
  bool m_valid;
};



class FireflyBeaconHeader : public Header {
public:
	FireflyBeaconHeader(uint32_t src = 0,
			double individual_reading = 0,
			double aggregated_reading = 0,
			uint8_t number_of_aggregated = 0,
			uint8_t point = 0);
	static TypeId GetTypeId ();
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);

	void Print (std::ostream &os) const;
	uint32_t GetSerializedSize () const;
	TypeId GetInstanceTypeId () const;

	double getIndividualReading();
	double getAggregatedReading();
	uint8_t getNumberOfReadings();
	uint32_t getSrc();
	uint8_t getClusteringPoint();


private:
	uint32_t src_m;
	uint64_t individual_reading_m;
	uint64_t aggregated_reading_m;
	uint8_t  number_of_aggregated_m;
	uint8_t  clustering_point_m;

};

} /* namespace ns3 */
#endif /* FIREFLYBEACONHEADER_H_ */
