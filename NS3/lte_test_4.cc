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

NS_LOG_COMPONENT_DEFINE ("EpcFirstExample");

int
main (int argc, char *argv[])
{

  uint16_t numberOfNodes = 3;
  double simTime = 20.0;
  // double distance = 60.0;
  double interPacketInterval = 10;
  bool useCa = false;

  int aux_energy = 0;

  double Vazao [numberOfNodes][1];
  double Lost [numberOfNodes][1];
  double Atraso [numberOfNodes][1];
  double Energia [numberOfNodes][1];
  double Rx[numberOfNodes][1];

  // Command line arguments
  CommandLine cmd;
  cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
  cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
  // cmd.AddValue("distance", "Distance between eNBs [m]", distance);
  cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
  cmd.AddValue("useCa", "Whether to use carrier aggregation.", useCa);
  cmd.Parse(argc, argv);

  if (useCa)
   {
     Config::SetDefault ("ns3::LteHelper::UseCa", BooleanValue (useCa));
     Config::SetDefault ("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue (2));
     Config::SetDefault ("ns3::LteHelper::EnbComponentCarrierManager", StringValue ("ns3::RrComponentCarrierManager"));
   }

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
  lteHelper->SetEpcHelper (epcHelper);

  ConfigStore inputConfig;
  inputConfig.ConfigureDefaults();

  // parse again so you can override default values from the command line
  cmd.Parse(argc, argv);

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

  // Install Mobility Model
  // Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  // for (uint16_t i = 0; i < numberOfNodes; i++)
  //   {
  //     positionAlloc->Add (Vector(distance * i, 0, 0));
  //   }
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


    //Energy
      srand((unsigned)time(0));
      for (int l=0; l<numberOfNodes; ++l)
        {
          aux_energy = rand()%(100);
          Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
          Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

          energySource->SetInitialEnergy (aux_energy);
          energyModel->SetEnergySource (energySource);
          energySource->AppendDeviceEnergyModel (energyModel);
          energyModel->SetCurrentA (20);

          // aggregate energy source to node
          ueNodes.Get(l)->AggregateObject (energySource);
          // energy[l][0] = energySource;
          Energia[l][0] = aux_energy;
        }



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
      dlClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      dlClient.SetAttribute ("MaxPackets", UintegerValue(10000000));

      UdpClientHelper ulClient (remoteHostAddr, ulPort);
      ulClient.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      ulClient.SetAttribute ("MaxPackets", UintegerValue(10000000));

      UdpClientHelper client (ueIpIface.GetAddress (u), otherPort);
      client.SetAttribute ("Interval", TimeValue (MilliSeconds(interPacketInterval)));
      client.SetAttribute ("MaxPackets", UintegerValue(10000000));

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
  serverApps.Start (Seconds (0.01));
  serverApps.Stop (Seconds (10.0));

  clientApps.Start (Seconds (0.01));
  clientApps.Stop (Seconds (10.0));

  lteHelper->EnableTraces ();
  // Uncomment to enable PCAP tracing
  //p2ph.EnablePcapAll("lena-epc-first");




 // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.Install(ueNodes);

  // 10. Print per flow statistics
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
        {
          
          // first 2 FlowIds are for ECHO apps, we don't want to display them
          //
          // Duration for throughput measurement is 9.0 seconds, since
          //   StartTime of the OnOffApplication is at about "second 1"
          // and
          //   Simulator::Stops at "second 10".
         
              Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
              if ( t.destinationAddress == "1.0.0.2")
                {
                  Rx[0][0] = i->second.rxPackets;
                }
        }

//WIFI Simulation
  int nAp = 1;
  int nSta = 2;
  int nAll = 0;

  nAll = nSta + nAp;
  // int nAp = nNode - 1;


  // 1. Create 3 nodes

      NodeContainer wifiApNodes;
      wifiApNodes.Create (nAp);
          
      NodeContainer wifiStaNodes;
      wifiStaNodes.Create (nSta);

      NodeContainer all;
      all.Add(wifiApNodes);
      all.Add(wifiStaNodes);

  // 2. Place nodes somehow, this is required by every wireless simulation
  // for (size_t i = 0; i < nAll; ++i)
  //   {
  //     all.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
  //   }

  // 3. Create propagation loss matrix
  Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel> ();
  lossModel->SetDefaultLoss (200); // set default loss to 200 dB (no link)
  for(int u = 0; u < nSta; ++u){
  lossModel->SetLoss (wifiApNodes.Get (0)->GetObject<MobilityModel> (), wifiStaNodes.Get (u)->GetObject<MobilityModel> (), 50); // set symmetric loss 0 <-> 1 to 50 dB
  }

  // 4. Create & setup wifi channel
  Ptr<YansWifiChannel> wifiChannel = CreateObject <YansWifiChannel> ();
  wifiChannel->SetPropagationLossModel (lossModel);
  wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());

  // 5. Install wireless devices
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue ("DsssRate2Mbps"),
                                "ControlMode",StringValue ("DsssRate1Mbps"));
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  wifiPhy.SetChannel (wifiChannel);

  //Adaptação
  
  WifiMacHelper wifiMac;

  NetDeviceContainer staDevice, apDevice;
  Ssid ssid;

  //Network A
  ssid = Ssid ("network-A");
  wifiPhy.Set ("ChannelNumber", UintegerValue (36));
  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid));
  apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes;


  wifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (true));
  staDevice = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);
      






  MobilityHelper mobilitywifiAp;
  mobilitywifiAp.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (15.0),
                                 "MinY", DoubleValue (20.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (1),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilitywifiAp.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilitywifiAp.Install (wifiApNodes);


  MobilityHelper mobilitywifiSta;
  mobilitywifiSta.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (20.0),
                                 "MinY", DoubleValue (25.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (2),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilitywifiSta.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobilitywifiSta.Install (wifiStaNodes);

  // uncomment the following to have athstats output
  // AthstatsHelper athstats;
  // athstats.EnableAthstats(enableCtsRts ? "rtscts-athstats-node" : "basic-athstats-node" , nodes);

  // uncomment the following to have pcap output
  // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);
  
  // //Energy
  //     srand((unsigned)time(0));
  //     for (int l=0; l<nNode; ++l)
  //       {
  //         aux_energy = rand()%(100);
  //         Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
  //         Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

  //         energySource->SetInitialEnergy (aux_energy);
  //         energyModel->SetEnergySource (energySource);
  //         energySource->AppendDeviceEnergyModel (energyModel);
  //         energyModel->SetCurrentA (20);

  //         // aggregate energy source to node
  //         nodes.Get(l)->AggregateObject (energySource);
  //         // energy[l][0] = energySource;
  //         Energia[l][0] = aux_energy;
  //       }



  // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internetw;
  internetw.Install (all);
  
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  ipv4.Assign (all);

  Ipv4InterfaceContainer apInterface;
  apInterface = ipv4.Assign (apDevice);

  // 7. Install applications: two CBR streams each saturating the channel
  // ApplicationContainer cbrApps;
  // uint16_t cbrPort = 12345;
  // OnOffHelper onOffHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), cbrPort));
  // onOffHelper.SetAttribute ("PacketSize", UintegerValue (1400));
  // onOffHelper.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  // onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  // // flow 1:  node 0 -> node 1


  // // flow 2:  node 2 -> node 1
  // /** \internal
  //  * The slightly different start times and data rates are a workaround
  //  * for \bugid{388} and \bugid{912}
  //  */

  //   for(int u = 0; u<nNode; ++u){
  //     if(u!=1){
  //       onOffHelper.SetAttribute ("DataRate", StringValue ("3001100bps"));
  //       onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (10.1)));
  //       cbrApps.Add (onOffHelper.Install (nodes.Get (u)));
  //     }
  //   }

  /** \internal
   * We also use separate UDP applications that will send a single
   * packet before the CBR flows start.
   * This is a workaround for the lack of perfect ARP, see \bugid{187}
   */
  uint16_t  port = 9;
  Time interPacketInterval = Seconds (0.1);

  UdpEchoServerHelper apServer (port);
  ApplicationContainer serverApp = apServer.Install (wifiApNodes.Get (0));
  serverApp.SetAttribute ("StartTime", TimeValue (Seconds (10.1)));
  serverApp.SetAttribute ("StopTime", TimeValue (Seconds (20.0)));
      
  UdpEchoClientHelper staClient (apInterface.GetAddress (0), port);
  staClient.SetAttribute ("MaxPackets", UintegerValue (Rx[0][0]));
  staClient.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
  staClient.SetAttribute ("PacketSize", UintegerValue (1024));


  
  ApplicationContainer wifiApps;

  // again using different start times to workaround Bug 388 and Bug 912
  for(int u = 0; u<nSta; ++u){
    if(u!=1){
      staClient.SetAttribute ("StartTime", TimeValue (Seconds (10.1)));
      staClient.SetAttribute ("StopTime", TimeValue (Seconds (20.0)));
      wifiApps.Add (staClient.Install (wifiStaNodes.Get (u)));
    }
  }



   AnimationInterface anim ("lte_test.xml"); // Mandatory
        
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
        for (uint32_t i = 0; i < wifiApNodes.GetN(); ++i)
        {
          anim.UpdateNodeDescription (wifiApNodes.Get(i), "Ap"); // Optional
          anim.UpdateNodeColor (wifiApNodes.Get(i), 0, 0, 0); // Coloração
        }
        for (uint32_t i = 0; i < wifiStaNodes.GetN(); ++i)
        {
          anim.UpdateNodeDescription (wifiStaNodes.Get(i), "Sta"); // Optional
          anim.UpdateNodeColor (wifiStaNodes.Get(i), 0, 0, 255); // Coloração
        }
        anim.EnablePacketMetadata (); // Optiona

  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon1;
  Ptr<FlowMonitor> monitor1 = flowmon1.InstallAll ();

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  // 10. Print per flow statistics
  monitor1->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier1 = DynamicCast<Ipv4FlowClassifier> (flowmon1.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats1 = monitor1->GetFlowStats ();

      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats1.begin (); i != stats1.end (); ++i)
        {
          
          // first 2 FlowIds are for ECHO apps, we don't want to display them
          //
          // Duration for throughput measurement is 9.0 seconds, since
          //   StartTime of the OnOffApplication is at about "second 1"
          // and
          //   Simulator::Stops at "second 10".
         
              Ipv4FlowClassifier::FiveTuple t = classifier1->FindFlow (i->first);
              if ( t.sourceAddress == "10.0.0.1")
                {
                  // Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
                  std::cout << " " <<std::endl;
                  std::cout << "Flow: " << i->first - 2 << ". (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                  std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
                  // std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
                  // std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
                  //std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  // std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
                  Rx[0][0] = i->second.rxPackets;
                  std::cout << "  Received Packets: " << Rx[0][0] << "\n";
                  Lost[0][0] =  i->second.txPackets - i->second.rxPackets;
                  std::cout << "  Lost Packets: " << Lost[0][0] << "\n";
                  Vazao[0][0] =  i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000;
                  std::cout << "  Throughput: " << Vazao[0][0] << " Mbps\n";
                  std::cout << "  Energy: " << Energia[0][0] << "\n";
                  Atraso[0][0] =  i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
                  std::cout << "  Atraso: " << Atraso[0][0] << "\n";
                }
        }
  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy();
  return 0;

}


