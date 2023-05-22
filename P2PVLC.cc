#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/vlc-module.h"

using namespace ns3;

int main()
{
  // Create nodes
  NodeContainer nodes;
  nodes.Create(2);

  // Install VLC devices
  VLCNetDeviceHelper vlc;
  vlc.SetAttribute("DataRate", DataRateValue(DataRate("100Mbps"))); // Set the data rate
  vlc.Install(nodes);

  // Set up point-to-point link
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("100Mbps")); // Set the link data rate
  NetDeviceContainer devices = p2p.Install(nodes);

  // Install Internet stack
  InternetStackHelper internet;
  internet.Install(nodes);

  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign(devices);

  // Create VLC applications
  Ptr<VlcTxNetDevice> txDevice = DynamicCast<VlcTxNetDevice>(devices.Get(0));
  Ptr<VlcRxNetDevice> rxDevice = DynamicCast<VlcRxNetDevice>(devices.Get(1));
  VlcAppHelper app;
  app.SetAttribute("DataRate", DataRateValue(DataRate("100Mbps"))); // Set the application data rate
  app.Install(txDevice, rxDevice, interfaces.GetAddress(1));

  // Start simulation
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
