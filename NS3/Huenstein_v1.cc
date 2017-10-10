/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/stats-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"

// Default Network Topology
//
//   
//     10.1.0.0
//   n1        n2
//
//
//
//  *    *    *    *
//  |    |    |    |
// n5   n6   n7   n0
//   Wifi 10.1.1.1 


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Huenstein");

int
main (int argc, char *argv[])
{
	int nMr = 2;
	int nCl = 4;

CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer client;
  client.Create (nCl);

  NodeContainer mRelay;
  mRelay.Create (nMr);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac;
  Ssid ssid = Ssid ("ns-3-ssid");
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  NetDeviceContainer clientDevice;
  NetDeviceContainer mRelayDevice;

  clientDevice = wifi.Install (phy, mac, client);
  mRelayDevice = wifi.Install (phy, mac, mRelay);


  MobilityHelper mobilityMR;

  mobilityMR.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (5.0),
                                 "MinY", DoubleValue (5.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (4),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilityMR.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityMR.Install (mRelay);

  MobilityHelper mobilityCL;  
  mobilityCL.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (7.0),
                                 "MinY", DoubleValue (15.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (4),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilityCL.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobilityCL.Install (client);

  InternetStackHelper stack;
  stack.Install (client);
  stack.Install (mRelay);

  Ipv4AddressHelper address;

  address.SetBase ("1.1.0.1", "255.255.0.0");
  Ipv4InterfaceContainer IntmR;
  IntmR = address.Assign (mRelayDevice);

  address.SetBase ("1.1.1.3", "255.255.255.255");
  Ipv4InterfaceContainer IntCli;
  IntCli = address.Assign (clientDevice);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (mRelay.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (IntmR.GetAddress (nMr), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = 
  echoClient.Install (client.Get (nCl));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //-------------------------Metodo-Animation-------------------------

      AnimationInterface anim ("Huenstein_v1.xml"); // Mandatory
      
      for (uint32_t i = 0; i < mRelay.GetN(); ++i)
      {
        anim.UpdateNodeDescription (mRelay.Get(i), "Multi_Relay"); // Optional
        anim.UpdateNodeColor (mRelay.Get(i), 255, 0, 0); // Coloração
      }
      
      for (uint32_t i = 0; i < client.GetN(); ++i)
      {
        anim.UpdateNodeDescription (client.Get(i), "Clients"); // Optional
        anim.UpdateNodeColor (client.Get(i), 255, 255, 0); // Coloração
      }
      anim.EnablePacketMetadata (); // Optional

  Simulator::Stop (Seconds (10.0));

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
