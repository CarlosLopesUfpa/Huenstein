/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Laboratory Of Development Systems
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
 * Author: Carlos Lopes <carloslopesufpa@gmail.com>
 */


#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"
#include "ns3/propagation-loss-model.h"

#include "ns3/propagation-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-module.h"
#include <time.h>
#include "ns3/gnuplot.h"

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/command-line.h"

//#include "ns3/gtk-config-store.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Wifi_2");

void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet);
void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2);
void LossMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3);
void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset4);

int cenario = 1;
std::string gp = std::to_string(cenario);

int main (int argc, char *argv[])
{
  
  double PacketInterval = 0.15;
  uint32_t MaxPacketSize = 1024;
  double simTime = 100;
  double Rx = 326;

  // int aux_energy = 0;
  int nAp = 1;
  int nSta = 100;


  CommandLine cmd;
  cmd.AddValue ("Rx", "Number of Packets", Rx);
  cmd.Parse (argc,argv);

  // double Energia;

  // Energia = 0;
  
  // 1. Create 3 nodes

  NodeContainer wifiApNodes;
  wifiApNodes.Create (nAp);
  NodeContainer wifiStaNodes;
      
  MobilityHelper mobilitywifi;

  if(cenario == 1){
  wifiStaNodes.Create (nSta/2);
  mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (1414),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (5.0),
                                 "GridWidth", UintegerValue (10),
                                 "LayoutType", StringValue ("RowFirst"));
    }else{
          if(cenario == 2){
          wifiStaNodes.Create (nSta/2);
          mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
                                         "MinX", DoubleValue (1500.0),
                                         "MinY", DoubleValue (0),
                                         "DeltaX", DoubleValue (5.0),
                                         "DeltaY", DoubleValue (5.0),
                                         "GridWidth", UintegerValue (10),
                                         "LayoutType", StringValue ("RowFirst"));
            }else{
                  if(cenario == 3){
                  wifiStaNodes.Create (nSta);
                  mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
                                               "MinX", DoubleValue (0),
                                               "MinY", DoubleValue (4242),
                                               "DeltaX", DoubleValue (5.0),
                                               "DeltaY", DoubleValue (5.0),
                                               "GridWidth", UintegerValue (10),
                                               "LayoutType", StringValue ("RowFirst"));

                    }else{
                          if(cenario == 4){
                          wifiStaNodes.Create (nSta);
                          mobilitywifi.SetPositionAllocator ("ns3::GridPositionAllocator",
                                                           "MinX", DoubleValue (1500),
                                                           "MinY", DoubleValue (5656),
                                                           "DeltaX", DoubleValue (5.0),
                                                           "DeltaY", DoubleValue (5.0),
                                                           "GridWidth", UintegerValue (10),
                                                           "LayoutType", StringValue ("RowFirst"));
                          }
                        }
                  }
          }

  mobilitywifi.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                           "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
                           "Bounds", StringValue ("-6000|6000|-6000|6000"));
  mobilitywifi.Install (wifiStaNodes);
  mobilitywifi.Install (wifiApNodes);

  NodeContainer all;
  all.Add(wifiApNodes);
  all.Add(wifiStaNodes);

  

  std::string phyMode ("DsssRate1Mbps");
  bool verbose = false;
