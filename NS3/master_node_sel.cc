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
#include "ns3/position-allocator.h"
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
#include "ns3/wifi-phy.h"
#include <stdlib.h>
using namespace ns3;

//Criação das Matrizes
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

void avalParam(int nAll, double** Vazao, double** Atraso, double** Loss, double** Energia, double** Jitterav);

int cenario = 1;
int nQd = 1;

std::string qd = std::to_string(nQd);

int main (int argc, char *argv[]) {

// Simulação 1: Reconhecimento da rede.
//Configurações da rede
    int nAll = 100; 
    double simTime = 1200;
    uint32_t MaxPacketSize = 300;
    double PacketInterval = 0.1;
    uint16_t grid = 500;
    std::string gr = std::to_string(grid);

    bool constant = true;
    bool manual = false;
    
//Variáveis para receber dados do FlowMonitor
    int col = 1;
    double** Vazao = create(nAll*nAll, col);
    double** Atraso = create(nAll*nAll, col);
    double** Loss = create(nAll*nAll, col);
    double** Jitter = create(nAll*nAll, col);
    double atraso1 = 0;
    double atraso2 = 0;
    double** Energia = create(nAll*nAll, col);
    int aux_energy = 0;

  


//Obter posição de arquivo .txt
  double x[nAll][1];
  double y[nAll][1];
  FILE *arqx;
  FILE *arqy;
  char Linhax[nAll];
  char Linhay[nAll];
  char *xAux;
  char *yAux;
  char *resultx;
  char *resulty;
  // Abre um arquivo TEXTO para LEITURA
  arqx = fopen("100_pos_x.txt", "rt");
  arqy = fopen("100_pos_y.txt", "rt");
  if (arqx == NULL || arqy == NULL)  // Se houve erro na abertura
  {
     printf("Problemas na abertura do arquivo\n");
     return(0);
  }
  for(int i = 0; i<nAll; ++i){
      // Lê uma linha (inclusive com o '\n')
      resultx = fgets(Linhax, nAll, arqx);  // o 'fgets' lê até 99 caracteres ou até o '\n'
      if (resultx)  // Se foi possível ler
      xAux = Linhax;
      x[i][0] = atof(xAux);

      // Lê uma linha (inclusive com o '\n')
      resulty = fgets(Linhay, nAll, arqy);  // o 'fgets' lê até 99 caracteres ou até o '\n'
      if (resulty)  // Se foi possível ler
      yAux = Linhay;
      y[i][0] = atof(yAux);
  }




//Criando Nós da Simulação
// Criando Grupo de todos os nós
  NodeContainer wifiAll;
  wifiAll.Create (nAll);

//Setar Mobilidade dos nós
  MobilityHelper mobilitywifiAll;
  if(manual == false){
    mobilitywifiAll.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
                                "X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max="+gr+"]"),
                                "Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max="+gr+"]"));
  }

if(constant == true){
//Constante Mobillity
  mobilitywifiAll.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
}

if(constant == false){
//Random Mobillity
  mobilitywifiAll.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                           "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=100.0]"),
                           "Bounds", StringValue ("-"+gr+"|"+gr+"|-"+gr+"|"+gr+""));
}
  mobilitywifiAll.Install (wifiAll);

  if(manual == true){   
    for(int i = 0; i < nAll; ++i){ 
        wifiAll.Get (i)->GetObject<MobilityModel> ()->SetPosition (Vector (x[i][0], y[i][0], 0));
     } 
  }  
//Configuração da rede
  std::string phyMode ("ErpOfdmRate54Mbps");
  bool verbose = false;

// The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  if (verbose)
    {
      wifi.EnableLogComponents ();  // Turn on all Wifi logging
    }

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
// set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (0) );
// ns-3 supports RadioTap and Prism tracing extensions for 802.11b
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

// Criação do modelo de propagação da rede
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");


  wifiPhy.SetChannel (wifiChannel.Create());

// Adição do mac
  WifiMacHelper wifiMac;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211g);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  
NetDeviceContainer allDevice;

//AD HOC - NETWORK
 
  wifiMac.SetType ("ns3::AdhocWifiMac");
  allDevice = wifi.Install (wifiPhy, wifiMac, wifiAll);



//Atribuindo energia nos dispositivos
      srand((unsigned)time(0));
      for (int l=0; l<nAll; ++l){
          aux_energy = rand()%((100-50) + 50); 

          Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
          Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

          energySource->SetInitialEnergy (aux_energy);
          energyModel->SetEnergySource (energySource);
          energySource->AppendDeviceEnergyModel (energyModel);
          energyModel->SetCurrentA (20);

          // aggregate energy source to node
          wifiAll.Get(l)->AggregateObject (energySource);
          // energy[l][0] = energySource;
          Energia[l][0] = aux_energy;
        }

//Intalação da pilha TCP/IP nos nós
  InternetStackHelper internet;
  internet.Install (wifiAll);
    
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer allInterface = ipv4.Assign (allDevice);
  
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

