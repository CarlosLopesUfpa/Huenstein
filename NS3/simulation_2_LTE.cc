/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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
 * Author: Jaume Nin <jaume.nin@cttc.cat>
 */

#include "ns3/lte-helper.h"
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


#include "ns3/propagation-module.h"

#include <ns3/buildings-propagation-loss-model.h>

#include "ns3/antenna-model.h"

#include "ns3/isotropic-antenna-model.h"

#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-module.h"
#include <time.h>


#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"

//#include "ns3/gtk-config-store.h"

using namespace ns3;

/**
 * Sample simulation script for LTE+EPC. It instantiates several eNodeB,
 * attaches one UE per eNodeB starts a flow for each UE to  and from a remote host.
 * It also  starts yet another flow between each UE pair.
 */

NS_LOG_COMPONENT_DEFINE ("Lte_Test");

int
main (int argc, char *argv[])
{

  uint16_t numberOfNodes = 1;

  double PacketInterval = 0.2;
  double MaxPacketSize = 1024;
  double maxPacketCount = 10000;

  double simTime = 300;
  double Rx = 0;

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  lteHelper->SetAttribute ("PathlossModel", 
                           StringValue ("ns3::FriisPropagationLossModel"));
  lteHelper->SetEnbAntennaModelType ("ns3::IsotropicAntennaModel");
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  Ptr<Node> pgw = epcHelper->GetPgwNode ();

   // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  // interface 0 is localhost, 1 is the p2p device
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(numberOfNodes);
  ueNodes.Create(numberOfNodes);

  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                     "MinX", DoubleValue (5.0),
                                     "MinY", DoubleValue (5.0),
                                     "DeltaX", DoubleValue (10.0),
                                     "DeltaY", DoubleValue (10.0),
                                     "GridWidth", UintegerValue (1),
                                     "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install(ueNodes);

  MobilityHelper mobility1;
  mobility1.SetPositionAllocator ("ns3::GridPositionAllocator",
                                     "MinX", DoubleValue (15.0),
                                     "MinY", DoubleValue (5.0),
                                     "DeltaX", DoubleValue (10.0),
                                     "DeltaY", DoubleValue (10.0),
                                     "GridWidth", UintegerValue (1),
                                     "LayoutType", StringValue ("RowFirst"));

  mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility1.Install(enbNodes);


  
  MobilityHelper mobility2;
  mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                     "MinX", DoubleValue (25.0),
                                     "MinY", DoubleValue (5.0),
                                     "DeltaX", DoubleValue (10.0),
                                     "DeltaY", DoubleValue (10.0),
                                     "GridWidth", UintegerValue (4),
                                     "LayoutType", StringValue ("RowFirst"));

  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install(pgw);

  MobilityHelper mobility3;
  mobility3.SetPositionAllocator ("ns3::GridPositionAllocator",
                                     "MinX", DoubleValue (35.0),
                                     "MinY", DoubleValue (5.0),
                                     "DeltaX", DoubleValue (10.0),
                                     "DeltaY", DoubleValue (10.0),
                                     "GridWidth", UintegerValue (4),
                                     "LayoutType", StringValue ("RowFirst"));

  mobility3.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility3.Install(remoteHost);

  // Install LTE Devices to the nodes
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
  internet.Install (ueNodes);
  Ipv4InterfaceContainer ueIpIface;
  ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
  // Assign IP address to UEs, and install applications
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      Ptr<Node> ueNode = ueNodes.Get (u);
      // Set the default gateway for the UE
      Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
      ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
    }

  // Attach one UE per eNodeB
  for (uint16_t i = 0; i < numberOfNodes; i++)
      {
        lteHelper->Attach (ueLteDevs.Get(i), enbLteDevs.Get(i));
        // side effect: the default EPS bearer will be activated
      }


  // Install and start applications on UEs and remote host
  uint16_t dlPort = 1234;
  uint16_t ulPort = 2000;
  uint16_t otherPort = 3000;
  ApplicationContainer clientApps;
  ApplicationContainer serverApps;
  for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
    {
      ++ulPort;
      ++otherPort;
      PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), dlPort));
      PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), ulPort));
      PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), otherPort));
      serverApps.Add (dlPacketSinkHelper.Install (ueNodes.Get(u)));
      serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
      serverApps.Add (packetSinkHelper.Install (ueNodes.Get(u)));

      UdpClientHelper dlClient (ueIpIface.GetAddress (u), dlPort);
      dlClient.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
      dlClient.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
      dlClient.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
      ulClient.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
      ulClient.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

      UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
      client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
      client.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
      client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

      clientApps.Add (dlClient.Install (remoteHost));
      clientApps.Add (ulClient.Install (ueNodes.Get(u)));
      if (u+1 < ueNodes.GetN ())
        {
          clientApps.Add (client.Install (ueNodes.Get(u+1)));
        }
      else
        {
          clientApps.Add (client.Install (ueNodes.Get(0)));
        }
    }
  serverApps.Start (Seconds (0.1));
  serverApps.Stop (Seconds (simTime-1));

  clientApps.Start (Seconds (0.1));
  clientApps.Stop (Seconds (simTime-1));

  lteHelper->EnableTraces ();
 
 // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

//Install NetAnim
   AnimationInterface anim ("simulation_2/lte_flow/simulation_2_lte.xml"); // Mandatory
        
        for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
        {
          anim.UpdateNodeDescription (ueNodes.Get(i), "ueNodes"); // Optional
          anim.UpdateNodeColor (ueNodes.Get(i), 255, 0, 0); // Coloração
        }
        
        for (uint32_t i = 0; i < enbNodes.GetN(); ++i)
        {
          anim.UpdateNodeDescription (enbNodes.Get(i), "enbNodes"); // Optional
          anim.UpdateNodeColor (enbNodes.Get(i), 255, 255, 0); // Coloração
        }
        anim.EnablePacketMetadata ();



  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

// 10. Print per flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
        {
          
          Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          if (i->first < 2)
          {
            std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
            std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
            std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
            std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
            std::cout << "\n";
            std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
            std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
            std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
            std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
            Rx = i->second.rxPackets;
            std::cout << "\n";
            std::cout << "\n";
            std::cout<<"Duration  : "<<(i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())<<std::endl;
            std::cout << "  LTE Rx Packets: " << Rx << "\n";
          }
        }
  Simulator::Destroy();
  return 0;
}