// The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
    }

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (-10) );
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add an upper mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  
  // AP/STA - NETWORK
  // NetDeviceContainer staDevice, apDevice;
  // Ssid ssid;

  // //Network A
  // ssid = Ssid ("network-A");
  // wifiPhy.Set ("ChannelNumber", UintegerValue (36));
  // wifiMac.SetType ("ns3::StaWifiMac",
  //                  "Ssid", SsidValue (ssid));
  // staDevice = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);


  // wifiMac.SetType ("ns3::ApWifiMac",
  //              "Ssid", SsidValue (ssid),
  //              "EnableBeaconJitter", BooleanValue (false));
  // apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes);



  //AD HOC - NETWORK
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes);
  NetDeviceContainer staDevice = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);


  // // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internetw;
  internetw.Install (all);

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("192.168.1.0", "255.255.255.0");
  // Ipv4InterfaceContainer apInterface = ipv4.Assign (apDevice);
  // Ipv4InterfaceContainer staInterface = ipv4.Assign (staDevice);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // ipv4.Assign (apDevice);
  // ipv4.Assign (staDevice);

  // uint16_t  port = 9;
  // // Time interPacketInterval = Seconds (0.1);

  // UdpEchoServerHelper apServer (port);
  // ApplicationContainer serverApp = apServer.Install (wifiApNodes.Get(0));
  // serverApp.Start (Seconds (1.0));
  // serverApp.Stop (Seconds(30.0));
      
  // UdpEchoClientHelper staClient (Ipv4Address ("192.168.1.1"), port);
  // staClient.SetAttribute ("MaxPackets", UintegerValue (Rx));
  // staClient.SetAttribute ("Interval", TimeValue (Seconds (interPacketInterval))); //packets/s
  // staClient.SetAttribute ("PacketSize", UintegerValue (1024));

  // // ApplicationContainer wifiApps;
  // for(uint16_t u = 0; u<wifiStaNodes.GetN(); ++u){
  //     serverApp = staClient.Install (wifiStaNodes.Get (u));
  // }
  // serverApp.Start (Seconds (2.0));
  // serverApp.Stop (Seconds(30.0));


//
// Create one udpServer applications on node one.
  uint16_t  port = 9;
for (uint16_t u = 0; u<nAp; ++u){
  std::string ipAp = "192.168.1." + std::to_string(u+1);
  
  port++;
  UdpServerHelper server (port);
  ApplicationContainer appsS = server.Install (wifiApNodes);
  appsS.Start (Seconds (simTime/2));
  appsS.Stop (Seconds (simTime));

  UdpClientHelper client (Ipv4Address (ipAp.c_str()), port); 
  client.SetAttribute ("MaxPackets", UintegerValue (Rx));
  client.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  for(uint16_t i = 0; i<nSta; ++i){
  ApplicationContainer appsC = client.Install (wifiStaNodes.Get(i));
  appsC.Start (Seconds (simTime/2));
  appsC.Stop (Seconds (simTime));
  }
}

