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

#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/command-line.h"


//#include "ns3/gtk-config-store.h"

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
NS_LOG_COMPONENT_DEFINE ("Wifi_1");

void avalParam(int nAp, double** Vazao, double** Atraso, double** Loss, double** Energia, double** Jitterav);

int cenario = 1;

int main (int argc, char *argv[]) {

// Step 1: Reconhecimento da rede.
//WIFI
  int nAp = 5;
  int nSta = 100;


  int col = 1;

  double simTime = 100;
  uint32_t MaxPacketSize = 1024;
  double PacketInterval = 0.15;
//Variáveis para receber dados do FlowMonitor
    double** Vazao = create(nAp, col);

    double** Jitter = create(nAp, col);
    double atraso1 = 0;
    double atraso2 = 0;
    // double pct_perdido = 0;

    double** Atraso = create(nAp, col);

    double** Loss = create(nAp, col);

    double** Energia = create(nAp, col);
    int aux_energy = 0;


  //uint16_t numberOfNodes = 1;
  // double Rx = 0;

  // CommandLine cmd;
  // cmd.AddValue ("Rx", "Number of Packets", Rx);
  // cmd.Parse (argc,argv);
    
  // 1. Create 3 nodes

  NodeContainer wifiApNodes;
  wifiApNodes.Create (nAp);
      
  NodeContainer wifiStaNodes;
      

   

  MobilityHelper mobilitywifiSta;

  MobilityHelper mobilitywifiAp;
  Ptr<ListPositionAllocator> positionAllocMN = CreateObject<ListPositionAllocator> ();
  
  if(cenario == 1){
  
  positionAllocMN->Add (Vector(0, 1414, 1.5));
  mobilitywifiAp.SetPositionAllocator(positionAllocMN);;

  
  wifiStaNodes.Create (nSta/2);
  mobilitywifiSta.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (1414),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (5.0),
                                 "GridWidth", UintegerValue (10),
                                 "LayoutType", StringValue ("RowFirst"));
    }else{
          if(cenario == 2){

          positionAllocMN->Add (Vector(1500, 0, 1.5));
          mobilitywifiAp.SetPositionAllocator(positionAllocMN);;

  
          wifiStaNodes.Create (nSta/2);
          mobilitywifiSta.SetPositionAllocator ("ns3::GridPositionAllocator",
                                         "MinX", DoubleValue (1500.0),
                                         "MinY", DoubleValue (0),
                                         "DeltaX", DoubleValue (5.0),
                                         "DeltaY", DoubleValue (5.0),
                                         "GridWidth", UintegerValue (10),
                                         "LayoutType", StringValue ("RowFirst"));
            }else{
                  if(cenario == 3){
                    
                    positionAllocMN->Add (Vector(0, 4242, 1.5));
                    mobilitywifiAp.SetPositionAllocator(positionAllocMN);;
   
                    
                    wifiStaNodes.Create (nSta);
                  mobilitywifiSta.SetPositionAllocator ("ns3::GridPositionAllocator",
                                               "MinX", DoubleValue (0),
                                               "MinY", DoubleValue (4242),
                                               "DeltaX", DoubleValue (5.0),
                                               "DeltaY", DoubleValue (5.0),
                                               "GridWidth", UintegerValue (10),
                                               "LayoutType", StringValue ("RowFirst"));

                    }else{
                          if(cenario == 4){
                  
                          positionAllocMN->Add (Vector(1500, 5656, 1.5));
                          mobilitywifiAp.SetPositionAllocator(positionAllocMN);;
                    
                         
                          wifiStaNodes.Create (nSta);
                          mobilitywifiSta.SetPositionAllocator ("ns3::GridPositionAllocator",
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
  mobilitywifiAp.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                           "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
                           "Bounds", StringValue ("-6000|6000|-6000|6000"));
  mobilitywifiAp.Install (wifiApNodes);
  mobilitywifiSta.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                           "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
                           "Bounds", StringValue ("-6000|6000|-6000|6000"));
  mobilitywifiSta.Install (wifiStaNodes);


      NodeContainer all;
      all.Add(wifiApNodes);
      all.Add(wifiStaNodes);
  std::string phyMode ("DsssRate5_5Mbps");
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
  
  // AP/STA HOC - NETWORK
  NetDeviceContainer staDevice, apDevice;
  Ssid ssid;

  //Network A
  ssid = Ssid ("network-A");
  wifiPhy.Set ("ChannelNumber", UintegerValue (36));
  wifiMac.SetType ("ns3::StaWifiMac",
                   "Ssid", SsidValue (ssid));
  staDevice = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);


  wifiMac.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid),
               "EnableBeaconJitter", BooleanValue (false));
  apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes);



  // //AD HOC - NETWORK
  // wifiMac.SetType ("ns3::AdhocWifiMac");
  // NetDeviceContainer apDevice = wifi.Install (wifiPhy, wifiMac, wifiApNodes);
  // NetDeviceContainer staDevice = wifi.Install (wifiPhy, wifiMac, wifiStaNodes);
      


  // Energy
      srand((unsigned)time(0));
      for (int l=0; l<nAp; ++l)
        {
          aux_energy = rand()%((100-50) + 50); 

          Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
          Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

          energySource->SetInitialEnergy (aux_energy);
          energyModel->SetEnergySource (energySource);
          energySource->AppendDeviceEnergyModel (energyModel);
          energyModel->SetCurrentA (20);

          // aggregate energy source to node
          wifiApNodes.Get(l)->AggregateObject (energySource);
          // energy[l][0] = energySource;
          Energia[l][0] = aux_energy;
        }



  // // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internet;
  internet.Install (wifiApNodes);
  internet.Install (wifiStaNodes);
  
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer apInterface = ipv4.Assign (apDevice);
  Ipv4InterfaceContainer staInterface = ipv4.Assign (staDevice);

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

