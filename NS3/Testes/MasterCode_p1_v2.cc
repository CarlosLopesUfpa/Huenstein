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

#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"

#include <ns3/boolean.h>
#include <string>
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/gnuplot.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <cstdlib>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MasterCode");


//Proposta Conexão por P2P

// Métodos QOS

void ThroughputMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, Gnuplot2dDataset dataset){

  double tempThroughput = 0.0;
  monitor->CheckForLostPackets(); 
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats){ 
      tempThroughput = (stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds() - stats->second.timeFirstTxPacket.GetSeconds())/1024/1024);
      
      //if(stats->first == 11) {
      dataset.Add((double)Simulator::Now().GetSeconds(), (double)tempThroughput);
      //}
  }
  
  //Tempo que será iniciado
  Simulator::Schedule(Seconds(1),&ThroughputMonitor, fmhelper, monitor, dataset);
}

void DelayMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, Gnuplot2dDataset dataset1){
  
  double delay = 0.0;
  monitor->CheckForLostPackets(); 
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats){ 
      //Ipv4FlowClassifier::FiveTuple fiveTuple = classifier->FindFlow (stats->first);
      delay = stats->second.delaySum.GetSeconds ();
      
      //if(stats->first == 11) {
      dataset1.Add((double)Simulator::Now().GetSeconds(), (double)delay);
      //}    
    }
  
  //Tempo que será iniciado
  Simulator::Schedule(Seconds(1),&DelayMonitor, fmhelper, monitor, dataset1);
}

void LostPacketsMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, Gnuplot2dDataset dataset2){
  
  double packets = 0.0;
  monitor->CheckForLostPackets(); 
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats){ 
      //Ipv4FlowClassifier::FiveTuple fiveTuple = classifier->FindFlow (stats->first);
      packets = stats->second.lostPackets;
      
      //if(stats->first == 2) {
      dataset2.Add((double)Simulator::Now().GetSeconds(), (double)packets);
      //}
    }
  
  //Tempo que será iniciado
  Simulator::Schedule(Seconds(1),&LostPacketsMonitor, fmhelper, monitor, dataset2);
}

void JitterMonitor (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor, Gnuplot2dDataset dataset3){
  
  double jitter = 0.0;
  monitor->CheckForLostPackets(); 
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats){ 
      //Ipv4FlowClassifier::FiveTuple fiveTuple = classifier->FindFlow (stats->first);
      jitter = stats->second.jitterSum.GetSeconds ();
      
      //if(stats->first == 2) {
      dataset3.Add((double)Simulator::Now().GetSeconds(), (double)jitter);
      //}
    }
  
  //Tempo que será iniciado
  Simulator::Schedule(Seconds(1),&LostPacketsMonitor, fmhelper, monitor, dataset3);
}

void ImprimeMetricas (FlowMonitorHelper* fmhelper, Ptr<FlowMonitor> monitor){
  double tempThroughput = 0.0;
  monitor->CheckForLostPackets(); 
  std::map<FlowId, FlowMonitor::FlowStats> flowStats = monitor->GetFlowStats();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
       
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats){ 
      // A tuple: Source-ip, destination-ip, protocol, source-port, destination-port
      Ipv4FlowClassifier::FiveTuple fiveTuple = classifier->FindFlow (stats->first);
      
      std::cout<<"Flow ID: " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
      std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
      std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
      std::cout<<"Duration: " <<stats->second.timeLastRxPacket.GetSeconds() - stats->second.timeFirstTxPacket.GetSeconds()<<std::endl;
      std::cout<<"Last Received Packet: "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
      tempThroughput = (stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds() - stats->second.timeFirstTxPacket.GetSeconds())/1024/1024);
      std::cout<<"Throughput: "<< tempThroughput <<" Mbps"<<std::endl;
      std::cout<< "Delay: " << stats->second.delaySum.GetSeconds () << std::endl;
      std::cout<< "LostPackets: " << stats->second.lostPackets << std::endl;
      std::cout<< "Jitter: " << stats->second.jitterSum.GetSeconds () << std::endl;
      //std::cout<<"Last Received Packet: "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds ---->" << "Throughput: " << tempThroughput << " Kbps" << std::endl;
      std::cout<<"------------------------------------------"<<std::endl;
    }
  
  //Tempo que será iniciado
  Simulator::Schedule(Seconds(1),&ImprimeMetricas, fmhelper, monitor);
}


		void avalParam(int nAp);