//FLOW-MONITOR
    

    //-----------------FlowMonitor-THROUGHPUT----------------

    std::string fileNameWithNoExtension = "wifi_Flow_vs_Throughput_Group_" + gp;
    std::string graphicsFileName        = fileNameWithNoExtension + ".png";
    std::string plotFileName            = fileNameWithNoExtension + ".plt";
    std::string plotTitle               = "Flow_vs_Throughput";
    std::string dataTitle               = "Throughput";

    Gnuplot gnuplot (graphicsFileName);
    gnuplot.SetTitle (plotTitle);
    gnuplot.SetTerminal ("png");
    gnuplot.SetLegend ("Flow", "Throughput");
     
    Gnuplot2dDataset dataset;
    dataset.SetTitle (dataTitle);
    dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

    //flowMonitor declaration
    FlowMonitorHelper fmHelper;
    Ptr<FlowMonitor> allMon = fmHelper.InstallAll();

    // call the flow monitor function
    ThroughputMonitor(&fmHelper, allMon, dataset); 
                
     
    //-----------------FlowMonitor-Atraso--------------------

    std::string fileNameWithNoExtension2 = "wifi_Flow_vs_Delay_Group_" + gp;
    std::string graphicsFileName2      = fileNameWithNoExtension2 + ".png";
    std::string plotFileName2        = fileNameWithNoExtension2 + ".plt";
    std::string plotTitle2           = "Flow_vs_Delay";
    std::string dataTitle2           = "Delay";

    Gnuplot gnuplot2 (graphicsFileName2);
    gnuplot2.SetTitle(plotTitle2);
    gnuplot2.SetTerminal("png");
    gnuplot2.SetLegend("Flow", "Delay");

    Gnuplot2dDataset dataset2;
    dataset2.SetTitle(dataTitle2);
    dataset2.SetStyle(Gnuplot2dDataset::LINES_POINTS);

    //FlowMonitorHelper fmHelper;
    //Ptr<FlowMonitor> allMon = fmHelper.InstallAll();

    DelayMonitor(&fmHelper, allMon, dataset2);

    //-----------------FlowMonitor-LossPackets--------------------

    std::string fileNameWithNoExtension3 = "wifi_Flow_vs_Loss_Group_" + gp;
    std::string graphicsFileName3      = fileNameWithNoExtension3 + ".png";
    std::string plotFileName3        = fileNameWithNoExtension3 + ".plt";
    std::string plotTitle3           = "Flow_vs_Loss";
    std::string dataTitle3           = "Loss";

    Gnuplot gnuplot3 (graphicsFileName3);
    gnuplot3.SetTitle(plotTitle3);
    gnuplot3.SetTerminal("png");
    gnuplot3.SetLegend("Flow", "Loss");

    Gnuplot2dDataset dataset3;
    dataset3.SetTitle(dataTitle3);
    dataset3.SetStyle(Gnuplot2dDataset::LINES_POINTS);

    //FlowMonitorHelper fmHelper;
    //Ptr<FlowMonitor> allMon = fmHelper.InstallAll();

    LossMonitor(&fmHelper, allMon, dataset3);
   
    //-----------------FlowMonitor-JITTER--------------------

    std::string fileNameWithNoExtension4 = "wifi_Flow_vs_Jitter_Group_" + gp;
    std::string graphicsFileName4      = fileNameWithNoExtension4 + ".png";
    std::string plotFileName4        = fileNameWithNoExtension4 + ".plt";
    std::string plotTitle4           = "Flow_vs_Jitter";
    std::string dataTitle4           = "Jitter";

    Gnuplot gnuplot4 (graphicsFileName4);
    gnuplot4.SetTitle(plotTitle4);
    gnuplot4.SetTerminal("png");
    gnuplot4.SetLegend("Flow", "Jitter");

    Gnuplot2dDataset dataset4;
    dataset4.SetTitle(dataTitle4);
    dataset4.SetStyle(Gnuplot2dDataset::LINES_POINTS);

    //FlowMonitorHelper fmHelper;
    //Ptr<FlowMonitor> allMon = fmHelper.InstallAll();

    JitterMonitor(&fmHelper, allMon, dataset4);

//NetAnim
   AnimationInterface anim (gp + "_group_simulation_2_wifi.xml"); // Mandatory
        
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
        anim.EnablePacketMetadata (); // Optional



  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

//Gnuplot ...continued
      gnuplot.AddDataset (dataset);
      std::ofstream plotFile (plotFileName.c_str());
      gnuplot.GenerateOutput (plotFile);
      plotFile.close ();

      gnuplot2.AddDataset(dataset2);;
      std::ofstream plotFile2 (plotFileName2.c_str());
      gnuplot2.GenerateOutput(plotFile2);
      plotFile2.close();

      gnuplot3.AddDataset(dataset3);;
      std::ofstream plotFile3 (plotFileName3.c_str());
      gnuplot3.GenerateOutput(plotFile3);
      plotFile3.close();

      gnuplot4.AddDataset(dataset4);;
      std::ofstream plotFile4 (plotFileName4.c_str());
      gnuplot4.GenerateOutput(plotFile4);
      plotFile4.close();

  Simulator::Destroy();
  return 0;

}

//-------------------------Metodo-VAZÃO---------------------------

  void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet)
  {
    double localvazao = 0;
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
      
      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
        {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        // if(fiveTuple.destinationAddress == "192.168.1.6")
        if(stats->first < 2)
        {
             std::cout<<"--------------------------------Vazao---------------------------------"<<std::endl;
              std::cout<<"Flow ID: " << stats->first <<"; "<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
              std::cout<<"Duration  : "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
              std::cout<<"Vazao: " <<  stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024<<" Mbps"<<std::endl;
              localvazao=  stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024;
              DataSet.Add((double)Simulator::Now().GetSeconds(),(double) localvazao);
              std::cout<<" "<<std::endl;
          }
        }
    
      Simulator::Schedule(Seconds(1), &ThroughputMonitor, fmhelper, flowMon, DataSet);
   //if(flowToXml)
      {
    flowMon->SerializeToXmlFile (gp + "_group_wifi2_Flow.xml", true, true);
      }
  }

