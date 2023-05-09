#include "ns3/core-module.h"
#include "ns3/vlc-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("LiFiStarTopologyNetwork");

int main(int argc, char *argv[]) {

  LogComponentEnable("LiFiStarTopologyNetwork", LOG_LEVEL_INFO);

  // Create the server node
  NodeContainer serverNode;
  serverNode.Create(1);

  // Create the six client nodes
  NodeContainer clientNodes;
  clientNodes.Create(6);

  // Create the VLC channel
  VlcChannelHelper channel;
  channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss("ns3::VlcPropagationLossModel");
  //specify the modulation type
  channel.SetMode("OOK");
  Ptr<VlcChannel> vlcChannel = channel.Create();

  
  VlcHelper vlc;
  // Install VLC devices for the server
  Ptr<VlcRxNetDevice> serverRxDevice = vlc.Install(serverNode.Get(0), vlcChannel);
   // Install VLC devices Client devices
  Ptr<VlcTxNetDevice> client1TxDevice = vlc.Install(clientNodes.Get(0), vlcChannel);
  Ptr<VlcTxNetDevice> client2TxDevice = vlc.Install(clientNodes.Get(1), vlcChannel);
  Ptr<VlcTxNetDevice> client3TxDevice = vlc.Install(clientNodes.Get(2), vlcChannel);
  Ptr<VlcTxNetDevice> client4TxDevice = vlc.Install(clientNodes.Get(3), vlcChannel);
  Ptr<VlcTxNetDevice> client5TxDevice = vlc.Install(clientNodes.Get(4), vlcChannel);
  Ptr<VlcTxNetDevice> client6TxDevice = vlc.Install(clientNodes.Get(5), vlcChannel);

  // Assign IP addresses 
  InternetStackHelper internet;
  internet.InstallAll();

  Ipv4AddressHelper serverAddress;
  serverAddress.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer serverInterfaces = serverAddress.Assign(serverRxDevice);

  Ipv4AddressHelper clientAddress;
  clientAddress.SetBase("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer client1Interfaces = clientAddress.Assign(client1TxDevice);
  Ipv4InterfaceContainer client2Interfaces = clientAddress.Assign(client2TxDevice);
  Ipv4InterfaceContainer client3Interfaces = clientAddress.Assign(client3TxDevice);
  Ipv4InterfaceContainer client4Interfaces = clientAddress.Assign(client4TxDevice);
  Ipv4InterfaceContainer client5Interfaces = clientAddress.Assign(client5TxDevice);
  Ipv4InterfaceContainer client6Interfaces = clientAddress.Assign(client6TxDevice);

  // Create a application to send data from each client to the server
  uint16_t port = 9;

  for (uint32_t i = 0; i < clientNodes.GetN(); ++i) {
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(serverInterfaces.GetAddress(0), port));
    onoff.SetAttribute("PacketSize", UintegerValue(1024));
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer apps = onoff.Install(clientNodes.Get(0));
    apps.Start(Seconds(1.0));
    apps.Stop(Seconds(10.0));
                                              
    // Run the simulation
    Simulator::Run();
    Simulator::Destroy();
 
  return 0;
  
                                              