//

  uint16_t port = 4000;
for (uint16_t u = 0; u<nAp; ++u){
  std::string ipAp = "192.168.1." + std::to_string(u+1);
  port++;
  UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (wifiApNodes);
  apps.Start (Seconds (0.1));
  apps.Stop (Seconds (simTime));

  UdpClientHelper client (Ipv4Address (ipAp.c_str()), port); 
  client.SetAttribute ("MaxPackets", UintegerValue ((uint32_t)(simTime*(1/PacketInterval))));
  client.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  for(uint16_t i = 0; i<wifiStaNodes.GetN(); ++i){
  apps = client.Install (wifiStaNodes.Get(i));
  apps.Start (Seconds (0.1));
  apps.Stop (Seconds (simTime));
  }
}

  // uint16_t port2 = 4001;

  // UdpServerHelper server2 (port2);
  // ApplicationContainer apps2 = server2.Install (wifiApNodes.Get (0));
  // apps2.Start (Seconds (0.1));
  // apps2.Stop (Seconds (simTime));

  // UdpClientHelper client2 (apInterface2.GetAddress (0), port);
  // client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  // client2.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
  // client2.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
  // for(int u = 0; u<nSta; ++u){
  // apps2 = client2.Install (wifiStaNodes2.Get(u));
  // apps2.Start (Seconds (0.1));
  // apps2.Stop (Seconds (simTime));
  // }



//FLOW-MONITOR
  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  std::string gp = std::to_string(cenario);
//Metodo Animation

        AnimationInterface anim (gp + "_group_simulation_1_wifi.xml"); // Mandatory
        
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
      for(int u=0; u<nAp;++u){
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);

        std::string destinIp = "192.168.1." + std::to_string(u+1);
        std::string sourceIp = "192.168.1." + std::to_string(nAp+1);

        
      if (t.destinationAddress == destinIp.c_str() && t.sourceAddress == sourceIp.c_str())
        {
          // for(int l = 0; l<nAp; ++l){
            
            std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
            std::cout<<"Duration  : "<<(i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())<<std::endl;

            std::cout<<"Tx Packets = " << i->second.txPackets<<std::endl;
            std::cout<<"Rx Packets = " << i->second.rxPackets<<std::endl;
            
            Loss[u][0] = i->second.txPackets - i->second.rxPackets;
            std::cout << "Perda de Pacotes: "<< Loss[u][0]<<std::endl;
            
            Vazao[u][0] = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())/1024/1024;
            std::cout << "Vazão: " << Vazao[u][0] << " Mbps\n";
            
            std::cout << "Energia: "<< Energia[u][0] <<std::endl;

            Atraso[u][0] = ((i->second.timeLastRxPacket.GetSeconds()) - (i->second.timeLastTxPacket.GetSeconds()));
            std::cout << "Atraso: "<< Atraso[u][0] <<std::endl;

            atraso2 = i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
            atraso1 = i->second.timeFirstRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds();
            Jitter[u][0] = atraso2 - atraso1;
            std::cout << "Jitter: "<< Jitter[u][0] <<std::endl;
            std::cout << " " <<std::endl;
          // }else{
          //   if(t.sourceAddress == "192.168.1.2" && t.destinationAddress == "192.168.1.1"){
            
          //   std::cout << "Flow " << i->first - 2 << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          //   std::cout<<"Duration  : "<<(i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())<<std::endl;

          //   std::cout<<"Tx Packets = " << i->second.txPackets<<std::endl;
          //   std::cout<<"Rx Packets = " << i->second.rxPackets<<std::endl;
            
          //   Loss[u][0] = i->second.txPackets - i->second.rxPackets;
          //   std::cout << "Perda de Pacotes: "<< Loss[u][0]<<std::endl;
            
          //   Vazao[u][0] = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())/1024/1024;
          //   std::cout << "Vazão: " << Vazao[u][0] << " Mbps\n";
            
          //   std::cout << "Energia: "<< Energia[u][0] <<std::endl;

          //   Atraso[u][0] = ((i->second.timeLastRxPacket.GetSeconds()) - (i->second.timeLastTxPacket.GetSeconds()));
          //   std::cout << "Atraso: "<< Atraso[u][0] <<std::endl;

          //   atraso2 = i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
          //   atraso1 = i->second.timeFirstRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds();
          //   Jitter[u][0] = atraso2 - atraso1;
          //   std::cout << "Jitter: "<< Jitter[u][0] <<std::endl;
          //   std::cout << " " <<std::endl;






          //   }
          }
        }    }