int main (int argc, char *argv[])
{
// Step 1: Reconhecimento da rede.

//WIFI
//Configure WIFI
		uint32_t payloadSize = 4096;//bytes
		uint64_t simulationTime = 10; //seconds
		uint32_t nAp = 1;
		uint32_t nSta = 2;

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
		  wifiApNodes.Get (0)->AggregateObject (energySource);



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
  
  		  Time interPacketInterval = Seconds (0.1);

		  UdpEchoClientHelper clientA (apInterfaceA.GetAddress (0), port);
		  clientA.SetAttribute ("MaxPackets", UintegerValue (1));
		  clientA.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  clientA.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  ApplicationContainer clientAppA = clientA.Install (wifiStaNodes.Get (0));
		  clientAppA.Start (Seconds (1.0));
		  clientAppA.Stop (Seconds (simulationTime - 1));

		  UdpEchoClientHelper clientAa (apInterfaceA.GetAddress (0), port);
		  clientAa.SetAttribute ("MaxPackets", UintegerValue (10000));
		  clientAa.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  clientAa.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  ApplicationContainer clientAppAa = clientAa.Install (wifiStaNodes.Get (1));
		  clientAppAa.Start (Seconds (1.0));
		  clientAppAa.Stop (Seconds (simulationTime - 1));

//LÓGICA DE SELEÇÃO
      avalParam(nAp);

//Monitor de fluxo
  Ptr<FlowMonitor> monitor;
  FlowMonitorHelper fmhelper;   
  monitor = fmhelper.InstallAll();

// captura QOS
  string tipo = "home/carl/repos/ns3-allinone/ns-3-dev/";


    //Throughput
    string vazao = tipo + "FlowVSThroughput";
    string graphicsFileName        = vazao + ".png";
    string plotFileName            = vazao + ".plt";
    string plotTitle               = "Flow vs Throughput";
    string dataTitle               = "Throughput";

    Gnuplot gnuplot (graphicsFileName);
    gnuplot.SetTitle (plotTitle);
    gnuplot.SetTerminal ("png");
    gnuplot.SetLegend ("Flow", "Throughput");

    Gnuplot2dDataset dataset;
    dataset.SetTitle (dataTitle);
    dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

    //Delay
    string delay = tipo + "FlowVSDelay";
    string graphicsFileName1        = delay + ".png";
    string plotFileName1            = delay + ".plt";
    string plotTitle1               = "Flow vs Delay";
    string dataTitle1               = "Delay";

    Gnuplot gnuplot1 (graphicsFileName1);
    gnuplot1.SetTitle (plotTitle1);
    gnuplot1.SetTerminal ("png");
    gnuplot1.SetLegend ("Flow", "Delay");

    Gnuplot2dDataset dataset1;
    dataset1.SetTitle (dataTitle1);
    dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);

    //LostPackets
    string lost = tipo + "FlowVSLostPackets";
    string graphicsFileName2        = lost + ".png";
    string plotFileName2            = lost + ".plt";
    string plotTitle2               = "Flow vs LostPackets";
    string dataTitle2               = "LostPackets";

    Gnuplot gnuplot2 (graphicsFileName2);
    gnuplot2.SetTitle (plotTitle2);
    gnuplot2.SetTerminal ("png");
    gnuplot2.SetLegend ("Flow", "LostPackets");

    Gnuplot2dDataset dataset2;
    dataset2.SetTitle (dataTitle2);
    dataset2.SetStyle (Gnuplot2dDataset::LINES_POINTS);

    //Jitter
    string jitter = tipo + "FlowVSJitter";
    string graphicsFileName3        = jitter + ".png";
    string plotFileName3            = jitter + ".plt";
    string plotTitle3               = "Flow vs Jitter";
    string dataTitle3               = "Jitter";

    Gnuplot gnuplot3 (graphicsFileName3);
    gnuplot3.SetTitle (plotTitle3);
    gnuplot3.SetTerminal ("png");
    gnuplot3.SetLegend ("Flow", "Jitter");

    Gnuplot2dDataset dataset3;
    dataset3.SetTitle (dataTitle3);
    dataset3.SetStyle (Gnuplot2dDataset::LINES_POINTS);

    //Chama classe de captura do fluxo
    ThroughputMonitor (&fmhelper, monitor, dataset);
    DelayMonitor (&fmhelper, monitor, dataset1);
    LostPacketsMonitor (&fmhelper, monitor, dataset2);
    JitterMonitor (&fmhelper, monitor, dataset3);

//Captura de pacotes
  //phy.EnablePcap ("video/resultados/lte_wifi/wifi_lte", apDevices.Get (0));
  phy.EnablePcapAll ("home/carl/repos/ns3-allinone/ns-3-dev/");

//Metodo Animation

		      AnimationInterface anim ("Master_p1.xml"); // Mandatory
		      
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
		  Simulator::Stop (Seconds (simulationTime));
		  Simulator::Run ();

ImprimeMetricas (&fmhelper, monitor);

    //Throughput
    gnuplot.AddDataset (dataset);
    std::ofstream plotFile (plotFileName.c_str()); // Abre o arquivo.
    gnuplot.GenerateOutput (plotFile);    //Escreve no arquivo.
    plotFile.close ();        // fecha o arquivo.
    //Delay
    gnuplot1.AddDataset (dataset1);
    std::ofstream plotFile1 (plotFileName1.c_str()); // Abre o arquivo.
    gnuplot1.GenerateOutput (plotFile1);    //Escreve no arquivo.
    plotFile1.close ();        // fecha o arquivo.
    //LostPackets
    gnuplot2.AddDataset (dataset2);
    std::ofstream plotFile2 (plotFileName2.c_str()); // Abre o arquivo.
    gnuplot2.GenerateOutput (plotFile2);    //Escreve no arquivo.
    plotFile2.close ();        // fecha o arquivo.
    //Jitter
    gnuplot3.AddDataset (dataset3);
    std::ofstream plotFile3 (plotFileName3.c_str()); // Abre o arquivo.
    gnuplot3.GenerateOutput (plotFile3);    //Escreve no arquivo.
    plotFile3.close ();        // fecha o arquivo.



//Cria arquivo para Monitor de fluxo
  monitor->SerializeToXmlFile("flow.xml", true, true);
		  Simulator::Destroy ();
		  return 0;
}