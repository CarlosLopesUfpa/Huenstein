/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *
 *
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
 *
 * Author: Billy Pinheiro <haquiticos@gmail.com>
 */

#include <fstream>
#include <string.h>

#include "ns3/lte-helper.h"
#include "ns3/wifi-module.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/netanim-module.h"
//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */
 
NS_LOG_COMPONENT_DEFINE ("Proposta_III_v2");

int
main (int argc, char *argv[])
{

//Configure P2P 
  // Create the Internet
	    NodeContainer p2pNodes;
	    p2pNodes.Create (2);

		PointToPointHelper p2p;
		p2p.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
		p2p.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));   //0.010

		NetDeviceContainer p2pDevices;
		p2pDevices = p2p.Install (p2pNodes);
		Ipv4AddressHelper addressp2p;
		addressp2p.SetBase ("7.0.1.1", "255.255.255.255");
		Ipv4InterfaceContainer p2pInterface;
		p2pInterface = addressp2p.Assign (p2pDevices);


		



//Configure WIFI
		// uint32_t payloadSize = 1472;//bytes
		// uint64_t simulationTime = 10; //seconds
		uint32_t nAp = 1;
		uint32_t nSta = 2;

		  NodeContainer wifiApNodes = p2pNodes.Get(1);
		  wifiApNodes.Create (nAp);
		  		
 		  NodeContainer wifiStaNodes;
		  wifiStaNodes.Create (nSta);

		  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
		  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
		  phy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
		  phy.SetChannel (channel.Create ());

		  WifiHelper wifi;
		  wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
		  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("HtMcs7"), "ControlMode", StringValue ("HtMcs0"));
		  WifiMacHelper mac;

		  NetDeviceContainer staDeviceA, staDeviceAa, apDeviceA;
		  Ssid ssid;

		  //Network A
		  ssid = Ssid ("network-A");
		  phy.Set ("ChannelNumber", UintegerValue (36));
		  mac.SetType ("ns3::StaWifiMac",
		               "Ssid", SsidValue (ssid));
		  apDeviceA = wifi.Install (phy, mac, wifiApNodes.Get (0));

		  mac.SetType ("ns3::ApWifiMac",
		               "Ssid", SsidValue (ssid),
		               "EnableBeaconJitter", BooleanValue (false));
		  staDeviceA = wifi.Install (phy, mac, wifiStaNodes.Get (0));
		  staDeviceAa = wifi.Install (phy, mac, wifiStaNodes.Get (1));
		  
		  /* Setting mobility model */
		  MobilityHelper mobilitywifi;
		  mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
		                                 "MinX", DoubleValue (10.0),
		                                 "MinY", DoubleValue (50.0),
		                                 "DeltaX", DoubleValue (20.0),
		                                 "DeltaY", DoubleValue (10.0),
		                                 "GridWidth", UintegerValue (3),
		                                 "LayoutType", StringValue ("RowFirst"));

		  mobilitywifi.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
		                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
		  mobilitywifi.Install (wifiApNodes);

		  mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
		                                 "MinX", DoubleValue (10.0),
		                                 "MinY", DoubleValue (60.0),
		                                 "DeltaX", DoubleValue (20.0),
		                                 "DeltaY", DoubleValue (10.0),
		                                 "GridWidth", UintegerValue (3),
		                                 "LayoutType", StringValue ("RowFirst"));

		  mobilitywifi.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		  mobilitywifi.Install (wifiStaNodes);
		  

		  /* Internet stack */
		  InternetStackHelper stack;
		  stack.Install (wifiApNodes);
		  stack.Install (wifiStaNodes);
		  stack.Install (p2pNodes);

		  Ipv4AddressHelper addresswifi;
		  addresswifi.SetBase ("7.0.1.3", "255.255.255.255");
		  Ipv4InterfaceContainer apInterfaceA;
		  apInterfaceA = addresswifi.Assign (apDeviceA);

		  Ipv4InterfaceContainer staInterfaceA;
		  staInterfaceA = addresswifi.Assign (staDeviceA);

		  Ipv4InterfaceContainer staInterfaceAa;
		  staInterfaceAa = addresswifi.Assign (staDeviceAa);