// Criação da aplicação UDP
// Aplicação para rede AP/STA
  
    uint16_t port = 4000;

      port++;
      UdpServerHelper server (port);
      ApplicationContainer apps = server.Install (wifiAll);
      apps.Start (Seconds (1.0));
      apps.Stop (Seconds (simTime));

for (int ip = 1; ip<=nAll; ++ip){
//Configuração da aplicação   
      std::string ipAp = "192.168.1." + std::to_string(ip);
      UdpClientHelper client (Ipv4Address (ipAp.c_str()), port); 
// UdpClientHelper client (Ipv4Address (allInterface.GetAddress(0)), port); 
      client.SetAttribute ("MaxPackets", UintegerValue ((uint32_t)(simTime*(1/PacketInterval))));
      client.SetAttribute ("Interval", TimeValue (Seconds (PacketInterval)));
      client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

// Intalar Aplicação em todos os nós(Usuários)
      apps = client.Install (wifiAll);
      apps.Start (Seconds (1.0));
      apps.Stop (Seconds (simTime));
    }
      
//FLOW-MONITOR
//Intalar FlowMonitor em todos os Nós
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  std::string gp = std::to_string(nAll);

//Gerar animação
        AnimationInterface anim ("algorithm/quadrante " + qd + "/" + gp + "_master_node_sel.xml"); // Mandatory
        
        for (uint32_t i = 0; i < wifiAll.GetN(); ++i)
        {
          anim.UpdateNodeDescription (wifiAll.Get(i), "Node"); // Optional
          anim.UpdateNodeColor (wifiAll.Get(i), 255, 0, 0); // Coloração
        }

        anim.EnablePacketMetadata (); // Optiona
        anim.EnableIpv4RouteTracking ("algorithm/quadrante " + qd + "/" + gp + "_master_node_sel_route.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional
        anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); //Optional
        anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); //Optional

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

//Imprimir resultados da simulação
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats ();

int u = 0;
double dur = 0;

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i){
      
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      dur = (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds());
      if(dur > 0)
      {        
                    std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                    std::cout<<"Duration  : "<< dur <<std::endl;
                    std::cout<<"Tx Packets = " << i->second.txPackets<<std::endl;
                    std::cout<<"Rx Packets = " << i->second.rxPackets<<std::endl;
                    
                    Loss[u][0] = i->second.txPackets - i->second.rxPackets;
                    std::cout << "Perda de Pacotes: "<< Loss[u][0]<<std::endl;
                    
                    Vazao[u][0] = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds())/1024;
                    std::cout << "Vazão: " << Vazao[u][0] << " Kbps\n";

                    std::cout << "Energia: "<< Energia[u][0] <<std::endl;

                    Atraso[u][0] = ((i->second.timeLastRxPacket.GetSeconds()) - (i->second.timeLastTxPacket.GetSeconds()));
                    std::cout << "Atraso: "<< Atraso[u][0] <<std::endl;

                    atraso2 = i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
                    atraso1 = i->second.timeFirstRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds();
                    Jitter[u][0] = atraso2 - atraso1;
                    std::cout << "Jitter: "<< Jitter[u][0] <<std::endl;
                    u++;
      }
    }
      
//LÓGICA DE SELEÇÃO
      avalParam(nAll, Vazao, Atraso, Loss, Energia, Jitter);

  Simulator::Destroy();
  return 0;

}
// Step 2: Analise dos Parametros e avaliação do nó mestre.
      

