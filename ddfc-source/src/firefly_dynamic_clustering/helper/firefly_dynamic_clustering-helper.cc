/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "firefly_dynamic_clustering-helper.h"
#include "ns3/names.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ColorHandler.h"

namespace ns3 {

void FireflyClusteringHelper::addReading(Ptr<Node> node,double reading,double time) {
	NS_ASSERT(node != NULL);
	Ptr<FireflyClustering> fc = NULL;
	fc = node->GetObject<FireflyClustering>(FireflyClustering::GetTypeId());

	// FIXME: will work only if firefly app is on slot 0..... see below comented code :P

	/*for (uint32_t i = 0; i< node->GetNApplications(); i++) {
		Ptr<Application> app = node->GetApplication(i);
		std::cout << "+" << app->GetTypeId().GetName();
		if (app->GetTypeId().GetName().compare(FireflyClustering::GetTypeId().GetName()) == 0)
			fc = app->GetObject<FireflyClustering>(FireflyClustering::GetTypeId());
			std::cout << "WIN";
	}*/

	fc = node->GetApplication(0)->GetObject<FireflyClustering>(FireflyClustering::GetTypeId());

	if (fc != NULL) {
		fc->AddReading(time,reading);
	} else {
		std::cout << "WARNING: could not add a reading.....\n";
	}
}

FireflyClusteringHelper::FireflyClusteringHelper (/*Ipv4Address remote*/)
{
  m_factory.SetTypeId ("ns3::FireflyClustering");
//  m_factory.Set ("Verbose", UintegerValue(5));
}

void
FireflyClusteringHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
FireflyClusteringHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
FireflyClusteringHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
FireflyClusteringHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
FireflyClusteringHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<FireflyClustering> app = m_factory.Create<FireflyClustering> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3