//Configure LTE
		uint16_t numberOfNodes = 1;
		// double simTime = 5.0;
		//double distance = 60.0;
		// Inter packet interval in ms
		// double interPacketInterval = 1;
		// double interPacketInterval = 100;
		//uint16_t port = 8000;


		Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
		//Ptr<EpcHelper>  epcHelper = CreateObject<EpcHelper> ();
		Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
		lteHelper->SetEpcHelper (epcHelper);
		lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");

		Ptr<Node> pgw = epcHelper->GetPgwNode ();


		// Create a single RemoteHost
		NodeContainer remoteHostContainer;
		remoteHostContainer.Create (1);
		Ptr<Node> remoteHost = remoteHostContainer.Get (0);
		InternetStackHelper internet;
		internet.Install (remoteHost);

		// Create the Internet
		PointToPointHelper p2ph;
		p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
		p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
		p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));   //0.010
		NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);


		Ipv4AddressHelper ipv4h;
		ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
		Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
		// interface 0 is localhost, 1 is the p2p device
		Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

		Ipv4StaticRoutingHelper ipv4RoutingHelper;
		Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
		remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

		
		NodeContainer enbNodes;
		enbNodes.Create(numberOfNodes);


		NodeContainer ueNodes;
		ueNodes.Add (p2pNodes.Get (0));
		ueNodes.Create(numberOfNodes);

		// Install Mobility Model
		// Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
		// for (uint16_t i = 0; i < numberOfNodes; i++)
		//   {
		//     positionAlloc->Add (Vector(distance * i, 0, 0));
		//   }

		MobilityHelper mobility;
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		                             "MinX", DoubleValue (10.0),
		                             "MinY", DoubleValue (10.0),
		                             "DeltaX", DoubleValue (20.0),
		                             "DeltaY", DoubleValue (10.0),
		                             "GridWidth", UintegerValue (3),
		                             "LayoutType", StringValue ("RowFirst"));
		mobility.Install(remoteHost);
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		                             "MinX", DoubleValue (10.0),
		                             "MinY", DoubleValue (20.0),
		                             "DeltaX", DoubleValue (20.0),
		                             "DeltaY", DoubleValue (10.0),
		                             "GridWidth", UintegerValue (3),
		                             "LayoutType", StringValue ("RowFirst"));
		mobility.Install(pgw);
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		                             "MinX", DoubleValue (10.0),
		                             "MinY", DoubleValue (30.0),
		                             "DeltaX", DoubleValue (20.0),
		                             "DeltaY", DoubleValue (10.0),
		                             "GridWidth", UintegerValue (3),
		                             "LayoutType", StringValue ("RowFirst"));
		mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
		//mobility.SetPositionAllocator(positionAlloc);
		mobility.Install(enbNodes);
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		                             "MinX", DoubleValue (10.0),
		                             "MinY", DoubleValue (40.0),
		                             "DeltaX", DoubleValue (20.0),
		                             "DeltaY", DoubleValue (10.0),
		                             "GridWidth", UintegerValue (3),
		                             "LayoutType", StringValue ("RowFirst"));
		mobility.Install(ueNodes);
















		// Install LTE Devices to the nodes
		NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
		NetDeviceContainer staLteDevs = lteHelper->InstallUeDevice (wifiStaNodes);


		// Install the IP stack on the UEs
		internet.Install (wifiStaNodes);
		Ipv4InterfaceContainer staIpIface;
		staIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (staLteDevs));



		// Assign IP address to UEs, and install applications

		Ptr<Node> wifiStaNode = wifiStaNodes.Get (0);
		// Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> staStaticRouting = ipv4RoutingHelper.GetStaticRouting (wifiStaNode->GetObject<Ipv4> ());
		staStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);

		// Attach one UE per eNodeB
		lteHelper->Attach (staLteDevs.Get(0), enbLteDevs.Get(0));

		//lteHelper->ActivateEpsBearer (ueLteDevs, EpsBearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT), EpcTft::Default ());

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  NS_LOG_INFO ("Create Applications.");
  
  // UdpServerHelper server(port);
  // ApplicationContainer apps = server.Install(remoteHostContainer.Get(0));
  // apps.Start (Seconds (9.0));
  // apps.Stop (Seconds (101.0));
  
  // UdpEchoClientHelper echoClient (ueIpIface.GetAddress (0), 9);
  // echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  // echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  // echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  // UdpClientHelper client (internetIpIfaces.GetAddress (1),port);
  // apps = client.Install (ueNodes.Get(0));
  // apps.Start (Seconds (10.0));
  // apps.Stop (Seconds (90.0));
   uint16_t dlPort = 1234;
   uint16_t ulPort = 2000;
   uint16_t otherPort = 3000;
   double interPacketInterval = 150.0;
   ApplicationContainer clientApps;
   ApplicationContainer serverApps;
   for (uint32_t u = 0; u < wifiStaNodes.GetN (); ++u)
    {
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
      serverApps.Add (dlPacketSinkHelper.Install (wifiStaNodes.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
      serverApps.Add (packetSinkHelper.Install (wifiStaNodes.Get(u)));

      UdpClientHelper dlClient (staIpIface.GetAddress (u), dlPort);
      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue(1000000));

      UdpClientHelper client (staIpIface.GetAddress (u), otherPort);
      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      client.SetAttribute ("MaxPackets", UintegerValue(1000000));

      clientApps.Add (dlClient.Install (remoteHost));
      clientApps.Add (ulClient.Install (wifiStaNodes.Get(u)));
      if (u+1 < wifiStaNodes.GetN ())
        {
          clientApps.Add (client.Install (wifiStaNodes.Get(u+1)));
        }
      else
        {
          clientApps.Add (client.Install (wifiStaNodes.Get(0)));
        }
    }








  
  //Metodo Animation

      AnimationInterface anim ("Proposta_III_v2.xml"); // Mandatory
      
      for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
      {
        anim.UpdateNodeDescription (ueNodes.Get(i), "ueNodes"); // Optional
        anim.UpdateNodeColor (ueNodes.Get(i), 191, 0, 30); // Coloração
      }
      
      for (uint32_t i = 0; i < enbNodes.GetN(); ++i)
      {
        anim.UpdateNodeDescription (enbNodes.Get(i), "enbNodes"); // Optional
        anim.UpdateNodeColor (enbNodes.Get(i), 255, 211, 0); // Coloração
      }
     
      for (uint32_t i = 0; i < wifiApNodes.GetN(); ++i)
      {
        anim.UpdateNodeDescription (wifiApNodes.Get(i), "AP"); // Optional
        anim.UpdateNodeColor (wifiApNodes.Get(i), 97, 154, 0); // Coloração
      }
      for (uint32_t i = 0; i < wifiStaNodes.GetN(); ++i)
      {
        anim.UpdateNodeDescription (wifiStaNodes.Get(i), "STA"); // Optional
        anim.UpdateNodeColor (wifiStaNodes.Get(i), 0, 132, 127); // Coloração
      }
      anim.EnablePacketMetadata (); // Optional

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(90));
  Simulator::Run ();         
  Simulator::Destroy ();
  
  
   NS_LOG_INFO ("Done.");
  return 0;

}