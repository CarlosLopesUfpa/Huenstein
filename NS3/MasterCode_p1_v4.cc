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


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MasterCode");


//Proposta Conexão por P2P

		void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet, int nAp, int cont_Vazao, double som_Vazao[][0], double med_Vazao[][0]);
		void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2, int nAp, int cont_Jitter, double som_Jitter[][0], double med_Jitter[][0]);
		void DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3,  int nAp, int cont_Delay, double som_Delay[][0], double med_Delay[][0]);
		void avalParam(int nAp, double medi_Vazao[][0], double medi_Delay[][0]);
int main (int argc, char *argv[]) {
// Step 1: Reconhecimento da rede.

//WIFI
//Configure WIFI
		uint32_t payloadSize = 4096;//bytes
		uint64_t simulationTime = 10; //seconds
		int nAp = 1;
		int nSta = 2;

//Variáveis para receber dados do FlowMonitor
		double soma_Vazao [nAp][0];
		double media_Vazao [nAp][0];
		int conta_Vazao = 0;

		double soma_Jitter [nAp][0];
		double media_Jitter [nAp][0];
		int conta_Jitter = 0;
		// double pct_perdido = 0;

		double soma_Delay [nAp][0];
		double media_Delay [nAp][0];
		int conta_Delay = 0;

		// double energy[nAp][0] = 0;



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

//FLOW-MONITOR
		

	//-----------------FlowMonitor-THROUGHPUT----------------

	    std::string fileNameWithNoExtension = "Master_FlowVSThroughput";
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
	  ThroughputMonitor(&fmHelper, allMon, dataset, nAp, conta_Vazao, soma_Vazao, media_Vazao); 


	   
	//-----------------FlowMonitor-JITTER--------------------

	    std::string fileNameWithNoExtension2 = "Master_FlowVSJitter";
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

	  JitterMonitor(&fmHelper, allMon, dataset2, nAp, conta_Jitter, soma_Jitter, media_Jitter);

	//-----------------FlowMonitor-DELAY---------------------

	  std::string fileNameWithNoExtension3 = "Master_FlowVSDelay";
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

      DelayMonitor(&fmHelper, allMon, dataset3, nAp, conta_Delay, soma_Delay, media_Delay);

//LÓGICA DE SELEÇÃO
      avalParam(nAp, media_Vazao, media_Delay);


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
	      anim.EnablePacketMetadata (); // Optiona
	      anim.EnableIpv4RouteTracking ("routingtable-wireless.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional
		  anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); //Optional
		  anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); //Optional
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

  void ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset DataSet, int nAp, int cont_Vazao, double som_Vazao[][0], double med_Vazao[][0])
    {

          double localThrou=0;
      std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats();
      Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier());
      	
      	for(int l=0; l<=nAp; ++l){
	      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
	      {
				Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
				if(fiveTuple.sourceAddress == "192.168.1.1"){//IFFFFFFFFFFFFFFFFFFFFFFF
			      	
		      	std::cout<<"--------------------------------Throughput---------------------------------"<<std::endl;
		        std::cout<<"Flow ID: " << stats->first <<";"<< fiveTuple.sourceAddress <<" -----> "<<fiveTuple.destinationAddress<<std::endl;
		        std::cout<<"Tx Packets = " << stats->second.txPackets<<std::endl;
		        std::cout<<"Rx Packets = " << stats->second.rxPackets<<std::endl;
		              std::cout<<"Duration    : "<<(stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())<<std::endl;
		        std::cout<<"Last Received Packet  : "<< stats->second.timeLastRxPacket.GetSeconds()<<" Seconds"<<std::endl;
		        std::cout<<"Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps"<<std::endl;
		              localThrou=(stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds()-stats->second.timeFirstTxPacket.GetSeconds())/1024/1024);
		        // updata gnuplot data
		              DataSet.Add((double)Simulator::Now().GetSeconds(),(double) localThrou);
		        std::cout<<" "<<std::endl;
		        std::cout<<" "<<std::endl;

	//Coleta de dados a Vazão para o algoritmode seleção
		        som_Vazao[l][0] = som_Vazao[l][0] + localThrou;
		        cont_Vazao++;
			   
	    		}//IFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF  
	  		}

	//Continuação da Coleta de dados
	  	med_Vazao[l][0] = som_Vazao[l][0]/cont_Vazao;
	  	cont_Vazao=0;
	  	}
	        Simulator::Schedule(Seconds(1), &ThroughputMonitor, fmhelper, flowMon, DataSet,  nAp, cont_Vazao, som_Vazao, med_Vazao);
	     //if(flowToXml)
	        {
	    flowMon->SerializeToXmlFile ("Master_ThroughputMonitor.xml", true, true);
	        }
	   	}

