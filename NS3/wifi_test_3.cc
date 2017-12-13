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
#include "ns3/gnuplot.h"

#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"

//#include "ns3/gtk-config-store.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Wifi_Test");

void ThroughputMonitor(FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet, double Vazao, double Energia);
void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2, double Atraso, double Lost);
    


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

  double Vazao;
  double Lost;
  double Atraso;
  double Energia;

  double Rx = 99;

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
          Energia = aux_energy;
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


//FLOW-MONITOR
    

  //-----------------FlowMonitor-THROUGHPUT----------------

      std::string fileNameWithNoExtension = "Flow vs Throughput";
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
    ThroughputMonitor(&fmHelper, allMon, dataset, Vazao, Energia); 
                
     
  //-----------------FlowMonitor-Atraso--------------------

      std::string fileNameWithNoExtension2 = "Flow vs Delay";
      std::string graphicsFileName2      = fileNameWithNoExtension2 + ".png";
      std::string plotFileName2        = fileNameWithNoExtension2 + ".plt";
      std::string plotTitle2           = "Flow vs Delay";
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

    DelayMonitor(&fmHelper, allMon, dataset2, Atraso, Lost);


//NetAnim
   AnimationInterface anim ("wifi_test_3.xml"); // Mandatory
        
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


  Simulator::Stop(Seconds(simTime));
  Simulator::Run();



  Simulator::Destroy();
  return 0;

}

//-------------------------Metodo-VAZÃO---------------------------

  void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet, double Vazao, double Energia)
  {
    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
      
      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
        {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);

        if(fiveTuple.sourceAddress == "192.168.1.1"){//IFFFFFFFFFFFFFFFFFFFFFFF
              
              std::cout<<"--------------------------------Vazao---------------------------------"<<std::endl;
              std::cout<<"Flow ID: " << stats->first <<";"<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
              std::cout<<"   Duration    : "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
              std::cout<<"   Last Received Packet  : "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
              Vazao=(stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024);
              std::cout<<"   Vazao: " << Vazao;
              std::cout << "   Energy: " << Energia << "\n";
              // updata gnuplot data
              DataSet.Add((double)Simulator::Now().GetSeconds(),(double) Vazao);
              std::cout<<" "<<std::endl;
              std::cout<<" "<<std::endl;

          }//IFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF  
        }
    
      Simulator::Schedule(Seconds(1), &ThroughputMonitor, fmhelper, flowMon, DataSet, Vazao, Energia);
   //if(flowToXml)
      {
    flowMon->SerializeToXmlFile ("ThroughputMonitor.xml", true, true);
      }
  }

//-------------------------Metodo-Atraso-------------------------
    void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2, double Atraso, double Lost)
  {
           std::map<FlowId, FlowMonitor::FlowStats> flowStats2 = flowMon->GetFlowStats();
           Ptr<Ipv4FlowClassifier> classing2 = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
           
      
             for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats2 = flowStats2.begin(); stats2 != flowStats2.end(); ++stats2)
             {
                Ipv4FlowClassifier::FiveTuple fiveTuple2 = classing2->FindFlow (stats2->first);
                if(fiveTuple2.sourceAddress == "192.168.1.1")
                {//IFFFFFFFFFFF
                    Ipv4FlowClassifier::FiveTuple fiveTuple2 = classing2->FindFlow (stats2->first);
                    std::cout<<"--------------------------------Atraso-------------------------------------"<<std::endl;
                    std::cout<<"Flow ID: "<< stats2->first <<";"<< fiveTuple2.sourceAddress <<" ------> " <<fiveTuple2.destinationAddress<<std::endl;
                    std::cout<<"Tx Packets = " << stats2->second.txPackets<<std::endl;
                    std::cout<<"Rx Packets = " << stats2->second.rxPackets<<std::endl;
                    Lost = stats2->second.rxPackets - stats2->second.txPackets;
                    std::cout<<"Perda de Pacotes: "<< Lost;
                    std::cout<<"Duration  : "<<(stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeFirstTxPacket.GetSeconds())<<std::endl;
                    Atraso = stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeLastTxPacket.GetSeconds();
                    std::cout<<"Atraso: "<< Atraso;
                               
                    Dataset2.Add((double)Simulator::Now().GetSeconds(), (double) Atraso);
                    std::cout<<" "<<std::endl;
                    std::cout<<" "<<std::endl;
        
                }//IFFFFFFFFFF

             }
      
        
      Simulator::Schedule(Seconds(1), &DelayMonitor, fmHelper, flowMon, Dataset2, Atraso, Lost);
      {
        flowMon->SerializeToXmlFile("DelayMonitor.xml", true, true);
      }
  }