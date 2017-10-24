/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 Sébastien Deronne
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
 * Author: Sébastien Deronne <sebastien.deronne@gmail.com>
 */

#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
// Bibliotecas para o FlowMonitor
#include "ns3/gnuplot.h"
#include "ns3/flow-monitor-module.h"
#include <ns3/flow-monitor-helper.h>

// This is an example that illustrates how 802.11n aggregation is configured.
// It defines 4 independant Wi-Fi networks (working on different channels).
// Each network contains one access point and one station. Each station
// continously transmits data packets to its respective AP.
//
// Network topology (numbers in parentheses are channel numbers):
//
//  Network A (36)   Network B (40)   Network C (44)   Network D (48)
//   *      *          *      *         *      *          *      *
//   |      |          |      |         |      |          |      |
//  AP A   STA A      AP B   STA B     AP C   STA C      AP D   STA D
//
// The aggregation parameters are configured differently on the 4 stations:
// - station A uses default aggregation parameter values (A-MSDU disabled, A-MPDU enabled with maximum size of 65 kB);
// - station B doesn't use aggregation (both A-MPDU and A-MSDU are disabled);
// - station C enables A-MSDU (with maximum size of 8 kB) but disables A-MPDU;
// - station D uses two-level aggregation (A-MPDU with maximum size of 32 kB and A-MSDU with maximum size of 4 kB).
//
// Packets in this simulation aren't marked with a QosTag so they
// are considered belonging to BestEffort Access Class (AC_BE).
//
// The user can select the distance between the stations and the APs and can enable/disable the RTS/CTS mechanism.
// Example: ./waf --run "wifi-aggregation --distance=10 --enableRts=0 --simulationTime=20"
//
// The output prints the throughput measured for the 4 cases/networks decribed above. When default aggregation parameters are enabled, the
// maximum A-MPDU size is 65 kB and the throughput is maximal. When aggregation is disabled, the throughput is about the half of the
// physical bitrate as in legacy wifi networks. When only A-MSDU is enabled, the throughput is increased but is not maximal, since the maximum
// A-MSDU size is limited to 7935 bytes (whereas the maximum A-MPDU size is limited to 65535 bytes). When A-MSDU and A-MPDU are both enabled
// (= two-level aggregation), the throughput is slightly smaller than the first scenario since we set a smaller maximum A-MPDU size.
//
// When the distance is increased, the frame error rate gets higher, and the output shows how it affects the throughput for the 4 networks.
// Even through A-MSDU has less overheads than A-MPDU, A-MSDU is less robust against transmission errors than A-MPDU. When the distance is
// augmented, the throughput for the third scenario is more affected than the throughput obtained in other networks.

using namespace ns3;

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon,Gnuplot2dDataset DataSet);
void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2);
void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3);

NS_LOG_COMPONENT_DEFINE ("Huenstein_v3_1");