void avalParam(int nAll, double** Vazao, double** Atraso, double** Loss, double** Energia, double** Jitterav){
//Determinar quantidade de parâmetros
  int nPar = 5;

  //Determinar os Parametros utilizados
  double LossPackets [nAll][1];
  double Throughput [nAll][1];
  double Energy [nAll][1];
  double Delay [nAll][1];
  double Jitter [nAll][1];

//Atribuir 0 a todas as posições da matriz (limpar)
  for(int l=0; l<nAll; ++l){
     LossPackets [nAll][0] = 0;
     Throughput [nAll][0] = 0;
     Energy [nAll][0] = 0;
     Delay [nAll][0] = 0;
     Jitter [nAll][0] = 0;
  }

//Atribuir valores dos Parâmetros
  srand((unsigned)time(0));
  for (int l = 0; l<nAll; ++l){

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
  for(int l=0;l<nAll; l++){
      std::cout << " " <<std::endl;
      std::cout << "Nó: " << l <<std::endl;
      std::cout << "LossPackets " << LossPackets [l][0] << " " <<std::endl;
      std::cout << "Throughput " << Throughput[l][0] << " " <<std::endl;
      std::cout << "Energy " << Energy[l][0] << " " <<std::endl;
      std::cout << "Delay " << Delay[l][0]<< " " <<std::endl;
      std::cout << "Jitter " << Jitter[l][0] << " " <<std::endl;
  }
          
//Criar Matriz dos nós de retransmissão
//mMR[0][1]= perda de pacotes      |    
//mMR[0][2]= vazão                 |
//mMR[0][3]= energia               |   Médias/Valores "brutos", ainda nao normalizadas
//mMR[0][4]= delay                 |
//mMR[0][5]= jitter                |

  double mMR [nAll][nPar+1];

  for (int l = 0; l < nAll; ++l)
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

//Normalização dos resultados
//somaPerdaPct   | 
//somaVazão      |
//somaEnergia    |  Essas variáveis recebem a soma dos valores dos parametros de todos os nós, 
//somaDelay      |  vamos usar pra normalizar os valores antes de usar na fórmula
//somaAlcance    |

  double somaPerdaPct = 0;
  double somaVazao = 0;
  double somaEnergia = 0;
  double somaDelay = 0;
  double somaJitter = 0; 

  for (int l = 0; l < nAll; ++l){
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
          somaPerdaPct--;
          std::cout << "Somatória LossPackets " << somaPerdaPct  << " #Alterada " <<std::endl;
      }else{
          std::cout << "Somatória LossPackets " << somaPerdaPct  << " " <<std::endl;
      }
      std::cout << "Somatória Throughput " << somaVazao  << " " <<std::endl;
      std::cout << "Somatória Energy " << somaEnergia  << " " <<std::endl;
      std::cout << "Somatória Delay " << somaDelay  << " " <<std::endl;
      std::cout << "Somatória Jitter " << somaJitter  << " " <<std::endl;
  }
std::cout << " " <<std::endl;

  float normalmMR[nAll][nPar];
  float FinalScore[nAll][2];

  for (int l = 0; l < nAll; ++l){
      normalmMR[l][1] = (mMR[l][1]/somaPerdaPct);
      normalmMR[l][2] = (mMR[l][2]/somaVazao);
      normalmMR[l][3] = (mMR[l][3]/somaEnergia);
      normalmMR[l][4] = (mMR[l][4]/somaDelay);
      normalmMR[l][5] = (mMR[l][5]/somaJitter);
// Atribuição da pontuação de acordo com o peso de cada atributo
      std::cout << "Nó " << l << " Pontuação perda de pacotes " << (((1-normalmMR[l][1])/(nAll-1))*100)*0.3<<std::endl;
      std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25<<std::endl;
      std::cout << "Nó " << l << " Pontuação Energia " << (normalmMR[l][3]*100)*0.2<<std::endl;
      std::cout << "Nó " << l << " Pontuação Delay " << (((1-normalmMR[l][4])/(nAll-1))*100)*0.15<<std::endl;
      std::cout << "Nó " << l << " Pontuação Jitter " << (((1-normalmMR[l][5])/(nAll-1))*100)*0.1<<std::endl;
      std::cout << " " <<std::endl;
      FinalScore[l][0] = l;
      FinalScore[l][1]= ((((1-normalmMR[l][1])/(nAll-1))*100)*0.3)+((normalmMR[l][2]*100)*0.25)+((normalmMR[l][3]*100)*0.2)+((((1-normalmMR[l][4])/(nAll-1))*100)*0.15)+((((1-normalmMR[l][5])/(nAll-1))*100)*0.1);
  }
//Escrevendo (FinalScore) referente à cada Nó
  double sum = 0;
//Escrevendo (FinalScore) referente à cada Nó
  for(int l=0; l<nAll; ++l){           
      std::cout << "Nó " << l << " Pontuação Geral " << FinalScore[l][1] <<std::endl;
      std::cout << "////////////////////////////" <<std::endl;
      sum = FinalScore[l][1] + sum;
    
  }
  std::cout << " " <<std::endl;
  std::cout << " " <<std::endl;
  std::cout << "Somatória da Pontuação " << sum <<std::endl;
    
//Imprimir Nó Mestre
  float maior_ic = 0;
  int id = 0;
  for (int l=0; l<nAll; ++l){
      if(FinalScore[l][1] > maior_ic){
          maior_ic = FinalScore[l][1];
          id = l;
      }
  }
  std::cout << " " <<std::endl;
  std::cout << "Nó de Retransmissão Mestre " << id << " Pontuação: " << maior_ic <<std::endl;
double M = 0;
int Id = 0;

for(int i = 0; i<nAll; ++i)
{
    for(int k = i; k<nAll; ++k)
    {
      if(FinalScore[i][1] > FinalScore[k][1])
      {
        M = FinalScore[i][1];
        FinalScore[i][1] = FinalScore[k][1];
        FinalScore[k][1] = M;

        Id = FinalScore[i][0];
        FinalScore[i][0] = FinalScore[k][0];
        FinalScore[k][0] = Id;

      }
    }

  }
     std::cout << " " <<std::endl;
     std::cout << "Ranking" <<std::endl;
  for(int i = 0; i<nAll; ++i)
  {
     std::cout << "Nós de Retransmissão " << FinalScore[i][0] << " Pontuação: " << FinalScore[i][1] <<std::endl;
  }
}