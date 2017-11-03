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

#include "ns3/gnuplot.h"
#include "ns3/flow-monitor-module.h"
#include <ns3/flow-monitor-helper.h>

#include <stdio.h>
#include <iostream>
#include <fstream>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MasterCode");


//Proposta Conexão por P2P

		void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon,Gnuplot2dDataset DataSet);
		void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2);
		void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3);
		void avalParam(int nAp, double localDelay, double localThrou, double txPackets, double rxPackets);

int main (int argc, char *argv[])
{
// Step 1: Reconhecimento da rede.

//WIFI
//Configure WIFI
		uint32_t payloadSize = 1024;//bytes
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
  
  		  Time interPacketInterval = Seconds (1.);

		  UdpEchoClientHelper clientA (apInterfaceA.GetAddress (0), port);
		  clientA.SetAttribute ("MaxPackets", UintegerValue (1));
		  clientA.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  clientA.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  ApplicationContainer clientAppA = clientA.Install (wifiStaNodes.Get (0));
		  clientAppA.Start (Seconds (1.0));
		  clientAppA.Stop (Seconds (simulationTime - 1));

		  UdpEchoClientHelper clientAa (apInterfaceA.GetAddress (0), port);
		  clientAa.SetAttribute ("MaxPackets", UintegerValue (1));
		  clientAa.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  clientAa.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  ApplicationContainer clientAppAa = clientAa.Install (wifiStaNodes.Get (1));
		  clientAppAa.Start (Seconds (1.0));
		  clientAppAa.Stop (Seconds (simulationTime - 1));

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

		      AnimationInterface anim ("MasterCode_p1.xml"); // Mandatory
		      
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

//-------------------------Metodo-JITTER-------------------------
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
         // avalParam (double localJitter){


         // }

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





// Step 2: Analise dos Parametros e avaliação do nó mestre.
			

	  void avalParam(int nAp, double localDelay, double localThrou, double txPackets, double rxPackets)
	  {
//Determinar quantidade de parâmetros
				int nPar = 5;

//Determinar os Parametros utilizados
				int LostPackets [nAp][0];
				int Throughput [nAp][0];
				int Energy [nAp][0];
				int Delay [nAp][0];
				int Alcance [nAp][0];
// Determinar fluxo




//Atribuir valores dos Parâmetros
				for (int l = 0; l<nAp; ++l){
					switch(l){
							case 1:
							LostPackets [l][0] = 1;	
							break;				
							case 2:
							Throughput [l][0] = 2;
							break;
							case 3:
							Energy [l][0] = 3;
							break;
							case 4:
							Delay [l][0] = 4;
							break;
							case 5:
							Alcance [l][0] = 5;
							break;
							default:
							break;

				}

//Criar Matriz dos nós de retransmissão
				int mMR [nAp][nPar];

				for (int l = 0; l < nAp; ++l)
				{
					for (int c = 0; c < nPar; ++c)
					{
						switch(c){
							case 0:
							mMR [l][c] = l;
							break;
							case 1:
							mMR [l][c] = LostPackets [l][0];	
							break;				
							case 2:
							mMR [l][c] = Throughput [l][0];
							break;
							case 3:
							mMR [l][c] = Energy [l][0];
							break;
							case 4:
							mMR [l][c] = Delay [l][0];
							break;
							case 5:
							mMR [l][c] = Alcance [l][0];
							break;
							default:
							break;
						}
					}
				}
//Comparar Parâmetros
				int low_LostPckt = 2147483647;
				int high_Thoughput = 0;
				int high_Energy = 0;
				int low_Delay = 2147483647;
				int high_Alcance = 0;

				for (int l = 0; l < nAp; ++l){
					for (int c = 0; c < nPar; ++c){
						
						if(LostPackets[l][0] < low_LostPckt){
							low_LostPckt = LostPackets[l][0];
						}

						if(Throughput[l][0] > high_Thoughput){
							high_Thoughput = Throughput[l][0];
						}

						if(Energy[l][0]>high_Energy){
							high_Energy = Energy[l][0];
						}

						if(Delay[l][0] < low_Delay){
							low_Delay = Delay[l][0];
						}

						if(Alcance[l][0]< high_Alcance){
							high_Alcance = Alcance[l][0];
						}

					}
				}

//Atribuir Pontuação aos MRs
				int sum [nAp][0];

				for (int l = 0; l < nAp; ++l){
					for (int c = 1; c < nPar; ++c){
						switch(c){
							case 1:
								if(mMR[l][c]==low_LostPckt){
									sum[l][0] = sum[l][0] + 30;
								}
							break;				
							case 2:
								if(mMR[l][c]==high_Thoughput){
									sum[l][0] = sum[l][0] + 25;
								}
							break;
							case 3:
								if(mMR[l][c]==high_Energy){
									sum[l][0] = sum[l][0] + 20;
								}
							break;
							case 4:
								if(mMR[l][c]==low_Delay){
									sum[l][0] = sum[l][0] + 15;
								}
							break;
							case 5:
								if(mMR[l][c]==high_Alcance){
									sum[l][0] = sum[l][0] + 10;
								}
							break;
							default:
							break;
						}


					}
				}

//Imprimir Resultados
				for (int l=0; l<nAp; ++l){
					for (int c=0; c<nPar; ++c){
						 std::cout<<"Nó de Retransmissão " <<mMR[l][0]<< " Pontuação: "<< sum[l]<<std::endl;
					}
				}
	}
}






// Step 3: Envio dos pacotes do Servidor(LTE) para os Clientes.