int main (int argc, char *argv[])
{
  uint32_t payloadSize = 1472;//bytes
  uint64_t simulationTime = 10; //seconds
  uint32_t nSta = 4;
  uint32_t nAp = 8;

  NodeContainer wifiStaNodes;
  wifiStaNodes.Create (nSta);
  NodeContainer wifiApNodes;
  wifiApNodes.Create (nAp);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  phy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  phy.SetChannel (channel.Create ());

  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("HtMcs7"), "ControlMode", StringValue ("HtMcs0"));
  WifiMacHelper mac;

  NetDeviceContainer staDeviceA, staDeviceB, staDeviceC, staDeviceD, apDeviceA, apDeviceAa, apDeviceB, apDeviceBb, apDeviceC, apDeviceCc, apDeviceD, apDeviceDd;
  Ssid ssid;

  //Network A
  ssid = Ssid ("network-A");
  phy.Set ("ChannelNumber", UintegerValue (36));
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid));
  staDeviceA = wifi.Install (phy, mac, wifiStaNodes.Get (0));

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false));
  apDeviceA = wifi.Install (phy, mac, wifiApNodes.Get (0));
  apDeviceAa = wifi.Install (phy, mac, wifiApNodes.Get (1));

  //Network B
  ssid = Ssid ("network-B");
  phy.Set ("ChannelNumber", UintegerValue (40));
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "BE_MaxAmpduSize", UintegerValue (0)); //Disable A-MPDU

  staDeviceB = wifi.Install (phy, mac, wifiStaNodes.Get (1));

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false));
  apDeviceB = wifi.Install (phy, mac, wifiApNodes.Get (2));
  apDeviceBb = wifi.Install (phy, mac, wifiApNodes.Get (3));

  //Network C
  ssid = Ssid ("network-C");
  phy.Set ("ChannelNumber", UintegerValue (44));
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "BE_MaxAmpduSize", UintegerValue (0), //Disable A-MPDU
               "BE_MaxAmsduSize", UintegerValue (7935)); //Enable A-MSDU with the highest maximum size allowed by the standard (7935 bytes)

  staDeviceC = wifi.Install (phy, mac, wifiStaNodes.Get (2));

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false));
  apDeviceC = wifi.Install (phy, mac, wifiApNodes.Get (4));
  apDeviceCc = wifi.Install (phy, mac, wifiApNodes.Get (5));

  //Network D
  ssid = Ssid ("network-D");
  phy.Set ("ChannelNumber", UintegerValue (48));
  mac.SetType ("ns3::StaWifiMac",
               "Ssid", SsidValue (ssid),
               "BE_MaxAmpduSize", UintegerValue (32768), //Enable A-MPDU with a smaller size than the default one
               "BE_MaxAmsduSize", UintegerValue (3839)); //Enable A-MSDU with the smallest maximum size allowed by the standard (3839 bytes)

  staDeviceD = wifi.Install (phy, mac, wifiStaNodes.Get (3));

  mac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false));
  apDeviceD = wifi.Install (phy, mac, wifiApNodes.Get (6));
  apDeviceDd = wifi.Install (phy, mac, wifiApNodes.Get (7));

  /* Setting mobility model */
  MobilityHelper mobilitySta;

  mobilitySta.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (7.5),
                                 "MinY", DoubleValue (5.0),
                                 "DeltaX", DoubleValue (10.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (4),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilitySta.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilitySta.Install (wifiStaNodes);

  MobilityHelper mobilityAp;  
  mobilityAp.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (5.0),
                                 "MinY", DoubleValue (10.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (8),
                                 "LayoutType", StringValue ("RowFirst"));

  mobilityAp.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobilityAp.Install (wifiApNodes);

  /* Internet stack */
  InternetStackHelper stack;
  stack.Install (wifiApNodes);
  stack.Install (wifiStaNodes);

  Ipv4AddressHelper address;

  address.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer StaInterfaceA;
  StaInterfaceA = address.Assign (staDeviceA);

  Ipv4InterfaceContainer ApInterfaceA;
  ApInterfaceA = address.Assign (apDeviceA);
  Ipv4InterfaceContainer ApInterfaceAa;
  ApInterfaceAa = address.Assign (apDeviceAa);

  address.SetBase ("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer StaInterfaceB;
  StaInterfaceB = address.Assign (staDeviceB);

  Ipv4InterfaceContainer ApInterfaceB;
  ApInterfaceB = address.Assign (apDeviceB);
  Ipv4InterfaceContainer ApInterfaceBb;
  ApInterfaceBb = address.Assign (apDeviceBb);

  address.SetBase ("192.168.3.0", "255.255.255.0");
  Ipv4InterfaceContainer StaInterfaceC;
  StaInterfaceC = address.Assign (staDeviceC);
  
  Ipv4InterfaceContainer ApInterfaceC;
  ApInterfaceC = address.Assign (apDeviceC);
  Ipv4InterfaceContainer ApInterfaceCc;
  ApInterfaceCc = address.Assign (apDeviceCc);

  address.SetBase ("192.168.4.0", "255.255.255.0");
  Ipv4InterfaceContainer StaInterfaceD;
  StaInterfaceD = address.Assign (staDeviceD);
  
  Ipv4InterfaceContainer ApInterfaceD;
  ApInterfaceD = address.Assign (apDeviceD);
  Ipv4InterfaceContainer ApInterfaceDd;
  ApInterfaceDd = address.Assign (apDeviceDd);

  /* Setting applications */
  uint16_t port = 9;
  

  UdpServerHelper serverA (port);
  ApplicationContainer serverAppA = serverA.Install (wifiStaNodes.Get (0));
  serverAppA.Start (Seconds (0.0));
  serverAppA.Stop (Seconds (simulationTime - 1));

  UdpClientHelper clientA (StaInterfaceA.GetAddress (0), port);
  clientA.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientA.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientA.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppA = clientA.Install (wifiApNodes.Get (0));
  clientAppA.Start (Seconds (1.0));
  clientAppA.Stop (Seconds (simulationTime - 1));

  UdpClientHelper clientAa (StaInterfaceA.GetAddress (0), port);
  clientAa.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientAa.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientAa.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppAa = clientAa.Install (wifiApNodes.Get (1));
  clientAppAa.Start (Seconds (1.0));
  clientAppAa.Stop (Seconds (simulationTime - 1));


  UdpServerHelper serverB (port);
  ApplicationContainer serverAppB = serverB.Install (wifiStaNodes.Get (1));
  serverAppB.Start (Seconds (0.0));
  serverAppB.Stop (Seconds (simulationTime - 1));
  
  UdpClientHelper clientB (StaInterfaceB.GetAddress (0), port);
  clientB.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientB.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientB.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppB = clientB.Install (wifiApNodes.Get (2));
  clientAppB.Start (Seconds (1.0));
  clientAppB.Stop (Seconds (simulationTime - 1));

  UdpClientHelper clientBb (StaInterfaceB.GetAddress (0), port);
  clientBb.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientBb.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientBb.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppBb = clientBb.Install (wifiApNodes.Get (3));
  clientAppBb.Start (Seconds (1.0));
  clientAppBb.Stop (Seconds (simulationTime - 1));


  UdpServerHelper serverC (port);
  ApplicationContainer serverAppC = serverC.Install (wifiStaNodes.Get (2));
  serverAppC.Start (Seconds (0.0));
  serverAppC.Stop (Seconds (simulationTime - 1));

  UdpClientHelper clientC (StaInterfaceC.GetAddress (0), port);
  clientC.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientC.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientC.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppC = clientC.Install (wifiApNodes.Get (4));
  clientAppC.Start (Seconds (1.0));
  clientAppC.Stop (Seconds (simulationTime - 1));
  
  UdpClientHelper clientCc (StaInterfaceC.GetAddress (0), port);
  clientCc.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientCc.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientCc.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppCc = clientCc.Install (wifiApNodes.Get (5));
  clientAppCc.Start (Seconds (1.0));
  clientAppCc.Stop (Seconds (simulationTime - 1));


  UdpServerHelper serverD (port);
  ApplicationContainer serverAppD = serverD.Install (wifiStaNodes.Get (3));
  serverAppD.Start (Seconds (0.0));
  serverAppD.Stop (Seconds (simulationTime - 1));
  
  UdpClientHelper clientD (StaInterfaceD.GetAddress (0), port);
  clientD.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientD.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientD.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppD = clientD.Install (wifiApNodes.Get (6));
  clientAppD.Start (Seconds (1.0));
  clientAppD.Stop (Seconds (simulationTime - 1));

  UdpClientHelper clientDd (StaInterfaceD.GetAddress (0), port);
  clientDd.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
  clientDd.SetAttribute ("Interval", TimeValue (Time ("0.00002"))); //packets/s
  clientDd.SetAttribute ("PacketSize", UintegerValue (payloadSize));
  ApplicationContainer clientAppDd = clientDd.Install (wifiApNodes.Get (7));
  clientAppDd.Start (Seconds (1.0));
  clientAppDd.Stop (Seconds (simulationTime - 1));


//FLOW-MONITOR


//-----------------FlowMonitor-THROUGHPUT----------------

    std::string fileNameWithNoExtension = "FlowVSThroughput_Huenstein";
    std::string graphicsFileName        = fileNameWithNoExtension + ".png";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "Flow vs Throughput";
    std::string dataTitle               = "Throughput";

    // Instantiate the plot and set its title.
    Gnuplot gnuplot (graphicsFileName);
    gnuplot.SetTitle (plotTitle);

    // Make the graphics file, which the plot file will be when it
    // is used with Gnuplot, be a PNG file.
    gnuplot.SetTerminal ("png");

    // Set the labels for each axis.
    gnuplot.SetLegend ("Flow", "Throughput");

     
   Gnuplot2dDataset dataset;
   dataset.SetTitle (dataTitle);
   dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

  //flowMonitor declaration
  FlowMonitorHelper fmHelper;
  Ptr<FlowMonitor> allMon = fmHelper.InstallAll();
  // call the flow monitor function
  ThroughputMonitor(&fmHelper, allMon, dataset); 

//-----------------FlowMonitor-JITTER--------------------

    std::string fileNameWithNoExtension2 = "FlowVSJitter_Huenstein";
    std::string graphicsFileName2      = fileNameWithNoExtension2 + ".png";
    std::string plotFileName2        = fileNameWithNoExtension2 + ".plt";
    std::string plotTitle2           = "Flow vs Jitter";
    std::string dataTitle2           = "Jitter";

    Gnuplot gnuplot2 (graphicsFileName2);
    gnuplot2.SetTitle(plotTitle2);

  gnuplot2.SetTerminal("png");

  gnuplot2.SetLegend("Flow", "Jitter");

  Gnuplot2dDataset dataset2;
  dataset2.SetTitle(dataTitle2);
  dataset2.SetStyle(Gnuplot2dDataset::LINES_POINTS);

  //FlowMonitorHelper fmHelper;
  //Ptr<FlowMonitor> allMon = fmHelper.InstallAll();

  JitterMonitor(&fmHelper, allMon, dataset2);

//-----------------FlowMonitor-DELAY---------------------

  std::string fileNameWithNoExtension3 = "FlowVSDelay_Huenstein";
  std::string graphicsFileName3      = fileNameWithNoExtension3 + ".png";
  std::string plotFileName3        = fileNameWithNoExtension3 + ".plt";
  std::string plotTitle3       = "Flow vs Delay";
  std::string dataTitle3       = "Delay";

      Gnuplot gnuplot3 (graphicsFileName3);
      gnuplot3.SetTitle(plotTitle3);

      gnuplot3.SetTerminal("png");

      gnuplot3.SetLegend("Flow", "Delay");

      Gnuplot2dDataset dataset3;
      dataset3.SetTitle(dataTitle3);
      dataset3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

      DelayMonitor(&fmHelper, allMon, dataset3);





 //Metodo Animation

      AnimationInterface anim ("Huenstein_v3_1.xml"); // Mandatory
      
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
      anim.EnablePacketMetadata (); // Optional
  Simulator::Stop (Seconds (simulationTime));
  Simulator::Run ();

//Gnuplot ...continued
  gnuplot.AddDataset (dataset);
  // Open the plot file.
  std::ofstream plotFile (plotFileName.c_str());
  // Write the plot file.
  gnuplot.GenerateOutput (plotFile);
  // Close the plot file.
  plotFile.close ();

//---ContJITTER---

  gnuplot2.AddDataset(dataset2);;
  std::ofstream plotFile2 (plotFileName2.c_str());
  gnuplot2.GenerateOutput(plotFile2);
  plotFile2.close();

  //---ContDelay---

  gnuplot3.AddDataset(dataset3);;
  std::ofstream plotFile3 (plotFileName3.c_str());
  gnuplot3.GenerateOutput(plotFile3);
  plotFile3.close();

  Simulator::Destroy ();
  return 0;
}


//-------------------------Metodo-VAZÃO---------------------------

  void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon,Gnuplot2dDataset DataSet)
    {
          double localThrou=0;
      std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
      Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
      {
                        if(stats->first == 1){//IFFFFFFFFFFFFFFFFFFFFFFF
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        std::cout<<"Flow ID     : " << stats->first <<" ; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
        std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
        std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
              std::cout<<"Duration    : "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
        std::cout<<"Last Received Packet  : "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
        std::cout<<"Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps"<<std::endl;
              localThrou=(stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024);
        // updata gnuplot data
              DataSet.Add((double)Simulator::Now().GetSeconds(),(double) localThrou);
        std::cout<<"---------------------------------------------------------------------------"<<std::endl;
    }//IFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF  
  }
        Simulator::Schedule(Seconds(1),&ThroughputMonitor, fmhelper, flowMon,DataSet);
     //if(flowToXml)
        {
    flowMon->SerializeToXmlFile ("ThroughputMonitor_Huenstein.xml", true, true);
        }

    }