//LÓGICA DE SELEÇÃO
      avalParam(nAp, Vazao, Atraso, Loss, Energia, Jitter);

  Simulator::Destroy();
  return 0;

}
    // Step 2: Analise dos Parametros e avaliação do nó mestre.
      

  void avalParam(int nAp, double** Vazao, double** Atraso, double** Loss, double** Energia, double** Jitterav)
      {
  //Determinar quantidade de parâmetros
          int nPar = 5;

  //Determinar os Parametros utilizados
          double LossPackets [nAp][1];
          double Throughput [nAp][1];
          double Energy [nAp][1];
          double Delay [nAp][1];
          double Jitter [nAp][1];

  //Atribuir 0 a todas as posições da matriz (limpar)
  // int flow[nAp+1][0];

          // for(int l=0; l<nAp; ++l){
          //  LossPackets [nAp][0] = 0;
         //     Throughput [nAp][0] = 0;
          //  Energy [nAp][0] = 0;
          //  Delay [nAp][0] = 0;
          //  Jitter [nAp][0] = 0;
          // }

  //Atribuir valores dos Parâmetros
          srand((unsigned)time(0));
          for (int l = 0; l<nAp; ++l){

            if(Loss[l][0]==0){
            LossPackets [l][0] = 1;
            }else{
            LossPackets [l][0]= Loss[l][0];
            }
                              
            Throughput [l][0]= Vazao[l][0];

            Energy [l][0]= Energia[l][0];

            Delay [l][0]= Atraso[l][0];

            Jitter [l][0]= Jitterav[l][0];
          }
          
          std::cout << "Valor de parametros: " <<std::endl;
          for(int l=0;l<nAp; l++){
          std::cout << " " <<std::endl;
          std::cout << "Nó: " << l <<std::endl;
          std::cout << " " <<std::endl;
          std::cout << "LossPackets " << LossPackets [l][0] << " " <<std::endl;
          std::cout << "Throughput " << Throughput[l][0] << " " <<std::endl;
          std::cout << "Energy " << Energy[l][0] << " " <<std::endl;
          std::cout << "Delay " << Delay[l][0]<< " " <<std::endl;
          std::cout << "Jitter " << Jitter[l][0] << " " <<std::endl;
          }
          
                
          

  //Criar Matriz dos nós de retransmissão
          double mMR [nAp][nPar+1];

          for (int l = 0; l < nAp; ++l)
          {
            for (int c = 0; c <= nPar; ++c)
            {
              switch(c){
                case 0:
                mMR [l][c] = l;
                break;
                case 1:
                mMR [l][c] = LossPackets [l][0];  
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
                mMR [l][c] = Jitter [l][0];
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
  //mMR[0][5]= jitter               |

  double somaPerdaPct = 0;
  double somaVazao = 0;
  double somaEnergia = 0;
  double somaDelay = 0;
  double somaJitter = 0; 

              for (int l = 0; l < nAp; ++l){

                somaPerdaPct =  somaPerdaPct +  mMR[l][1];  
                somaVazao    =  somaVazao    +  mMR[l][2];
                somaEnergia  =  somaEnergia  +  mMR[l][3];
                somaDelay    =  somaDelay    +  mMR[l][4];
                somaJitter   =  somaJitter   +  mMR[l][5];

              }

              for(int l = 0; l<1; l++){
                std::cout << " " <<std::endl;
                std::cout << " " << " " <<std::endl;
                if(somaPerdaPct==0){
                  somaPerdaPct++;
                  std::cout << "Somatória LossPackets " << somaPerdaPct  << " #Alterada " <<std::endl;
                }
                else{
                  std::cout << "Somatória LossPackets " << somaPerdaPct  << " " <<std::endl;
                }
                std::cout << "Somatória Throughput " << somaVazao  << " " <<std::endl;
                std::cout << "Somatória Energy " << somaEnergia  << " " <<std::endl;
                std::cout << "Somatória Delay " << somaDelay  << " " <<std::endl;
                std::cout << "Somatória Jitter " << somaJitter  << " " <<std::endl;
              }


  //somaPerdaPct   | 
//somaVazão      |
//somaEnergia    |  Essas variáveis recebem a soma dos valores dos parametros de todos os nós, 
//somaDelay      |  vamo usar pra normalizar os valores antes de usar na fórmula
//somaAlcance    |

//Na verdade o importante é o que importa ... aqui acontece a porra toda...(emojin da dançarina do whatsapp) 

        float normalmMR[nAp][nPar];
        float FinalScore[nAp][2];


        for (int l = 0; l < nAp; ++l){

        normalmMR[l][1] = (mMR[l][1]/somaPerdaPct);
        normalmMR[l][2] = (mMR[l][2]/somaVazao);
        normalmMR[l][3] = (mMR[l][3]/somaEnergia);
        normalmMR[l][4] = (mMR[l][4]/somaDelay);
        normalmMR[l][5] = (mMR[l][5]/somaJitter);

        FinalScore[l][0]=l;
        FinalScore[l][1]= ((((1-normalmMR[l][1])/(nAp-1))*100)*0.3)+((normalmMR[l][2]*100)*0.25)+((normalmMR[l][3]*100)*0.2)+((((1-normalmMR[l][4])/(nAp-1))*100)*0.15)+((((1-normalmMR[l][5])/(nAp-1))*100)*0.1);
        
        std::cout << "Nó " << l << " Pontuação perda de pacotes " << (((1-normalmMR[l][1])/(nAp-1))*100)*0.3<<std::endl;
        std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25<<std::endl;
        std::cout << "Nó " << l << " Pontuação Energia " << (normalmMR[l][3]*100)*0.2<<std::endl;
        std::cout << "Nó " << l << " Pontuação Delay " << (((1-normalmMR[l][4])/(nAp-1))*100)*0.15<<std::endl;
        std::cout << "Nó " << l << " Pontuação Jitter " << (((1-normalmMR[l][5])/(nAp-1))*100)*0.1<<std::endl;
        std::cout << " " <<std::endl;
        }
         // ((1 - (Pa/Spa))/(Qtn - 1)) * Po  

        // //Escrever de forma agrupada os pontos de cada nó 

        // for (int l = 0; l < nAp; ++l){
                
        //   std::cout << "Nó " << l << " Pontuação perda de pacotes " << ((1-normalmMR[l][1])*100)*0.3<<std::endl;
          
        // }
        // std::cout << " " <<std::endl;
        // for (int l = 0; l < nAp; ++l){
            
        //   std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25 <<std::endl;
          
        // }
        // std::cout << " " <<std::endl;
        // for (int l = 0; l < nAp; ++l){
                    
        //   std::cout << "Nó " << l << " Pontuação energia " << (normalmMR[l][3]*100)*0.2 <<std::endl;
          
        // }
        // std::cout << " " <<std::endl;           
        // for (int l = 0; l < nAp; ++l){
                                
        //   std::cout << "Nó " << l << " Pontuação Delay " << ((normalmMR[l][4])*100)*0.15<<std::endl;
          
        // }
        // std::cout << " " <<std::endl;
        // for (int l = 0; l < nAp; ++l){
                    
        //   std::cout << "Nó " << l << " Pontuação Jitter " << ((1-normalmMR[l][5])*100)*0.1<<std::endl;
                  
        // }
        // std::cout << " " <<std::endl;

//Escrevendo o resultado final (FinalScore) referente à cada Nó
    
        for(int l=0; l<nAp; ++l){           
        std::cout << "Nó " << l << " Pontuação Geral " << FinalScore[l][1] <<std::endl;
        std::cout << "////////////////////////////" <<std::endl;
        }
                

//Imprimir quem foi o Nó fodão picagrossa*** das galaxias

        float maior_ic = 0;
        int id = 0;
        for (int l=0; l<nAp; ++l){
          if(FinalScore[l][1] > maior_ic){
                maior_ic = FinalScore[l][1];
                id = l;
          }
        }
        std::cout << " " <<std::endl;
        std::cout << "Nó de Retransmissão Mestre " << id << " Pontuação: " << maior_ic <<std::endl;

}