//-------------------------Metodo-Atraso-------------------------
    void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2)
  {
    double localDelay=0;
    
           std::map<FlowId, FlowMonitor::FlowStats> flowstats = flowMon->GetFlowStats();
           Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
           
      
             for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowstats.begin(); stats != flowstats.end(); ++stats)
             {
                Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
                // if(fiveTuple.destinationAddress == "192.168.1.6")
                if(stats->first < 2)
                {
                    std::cout<<"--------------------------------Atraso-------------------------------------"<<std::endl;
                    std::cout<<"Flow ID: "<< stats->first <<"; "<< fiveTuple.sourceAddress <<" ------> " <<fiveTuple.destinationAddress<<std::endl;
                    std::cout<<"Duration  : "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
                    std::cout<<"Atraso: "<< ((stats->second.timeLastRxPacket.GetSeconds()) - (stats->second.timeLastTxPacket.GetSeconds()))<<std::endl;
                    localDelay = ((stats->second.timeLastRxPacket.GetSeconds()) - (stats->second.timeLastTxPacket.GetSeconds()));
                    Dataset2.Add((double)Simulator::Now().GetSeconds(), (double) localDelay);
                    std::cout<<" "<<std::endl;
                }
             }
      
        
      Simulator::Schedule(Seconds(1), &DelayMonitor, fmHelper, flowMon, Dataset2);
      // {
      //   flowMon->SerializeToXmlFile("DelayMonitor.xml", true, true);
      // }
  }

  //-------------------------Metodo-Loss-------------------------
    void LossMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3)
  {
    double localLoss=0;

           std::map<FlowId, FlowMonitor::FlowStats> flowstats = flowMon->GetFlowStats();
           Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
           
      
             for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowstats.begin(); stats != flowstats.end(); ++stats)
             {
                Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
                // if(fiveTuple.destinationAddress == "192.168.1.6")
                if(stats->first < 2)
                {
                    std::cout<<"--------------------------------Loss-------------------------------------"<<std::endl;
                    std::cout<<"    Flow ID: "<< stats->first <<"; "<< fiveTuple.sourceAddress <<" ------> " <<fiveTuple.destinationAddress<<std::endl;
                    std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
                    std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
                    localLoss =stats->second.txPackets - stats->second.rxPackets;
                    std::cout<<"Perda de Pacotes: "<< localLoss<<std::endl;
                    Dataset3.Add((double)Simulator::Now().GetSeconds(), (double) localLoss);
                    std::cout<<" "<<std::endl;
                }

             }
              
      Simulator::Schedule(Seconds(1), &LossMonitor, fmHelper, flowMon, Dataset3);
      // {
      //   flowMon->SerializeToXmlFile("LossMonitor.xml", true, true);
      // }
  }


    
    void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset4)
    {
      double localJitter=0;
      double atraso1=0;
      double atraso2 =0;

           std::map<FlowId, FlowMonitor::FlowStats> flowstats = flowMon->GetFlowStats();
           Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
           for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowstats.begin(); stats != flowstats.end(); ++stats)
           {
            Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
            // if(fiveTuple.destinationAddress == "192.168.1.6")
            if(stats->first < 2)
            {
                std::cout<<"--------------------------------Jitter-------------------------------------"<<std::endl;
                std::cout<<"Flow ID : "<< stats->first <<"; "<< fiveTuple.sourceAddress <<"------>" <<fiveTuple.destinationAddress<<std::endl;
                atraso2 = stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeLastTxPacket.GetSeconds();
                atraso1 = stats->second.timeFirstRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds();
                std::cout<<"Jitter: "<< atraso2 - atraso1 <<std::endl;
                localJitter= atraso2-atraso1;//Jitter
                Dataset4.Add((double)Simulator::Now().GetSeconds(), (double) localJitter);
                std::cout<<" "<<std::endl;
                }
                atraso1 = atraso2;
           }

           Simulator::Schedule(Seconds(1),&JitterMonitor, fmHelper, flowMon, Dataset4);
           // {
           //   flowMon->SerializeToXmlFile("JitterMonitor.xml", true, true);
           // }
    }