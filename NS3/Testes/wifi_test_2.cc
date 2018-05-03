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
 * Authors: 
 Carlos Lopes <carloslopesufpa@gmail.com>
 Elizeu Conde <elizeu.g.f.conde@gmail.com>
 Marcos Seruffo <marcos.seruffo@gmail.com>
 */


#include "ns3/wifi-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/netanim-module.h"

#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"

#include "ns3/gnuplot.h"
#include "ns3/flow-monitor-module.h"
#include <ns3/flow-monitor-helper.h>

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <cstdio>

using namespace ns3;

double** create(int rows, int columns){
    double** table = new double*[rows];
    for(int i = 0; i < rows; i++) {
        table[i] = new double[columns]; 
        for(int j = 0; j < columns; j++){ 
        table[i][j] = 0; 
        }// sample set value;    
    }
    return table;
}

NS_LOG_COMPONENT_DEFINE ("MasterCode");


int main (int argc, char *argv[]) {
// Step 1: Reconhecimento da rede.

//WIFI
//Configure WIFI
		uint32_t payloadSize = 1024;//bytes
		uint64_t simulationTime = 10; //seconds
		int nAp = 1;
		int col = 1;
		int nSta = 2;

//Variáveis para receber dados do FlowMonitor
		// double soma_Vazao[nAp][1];
		double** Vazao = create(nAp, col);

		// double soma_Jitter[nAp][1];
		//double** Jitter = create(nAp, col);
		// double pct_perdido = 0;

		// double soma_Delay[nAp][1];
		double** Atraso = create(nAp, col);

		  NodeContainer wifiApNodes;
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
		                                 "MinX", DoubleValue (15.0),
		                                 "MinY", DoubleValue (10.0),
		                                 "DeltaX", DoubleValue (20.0),
		                                 "DeltaY", DoubleValue (10.0),
		                                 "GridWidth", UintegerValue (3),
		                                 "LayoutType", StringValue ("RowFirst"));

		  mobilitywifi.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		  mobilitywifi.Install (wifiApNodes);

		  mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
		                                 "MinX", DoubleValue (10.0),
		                                 "MinY", DoubleValue (20.0),
		                                 "DeltaX", DoubleValue (10.0),
		                                 "DeltaY", DoubleValue (10.0),
		                                 "GridWidth", UintegerValue (3),
		                                 "LayoutType", StringValue ("RowFirst"));

		  mobilitywifi.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
		                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
		  mobilitywifi.Install (wifiStaNodes);
		  

		  Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
		  Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

		  energySource->SetInitialEnergy (300);
		  energyModel->SetEnergySource (energySource);
		  energySource->AppendDeviceEnergyModel (energyModel);
		  energyModel->SetCurrentA (20);

		  // aggregate energy source to node
		  wifiApNodes.Get(0)->AggregateObject (energySource);
		  // energy[l][0] = energySource;


		  /* Internet stack */
		  InternetStackHelper stack;
		  stack.Install (wifiApNodes);
		  stack.Install (wifiStaNodes);
		  
		  Ipv4AddressHelper addresswifi;
		  addresswifi.SetBase ("192.168.1.0", "255.255.255.0");
		  Ipv4InterfaceContainer apInterfaceA;
		  apInterfaceA = addresswifi.Assign (apDeviceA);

		  Ipv4InterfaceContainer staInterfaceA;
		  staInterfaceA = addresswifi.Assign (staDeviceA);

		  Ipv4InterfaceContainer staInterfaceAa;
		  staInterfaceAa = addresswifi.Assign (staDeviceAa);


//APLICATIONS
		  uint16_t port = 9;
		  

		  UdpEchoServerHelper serverA (port);
		  ApplicationContainer serverAppA = serverA.Install (wifiApNodes.Get (0));
		  serverAppA.Start (Seconds (0.0));
		  serverAppA.Stop (Seconds (simulationTime - 1));
  
  		  Time interPacketInterval = Seconds (0.01);

		  UdpEchoClientHelper clientA (apInterfaceA.GetAddress (0), port);
		  clientA.SetAttribute ("MaxPackets", UintegerValue (100));
		  clientA.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  clientA.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  ApplicationContainer clientAppA = clientA.Install (wifiStaNodes.Get (0));
		  clientAppA.Start (Seconds (1.0));
		  clientAppA.Stop (Seconds (simulationTime - 1));

		  UdpEchoClientHelper clientAa (apInterfaceA.GetAddress (0), port);
		  clientAa.SetAttribute ("MaxPackets", UintegerValue (100));
		  clientAa.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  clientAa.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  ApplicationContainer clientAppAa = clientAa.Install (wifiStaNodes.Get (1));
		  clientAppAa.Start (Seconds (1.0));
		  clientAppAa.Stop (Seconds (simulationTime - 1));


std::cout << " " <<std::endl;
std::cout << " " <<std::endl;
std::cout << " " <<std::endl;
//Metodo Animation

	      AnimationInterface anim ("wifi_test_2.xml"); // Mandatory
	      
	      for (uint32_t i = 0; i < wifiStaNodes.GetN(); ++i)
	      {
	        anim.UpdateNodeDescription (wifiStaNodes.Get(i), "STA"); // Optional
	        anim.UpdateNodeColor (wifiStaNodes.Get(i), 255, 0, 0); // Coloração
	      }
	      
	      for (uint32_t i = 0; i < wifiApNodes.GetN(); ++i)
	      {
	        anim.UpdateNodeDescription (wifiApNodes.Get(i), "AP"); // Optional
	        anim.UpdateNodeColor (wifiApNodes.Get(i), 255, 255, 0); // Coloração
	      }
	      anim.EnablePacketMetadata (); // Optiona
	      anim.EnableIpv4RouteTracking ("routingtable-wireless.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional
		  anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); //Optional
		  anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); //Optional

  		  FlowMonitorHelper flowmon;
  		  Ptr<FlowMonitor> monitor = flowmon.InstallAll();

		  Simulator::Stop (Seconds (simulationTime));
		  Simulator::Run ();


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
              if (i->first > 0)
                {
                  //Ipv4FlowClassifier::FiveTuple t = classifier1->FindFlow (i->first);
                  std::cout << " " <<std::endl;
                  std::cout << "Flow: " << i->first - 2 << ". (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                  //std::cout << "  Received Packets: " << Rx << "\n";
                  std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
                  // std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
                  // std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
                  //std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  // std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
                  std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  //Lost[0][0] =  i->second.txPackets - i->second.rxPackets;
                  //std::cout << "  Lost Packets: " << Lost[0][0] << "\n";
                  Vazao[0][0] =  i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000;
                  std::cout << "  Throughput: " << Vazao[0][0] << " Mbps\n";
                  //std::cout << "  Energy: " << Energia[0][0] << "\n";
                  Atraso[0][0] =  i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
                  std::cout << "  Atraso: " << Atraso[0][0] << "\n";
                }
        }



		  Simulator::Destroy ();
		  return 0;

}