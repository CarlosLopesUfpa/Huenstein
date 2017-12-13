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

NS_LOG_COMPONENT_DEFINE ("Wifi_Test");

int
main (int argc, char *argv[])
{

  //uint16_t numberOfNodes = 1;
  double simTime = 20.0;
  // double distance = 60.0;
  double interPacketInterval = 0.01;
  int aux_energy = 0;
  int nAp = 1;
  int nSta = 3;

  double Vazao [nAp][1];
  double Lost [nAp][1];
  double Atraso [nAp][1];
  double Energia [nAp][1];

  double Rx = 99;


  
  // int nAll = 0;
  // nAll = nSta + nAp;
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
  for (size_t i = 0; i < 4; ++i)
    {
      all.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
    }

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
  apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes);


  wifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (true));
  staDevice = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);
      






  MobilityHelper mobilitywifiAp;
  mobilitywifiAp.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (30.0),
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
                                 "GridWidth", UintegerValue (nSta),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilitywifiSta.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobilitywifiSta.Install (wifiStaNodes);

  // uncomment the following to have athstats output
  // AthstatsHelper athstats;
  // athstats.EnableAthstats(enableCtsRts ? "rtscts-athstats-node" : "basic-athstats-node" , nodes);

  // uncomment the following to have pcap output
  // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);
  
  // Energy
      srand((unsigned)time(0));
      for (int l=0; l<nAp; ++l)
        {
          aux_energy = rand()%(100);
          Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
          Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

          energySource->SetInitialEnergy (aux_energy);
          energyModel->SetEnergySource (energySource);
          energySource->AppendDeviceEnergyModel (energyModel);
          energyModel->SetCurrentA (20);

          // aggregate energy source to node
          wifiApNodes.Get(l)->AggregateObject (energySource);
          // energy[l][0] = energySource;
          Energia [l][0] = aux_energy;
        }



  // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internetw;
  internetw.Install (all);
  
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface;
  apInterface = ipv4.Assign (apDevice);

  Ipv4InterfaceContainer staInterface;
  staInterface = ipv4.Assign (staDevice);
    // Ipv4InterfaceContainer apInterface;
  // apInterface = ipv4.Assign (apDevice);

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
  // Time interPacketInterval = Seconds (0.1);

  UdpEchoServerHelper apServer (port);
  ApplicationContainer serverApp = apServer.Install (wifiApNodes.Get (0));
  serverApp.Start (Seconds (0.0));
  serverApp.Stop (Seconds(10.0));
      
  UdpEchoClientHelper staClient (apInterface.GetAddress (0), port);
  staClient.SetAttribute ("MaxPackets", UintegerValue (Rx));
  staClient.SetAttribute ("Interval", TimeValue (Seconds (interPacketInterval))); //packets/s
  staClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer wifiApps;
  for(int u = 0; u<nSta; ++u){
      wifiApps.Add (staClient.Install (wifiStaNodes.Get (u)));
  }
  wifiApps.Start (Seconds (0.1));
  wifiApps.Stop (Seconds(9.9));
      
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

//NetAnim
   AnimationInterface anim ("wifi_test_1.xml"); // Mandatory
        
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
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.Install(all);

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

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
            
                  //Ipv4FlowClassifier::FiveTuple t = classifier1->FindFlow (i->first);
                  std::cout << " " <<std::endl;
                  std::cout << "Flow: " << i->first - 2 << ". (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                  std::cout << "  Received Packets: " << Rx << "\n";
                  std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
                  // std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
                  // std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
                  //std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  // std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
                  std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  Lost[0][0] =  i->second.txPackets - i->second.rxPackets;
                  std::cout << "  Lost Packets: " << Lost[0][0] << "\n";
                  Vazao[0][0] =  i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000;
                  std::cout << "  Throughput: " << Vazao[0][0] << " Mbps\n";
                  std::cout << "  Energy: " << Energia[0][0] << "\n";
                  Atraso[0][0] =  i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
                  std::cout << "  Atraso: " << Atraso[0][0] << "\n";
                
        }

  /*GtkConfigStore config;
  config.ConfigureAttributes();*/

  Simulator::Destroy();
  return 0;

}