//-------------------------Metodo-JITTER-------------------------
	double atraso1=0;
	// double pct_Enviado = 0;
	// double pct_Recebido = 0;
	// double som_Enviado = 0;
	// double som_Recebido = 0;
  void JitterMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset2, int nAp, int cont_Jitter, double som_Jitter[][0], double med_Jitter[][0])
  {
         double localJitter = 0;
         double atraso2 = 0;

		 
         std::map<FlowId, FlowMonitor::FlowStats> flowStats2 = flowMon->GetFlowStats();
         Ptr<Ipv4FlowClassifier> classing2 = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
         
	for(int l=0; l<=nAp; ++l){
         for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats2 = flowStats2.begin(); stats2 != flowStats2.end(); ++stats2)
         {
         	Ipv4FlowClassifier::FiveTuple fiveTuple2 = classing2->FindFlow (stats2->first);
       		if(fiveTuple2.sourceAddress == "192.168.1.1"){//IFFFFFFFFFFF
			    Ipv4FlowClassifier::FiveTuple fiveTuple2 = classing2->FindFlow (stats2->first);
			    std::cout<<"--------------------------------Jitter-------------------------------------"<<std::endl;
			    std::cout<<"Flow ID: "<< stats2->first <<";"<< fiveTuple2.sourceAddress <<" ------> " <<fiveTuple2.destinationAddress<<std::endl;
			    // pct_Enviado = stats2->second.txPackets;
			    // pct_Recebido = stats2->second.rxPackets;
			    std::cout<<"Tx Packets = " << stats2->second.txPackets<<std::endl;
			    std::cout<<"Rx Packets = " << stats2->second.rxPackets<<std::endl;
			    std::cout<<"Duration  : "<<(stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeFirstTxPacket.GetSeconds())<<std::endl;
			    std::cout<<"Atraso: "<<stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeLastTxPacket.GetSeconds() <<"s"<<std::endl;
			  	atraso2 = stats2->second.timeLastRxPacket.GetSeconds()-stats2->second.timeLastTxPacket.GetSeconds();
			 	atraso1 = stats2->second.timeFirstRxPacket.GetSeconds()-stats2->second.timeFirstTxPacket.GetSeconds();
			    std::cout<<"Jitter: "<< atraso2-atraso1 <<std::endl;
			    localJitter= atraso2-atraso1;//Jitter
			    Dataset2.Add((double)Simulator::Now().GetSeconds(), (double) localJitter);
			    std::cout<<" "<<std::endl;
    			std::cout<<" "<<std::endl;
//Coleta de dados da Perda de Pacotes para o algoritmode seleção
    			// som_Enviado = som_Enviado + pct_Enviado;
    			// som_Recebido = som_Recebido + pct_Recebido;

//Coleta de dados do Jitter para o algoritmode seleção
    			som_Jitter[l][0] = som_Jitter[l][0] + localJitter;
    			cont_Jitter++;
             }//IFFFFFFFFFF

         atraso1 = atraso2;
         
         }

//Continuação da Coleta de dados do Jitter
    med_Jitter[l][0] = som_Jitter[l][0]/cont_Jitter;
    cont_Jitter = 0;
	}
					// pct_perd = som_Enviado - som_Recebido;

         Simulator::Schedule(Seconds(1),&JitterMonitor, fmHelper, flowMon, Dataset2, nAp, cont_Jitter, som_Jitter, med_Jitter);
         {
           flowMon->SerializeToXmlFile("Master_JitterMonitor.xml", true, true);
         }
         // avalParam (double localJitter){


         // }

  }

	//-------------------------Metodo-DELAY---------------------------

	  void  DelayMonitor(FlowMonitorHelper *fmHelper, Ptr<FlowMonitor> flowMon, Gnuplot2dDataset Dataset3,  int nAp, int cont_Delay, double som_Delay[][0], double med_Delay[][0])
	  {
	    double localDelay=0;

	    std::map<FlowId, FlowMonitor::FlowStats> flowStats3 = flowMon->GetFlowStats();
	    Ptr<Ipv4FlowClassifier> classing3 = DynamicCast<Ipv4FlowClassifier> (fmHelper->GetClassifier());
	for(int l=0; l<=nAp; ++l){
	    for(std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats3 = flowStats3.begin(); stats3 != flowStats3.end(); ++stats3)
	    {
	    	Ipv4FlowClassifier::FiveTuple fiveTuple3 = classing3->FindFlow (stats3->first);
	        if(fiveTuple3.sourceAddress == "192.168.1.1"){//IFFFFFFFFFFF
	        Ipv4FlowClassifier::FiveTuple fiveTuple3 = classing3->FindFlow (stats3->first);
	        std::cout<<"--------------------------------Delay--------------------------------------"<<std::endl;
	        std::cout<<"Flow ID: "<< stats3->first <<";"<< fiveTuple3.sourceAddress <<" ------> " <<fiveTuple3.destinationAddress<<std::endl;
	        localDelay = stats3->second.timeLastRxPacket.GetSeconds()-stats3->second.timeLastTxPacket.GetSeconds();
	        Dataset3.Add((double)Simulator::Now().GetSeconds(), (double) localDelay);
	        std::cout<<"Delay: "<<localDelay<<std::endl;
	        std::cout<<" "<<std::endl;
	        std::cout<<" "<<std::endl;

//Coleta de dados do Delay para o algoritmode seleção
			som_Delay[l][0] = som_Delay[l][0] + localDelay;
			cont_Delay++;
	      }//IFFFFFFFFF
	    }
	

//Continuação da Coleta de dados do Delay
	med_Delay[l][0] = som_Delay[l][0]/cont_Delay;
	cont_Delay=0;
	}
	    Simulator::Schedule(Seconds(1), &DelayMonitor, fmHelper, flowMon, Dataset3, nAp, cont_Delay, som_Delay, med_Delay);
	    {
	       flowMon->SerializeToXmlFile("Master_DelayMonitor.xml", true, true);
	    }
	  }





