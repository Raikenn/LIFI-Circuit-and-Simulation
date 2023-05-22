#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/vlcnew.h"
#include "vlcChannelHelper.h"
#include "vlcDeviceHelper.h"

using namespace ns3;
using namespace std;
using namespace vlc;

int main (int argc, char *argv[])
{
  // Enable VLC logs
  LogComponentEnable ("VlcTransmitter", LOG_LEVEL_ALL);
  LogComponentEnable ("VlcReceiver", LOG_LEVEL_ALL);

  // Create nodes
  NodeContainer nodes;
  nodes.Create (2);

  // Install VLC devices
  vlcChannelHelper vlcChannelHelper;
  vlcChannelHelper.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  vlcChannelHelper.SetPropagationLoss ("ns3::LogDistancePropagationLossModel");
  vlcChannelHelper.SetPathlossModel ("ns3::FixedPathLossModel");
  vlcChannelHelper.Install (nodes);

  // Set up point-to-point link
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("10ms"));
  NetDeviceContainer devices = p2p.Install (nodes);

  // Assign IP addresses
  InternetStackHelper stack;
  stack.Install (nodes);
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  // Create transmitter
  Ptr<vlc_NetDeviceTX> transmitter = CreateObject<vlc_NetDeviceTX> ();
  transmitter.SetPosition (0.0, 0.0, 1.5); // Set transmitter position (x, y, z)
  transmitter.SetSpectrumModel ("ns3::ConstantSpectrumModel");
  transmitter.SetSignalPower (1.0); // Set signal power (in Watts)

  // Create receiver
  Ptr<vlc_NetDeviceRX> receiver = CreateObject<vlc_NetDeviceRX> ();
  receiver.SetPosition (10.0, 0.0, 1.5); // Set receiver position (x, y, z)

  // Connect transmitter and receiver
  Ptr<VLCPropagationLossModel> channel = vlcChannelHelper.GetChannel (0);
  transmitter.SetChannel (channel);
  receiver.SetChannel (channel);

  // Set up applications
  OnOffHelper onOffHelper ("ns3::UdpSocketFactory", Address ());
  onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"));
  onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
  onOffHelper.SetAttribute ("DataRate", StringValue ("1Mbps"));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (1024));

  // Install applications
  ApplicationContainer appContainer = onOffHelper.Install (nodes.Get (0));
  appContainer.Start (Seconds (1.0));
  appContainer.Stop (Seconds (10.0));

  // Enable packet capture for receiver
  receiver.EnablePacketCapture ("vlc-receiver.pcap", Seconds (1.0));

 // Run simulation
  Simulator::Stop (Seconds (10.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
