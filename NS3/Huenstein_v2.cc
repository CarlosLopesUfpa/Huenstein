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

  NodeContainer N1;
  N1.Add(mRelay.Get(0));
  N1.Add(client.Get(0));
  N1.Add(client.Get(1));
  
  NodeContainer N2;
  N2.Add(mRelay.Get(1));
  N2.Add(client.Get(2));
  N2.Add(client.Get(3));

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
  WifiMacHelper mac;

  Ssid ssid = Ssid ("ns-3");
   mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  
  NetDeviceContainer mRelayDevice0;
  mRelayDevice0 = wifi.Install (phy, mac, mRelay.Get(0));
  
  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));

  NetDeviceContainer clientDevice0;  
  NetDeviceContainer clientDevice1;
  clientDevice0 = wifi.Install (phy, mac, client.Get(0));
  clientDevice1 = wifi.Install (phy, mac, client.Get(1));

  WifiHelper wifi1;
  wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");
  WifiMacHelper mac1;

  Ssid ssid1 = Ssid ("ns-31");
   mac1.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "ActiveProbing", BooleanValue (false));
  
  NetDeviceContainer mRelayDevice1;
  mRelayDevice1 = wifi1.Install (phy, mac1, mRelay.Get(1));
  
  mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid1));

  NetDeviceContainer clientDevice2;
  NetDeviceContainer clientDevice3;
  clientDevice2 = wifi.Install (phy, mac1, client.Get(2));
  clientDevice3 = wifi.Install (phy, mac1, client.Get(3));



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

  InternetStackHelper stack1;
  stack1.Install (mRelay.Get(0));
  stack1.Install (client.Get(0));
  stack1.Install (client.Get(1));

  InternetStackHelper stack2;
  stack2.Install (mRelay.Get(1));
  stack2.Install (client.Get(2));
  stack2.Install (client.Get(3));

  Ipv4AddressHelper address1;
  address1.SetBase ("192.168.1.0", "255.255.255.0");
 
  Ipv4InterfaceContainer IntmR0;
  IntmR0 = address1.Assign (mRelayDevice0);
  Ipv4InterfaceContainer IntCli0;
  IntCli0 = address1.Assign (clientDevice0);
  Ipv4InterfaceContainer IntCli1;
  IntCli1 = address1.Assign (clientDevice1);
  
  Ipv4AddressHelper address2;
  Ipv4InterfaceContainer IntmR1;
  IntmR1 = address2.Assign (mRelayDevice1);
  Ipv4InterfaceContainer Intcli2;
  Intcli2 = address2.Assign (clientDevice2);
  Ipv4InterfaceContainer Intcli3;
  Intcli3 = address2.Assign (clientDevice3);

  UdpEchoServerHelper echoServer1 (9);

  ApplicationContainer serverApps1 = echoServer1.Install (mRelay.Get (0));
  serverApps1.Start (Seconds (1.0));
  serverApps1.Stop (Seconds (10.0));
  
  UdpEchoClientHelper echoClient0 (IntmR0.GetAddress (0), 9);
  echoClient0.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient0.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient0.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps0 = 
  echoClient0.Install (client.Get (0));
  clientApps0.Start (Seconds (2.0));
  clientApps0.Stop (Seconds (10.0));
  
  UdpEchoClientHelper echoClient1 (IntmR0.GetAddress (0), 9);
  echoClient1.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = 
  echoClient1.Install (client.Get (1));
  clientApps1.Start (Seconds (2.0));
  clientApps1.Stop (Seconds (10.0));

  UdpEchoServerHelper echoServer2 (9);

  ApplicationContainer serverApps2 = echoServer2.Install (mRelay.Get (1));
  serverApps2.Start (Seconds (1.0));
  serverApps2.Stop (Seconds (10.0));
  
  UdpEchoClientHelper echoClient2 (IntmR1.GetAddress (1), 9);
  echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps2 = 
  echoClient2.Install (client.Get (2));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));
  
  UdpEchoClientHelper echoClient3 (IntmR1.GetAddress (1), 9);
  echoClient3.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient3.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps3 = 
  echoClient3.Install (client.Get (3));
  clientApps3.Start (Seconds (2.0));
  clientApps3.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //-------------------------Metodo-Animation-------------------------

      AnimationInterface anim ("Huenstein_v2.xml"); // Mandatory
      
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