// Step 2: Analise dos Parametros e avaliação do nó mestre.
			

	void avalParam(int nAp, double medi_Vazao[][0], double medi_Delay[][0])
		  {
	//Determinar quantidade de parâmetros
					int nPar = 5;

	//Determinar os Parametros utilizados
					double LostPackets [nAp][1];
					double Throughput [nAp][1];
					double Energy [nAp][1];
					double Delay [nAp][1];
					double Range [nAp][1];

	//Atribuir 0 a todas as posições da matriz (limpar)
	// int flow[nAp+1][0];

					for(int l=0; l<=nAp; ++l){
						LostPackets [nAp][0] = 0;
				 		Throughput [nAp][0] = 0;
						Energy [nAp][0] = 0;
						Delay [nAp][0] = 0;
						Range [nAp][0] = 0;
					}

	//Atribuir valores dos Parâmetros
					srand((unsigned)time(0));
					for (int l = 0; l<=nAp; ++l){

						LostPackets [l][0]= rand()%(1000);
															
						Throughput [l][0]= medi_Vazao[l][0];

						Energy [l][0]= rand()%(100);

						Delay [l][0]= medi_Delay[l][0];

						Range [l][0]= rand()%(100);
					}
					
					std::cout << "Valor de parametros: " <<std::endl;
					for(int l=0;l<=nAp; l++){
					std::cout << " " <<std::endl;
					std::cout << "Nó: " << l <<std::endl;
					std::cout << " " <<std::endl;
					std::cout << "LostPackets " << LostPackets [l][0] << " " <<std::endl;
					std::cout << "Throughput " << Throughput[l][0] << " " <<std::endl;
					std::cout << "Energy " << Energy[l][0] << " " <<std::endl;
					std::cout << "Delay " << Delay[l][0]<< " " <<std::endl;
					std::cout << "Range " << Range[l][0] << " " <<std::endl;
					}
					
								
					

	//Criar Matriz dos nós de retransmissão
					double mMR [nAp][nPar+1];

					for (int l = 0; l <= nAp; ++l)
					{
						for (int c = 0; c <= nPar; ++c)
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
								mMR [l][c] = Range [l][0];
								break;
								default:
								break;
							}
						}
					}

	//mMR[0][1]= perda de pacotes      |   	
	//mMR[0][2]= vazão                 |
	//mMR[0][3]= energia               |   Médias/Valores "brutos", ainda nao normalizadas
	//mMR[0][4]= delay                 |
	//mMR[0][5]= alcance               |

	double somaPerdaPct = 0;
	double somaVazao = 0;
	double somaEnergia = 0;
	double somaDelay = 0;
	double somaAlcance = 0; 

							for (int l = 0; l <= nAp; ++l){

								somaPerdaPct =  somaPerdaPct +  mMR[l][1];  
								somaVazao    =  somaVazao    +  mMR[l][2];
								somaEnergia  =  somaEnergia  +  mMR[l][3];
								somaDelay    =  somaDelay    +  mMR[l][4];
								somaAlcance	 =  somaAlcance  +  mMR[l][5];

							}
							for(int l = 0; l<1; l++){
								std::cout << " " <<std::endl;
								std::cout << " " << " " <<std::endl;
								std::cout << "Somatória LostPackets " << somaPerdaPct  << " " <<std::endl;
								std::cout << "Somatória Throughput " << somaVazao  << " " <<std::endl;
								std::cout << "Somatória Energy " << somaEnergia  << " " <<std::endl;
								std::cout << "Somatória Delay " << somaDelay  << " " <<std::endl;
								std::cout << "Somatória Range " << somaAlcance  << " " <<std::endl;
							}


	//somaPerdaPct   | 
	//somaVazão      |
	//somaEnergia    |  Essas variáveis recebem a soma dos valores dos parametros de todos os nós, 
	//somaDelay      |  vamo usar pra normalizar os valores antes de usar na fórmula
	//somaAlcance    |



	//Normalização Inversa e Atribuição da pontuação de perda de pacotes ao Ap (Valor Normalizado * Pontuação do parametro).
					double normalmMR[nAp][nPar];
							std::cout << " " <<std::endl;
							for (int l = 0; l <= nAp; ++l){
								normalmMR[l][1] = (mMR[l][1]/somaPerdaPct)*0.30;
								mMR[l][1] = 1-normalmMR[l][1];
			
								std::cout << "Nó " << l << " Pontuação perda de pacotes " << mMR[l][1] <<std::endl;
								std::cout << " " <<std::endl;

							}
	//Normalização e Atribuição da pontuação de Vazão ao Ap.
							for (int l = 0; l <= nAp; ++l){
								normalmMR[l][2] = (mMR[l][2]/somaVazao)*100;
								mMR[l][2] = normalmMR[l][2]*0.25;
					
								std::cout << "Nó " << l << " Pontuação vazão " << mMR[l][2] <<std::endl;
								std::cout << " " <<std::endl;
							}
	//Normalização e Atribuição da pontuação de Energia ao Ap.
							for (int l = 0; l <= nAp; ++l){
								normalmMR[l][3] = (mMR[l][3]/somaEnergia)*100;
								mMR[l][3] = normalmMR[l][3]*0.20;
				
								std::cout << "Nó " << l << " Pontuação energia " << mMR[l][3] <<std::endl;
								std::cout << " " <<std::endl;
							}
	//Normalização Inversa e Atribuição da pontuação de Delay ao Ap.
							
							for (int l = 0; l <= nAp; ++l){
								normalmMR[l][4] = (mMR[l][4]/somaDelay)*0.3;
								mMR[l][4] = 1-normalmMR[l][4];
										
								std::cout << "Nó " << l << " Pontuação Delay " << mMR[l][4] <<std::endl;
								std::cout << " " <<std::endl;
							}
	//Normalização e Atribuição da pontuação de Alcance ao Ap.
							for (int l = 0; l <= nAp; ++l){
								normalmMR[l][5] = (mMR[l][5]/somaAlcance)*100;
								mMR[l][5] = normalmMR[l][5]*0.10;
					
								std::cout << "Nó " << l << " Pontuação alcance " << mMR[l][5] <<std::endl;
								std::cout << " " <<std::endl;
						
							}
	//Somatória da pontuação dos Aps.
					double ic[nAp][2];
							for(int l=0; l<=nAp; ++l){
								for(int c=0; c<=nPar; ++c){
									ic[l][0] = l;
									ic[l][1] = ic[l][1]+mMR[l][c];
								}
							
							std::cout << "Nó " << ic[l][0] << " Pontuação Geral " << ic[l][1] <<std::endl;
							std::cout << "////////////////////////////" <<std::endl;
							}
									

	//Imprimir Resultados
							double maior_ic = 0;
							double id = 0;
							for (int l=0; l<=nAp; ++l){
								if(ic[l][1] > maior_ic){
									maior_ic = ic[l][1];
									id = l;
								}
							}
							std::cout << " " <<std::endl;
							std::cout << "Nó de Retransmissão Mestre " << id << " Pontuação: " << maior_ic <<std::endl;


	}






// Step 3: Envio dos pacotes do Servidor(LTE) para os Clientes.