//-------------------------Metodo-JINTTER-------------------------

  double atraso1=0;
  void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2)
  {
         double localJitter=0;
         double atraso2 =0;

         std::map<FlowId, FlowMonitor::FlowStats> flowStats2 = flowMon->GetFlowStats();
         Ptr<Ipv4FlowClassifier> classing2 = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
         for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats2 = flowStats2.begin(); stats2 != flowStats2.end(); ++stats2)
         {
                 if(stats2->first == 1){//IFFFFFFFFFFF
              Ipv4FlowClassifier::FiveTuple fiveTuple2 = classing2->FindFlow (stats2->first);
      std::cout<<"Flow ID : "<< stats2->first <<";"<< fiveTuple2.sourceAddress <<"------>" <<fiveTuple2.destinationAddress<<std::endl;
      std::cout<<"Tx Packets = " << stats2->second.txPackets<<std::endl;
      std::cout<<"Rx Packets = " << stats2->second.rxPackets<<std::endl;
      std::cout<<"Duration  : "<<(stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeFirstTxPacket.GetSeconds())<<std::endl;
      std::cout<<"Atraso: "<<stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeLastTxPacket.GetSeconds() <<"s"<<std::endl;
  atraso2 = stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeLastTxPacket.GetSeconds();
  atraso1 = stats2->second.timeFirstRxPacket.GetSeconds()-stats2->second.timeFirstTxPacket.GetSeconds();
      std::cout<<"Jitter: "<< atraso2-atraso1 <<std::endl;
      localJitter= atraso2-atraso1;//Jitter
      Dataset2.Add((double)Simulator::Now().GetSeconds(), (double) localJitter);
      std::cout<<"---------------------------------------------------------------------------"<<std::endl;
                 }//IFFFFFFFFFF

         atraso1 = atraso2;
         }

         Simulator::Schedule(Seconds(1),&JitterMonitor, fmHelper, flowMon, Dataset2);
         {
           flowMon->SerializeToXmlFile("JitterMonitor_Huenstein.xml", true, true);
         }
  }

//-------------------------Metodo-DELAY---------------------------

  void  DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3)
  {
    double localDelay=0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats3 = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing3 = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
    for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats3 = flowStats3.begin(); stats3 != flowStats3.end(); ++stats3)
    {
                 if(stats3->first == 1){//IFFFFFFFFFFF
            Ipv4FlowClassifier::FiveTuple fiveTuple3 = classing3->FindFlow (stats3->first);
            std::cout<<"Flow ID : "<< stats3->first <<";"<< fiveTuple3.sourceAddress <<"------>" <<fiveTuple3.destinationAddress<<std::endl;
            localDelay = stats3->second.timeLastRxPacket.GetSeconds()-stats3->second.timeLastTxPacket.GetSeconds();
          Dataset3.Add((double)Simulator::Now().GetSeconds(), (double) localDelay);
          std::cout<<"---------------------------------------------------------------------------"<<std::endl;
      }//IFFFFFFFFF
    }
    Simulator::Schedule(Seconds(1),&DelayMonitor, fmHelper, flowMon, Dataset3);
    {
       flowMon->SerializeToXmlFile("DelayMonitor_Huenstein.xml", true, true);
    }
  }
