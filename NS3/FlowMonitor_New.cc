/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 IITP RAS
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
 * Authors: Pavel Boyko <boyko@iitp.ru>
 */

/*
 * Classical hidden terminal problem and its RTS/CTS solution.
 *
 * Topology: [node 0] <-- -50 dB --> [node 1] <-- -50 dB --> [node 2]
 *
 * This example illustrates the use of
 *  - Wifi in ad-hoc mode
 *  - Matrix propagation loss model
 *  - Use of OnOffApplication to generate CBR stream
 *  - IP flow monitor
 */

#include "ns3/core-module.h"
#include "ns3/propagation-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-module.h"

#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"
#include "string"
#include "stdio.h"
#include "iostream"

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

void avalParam(int nAp, double Vazao[][1], double Atraso[][1], double Lost[][1], double Energia[][1]);


int main (int argc, char **argv)
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  int nNode = 3;
  int nAp = nNode - 1;
  // int col = 1;
  int aux_energy = 0;

  double Vazao [nAp][1];
  double Lost [nAp][1];
  double Atraso [nAp][1];
  double Energia [nAp][1];


  // 1. Create 3 nodes
  NodeContainer nodes;
  nodes.Create (nNode);

  // 2. Place nodes somehow, this is required by every wireless simulation
  for (size_t i = 0; i < 3; ++i)
    {
      nodes.Get (i)->AggregateObject (CreateObject<ConstantPositionMobilityModel> ());
    }

  // 3. Create propagation loss matrix
  Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel> ();
  lossModel->SetDefaultLoss (200); // set default loss to 200 dB (no link)
  lossModel->SetLoss (nodes.Get (0)->GetObject<MobilityModel> (), nodes.Get (1)->GetObject<MobilityModel> (), 50); // set symmetric loss 0 <-> 1 to 50 dB
  lossModel->SetLoss (nodes.Get (2)->GetObject<MobilityModel> (), nodes.Get (1)->GetObject<MobilityModel> (), 50); // set symmetric loss 2 <-> 1 to 50 dB

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
  WifiMacHelper wifiMac;
  wifiMac.SetType ("ns3::AdhocWifiMac"); // use ad-hoc MAC
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, nodes);

  // uncomment the following to have athstats output
  // AthstatsHelper athstats;
  // athstats.EnableAthstats(enableCtsRts ? "rtscts-athstats-node" : "basic-athstats-node" , nodes);

  // uncomment the following to have pcap output
  // wifiPhy.EnablePcap (enableCtsRts ? "rtscts-pcap-node" : "basic-pcap-node" , nodes);
  
  //Energy
      srand((unsigned)time(0));
      for (int l=0; l<nNode; ++l)
        {
          aux_energy = rand()%(100);
          Ptr<BasicEnergySource> energySource = CreateObject<BasicEnergySource>();
          Ptr<SimpleDeviceEnergyModel> energyModel = CreateObject<SimpleDeviceEnergyModel>();

          energySource->SetInitialEnergy (aux_energy);
          energyModel->SetEnergySource (energySource);
          energySource->AppendDeviceEnergyModel (energyModel);
          energyModel->SetCurrentA (20);

          // aggregate energy source to node
          nodes.Get(l)->AggregateObject (energySource);
          // energy[l][0] = energySource;
          Energia[l][0] = aux_energy;
        }



  // 6. Install TCP/IP stack & assign IP addresses
  InternetStackHelper internet;
  internet.Install (nodes);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  ipv4.Assign (devices);

  // 7. Install applications: two CBR streams each saturating the channel
  ApplicationContainer cbrApps;
  uint16_t cbrPort = 12345;
  OnOffHelper onOffHelper ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address ("10.0.0.2"), cbrPort));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (1400));
  onOffHelper.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  // flow 1:  node 0 -> node 1
  onOffHelper.SetAttribute ("DataRate", StringValue ("3000000bps"));
  onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.000000)));
  cbrApps.Add (onOffHelper.Install (nodes.Get (0)));

  // flow 2:  node 2 -> node 1
  /** \internal
   * The slightly different start times and data rates are a workaround
   * for \bugid{388} and \bugid{912}
   */
  onOffHelper.SetAttribute ("DataRate", StringValue ("3001100bps"));
  onOffHelper.SetAttribute ("StartTime", TimeValue (Seconds (1.001)));
  cbrApps.Add (onOffHelper.Install (nodes.Get (2)));

  /** \internal
   * We also use separate UDP applications that will send a single
   * packet before the CBR flows start.
   * This is a workaround for the lack of perfect ARP, see \bugid{187}
   */
  uint16_t  echoPort = 9;
  UdpEchoClientHelper echoClientHelper (Ipv4Address ("10.0.0.2"), echoPort);
  echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.1)));
  echoClientHelper.SetAttribute ("PacketSize", UintegerValue (10));
  ApplicationContainer pingApps;

  // again using different start times to workaround Bug 388 and Bug 912
  echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.001)));
  pingApps.Add (echoClientHelper.Install (nodes.Get (0)));
  echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.006)));
  pingApps.Add (echoClientHelper.Install (nodes.Get (2)));

  // 8. Install FlowMonitor on all nodes
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  // 9. Run simulation for 10 seconds
  Simulator::Stop (Seconds (10));
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
              if (t.sourceAddress == "10.0.0.1")
                {
                  // Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
                  std::cout << " " <<std::endl;
                  std::cout << "Flow: " << i->first - 2 << ". (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                   std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
                  // std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
                  // std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
                   std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  // std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
                  Lost[0][0] =  i->second.txPackets - i->second.rxPackets;
                  std::cout << "  Lost Packets: " << Lost[0][0] << "\n";
                  Vazao[0][0] =  i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000;
                  std::cout << "  Throughput: " << Vazao[0][0] << " Mbps\n";
                  std::cout << "  Energy: " << Energia[0][0] << "\n";
                  Atraso[0][0] =  i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
                  std::cout << "  Atraso: " << Atraso[0][0] << "\n";
                  
                  
                }else{
                  if(t.sourceAddress == "10.0.0.3"){
                  std::cout << " " <<std::endl;
                  std::cout << "Flow: " << i->first - 2 << ". (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
                  std::cout << "  Tx Packets: " << i->second.txPackets << "\n";
                  // std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
                  // std::cout << "  TxOffered:  " << i->second.txBytes * 8.0 / 9.0 / 1000 / 1000  << " Mbps\n";
                  std::cout << "  Rx Packets: " << i->second.rxPackets << "\n";
                  // std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
                  Lost[1][0] =  i->second.txPackets - i->second.rxPackets;
                  std::cout << "  Lost Packets: " << Lost[1][0] << "\n";
                  Vazao[1][0] =  i->second.rxBytes * 8.0 / 9.0 / 1000 / 1000;
                  std::cout << "  Throughput: " << Vazao[1][0] << " Mbps\n";
                  std::cout << "  Energy: " << Energia[1][0] << "\n";
                  Atraso[1][0] =  i->second.timeLastRxPacket.GetSeconds()-i->second.timeLastTxPacket.GetSeconds();
                  std::cout << "  Atraso: " << Atraso[1][0] << "\n";


                  }
                }
          }
        
  avalParam(nAp, Vazao, Atraso, Lost, Energia);
  // 11. Cleanup
  Simulator::Destroy ();



  return 0;
}



  void avalParam(int nAp, double Vazao[][1], double Atraso[][1], double Lost[][1], double Energia[][1])
      {
//Determinar quantidade de parâmetros
        int nPar = 5;

//Determinar os Parametros utilizados
        float LostPackets [nAp][1];
        float Throughput [nAp][1];
        float Energy [nAp][1];
        float Delay [nAp][1];
        float Range [nAp][1];

//Atribuir 0 a todas as posições da matriz (limpar)
// int flow[nAp+1][0];

        for(int l=0; l<nAp; ++l){
          LostPackets [nAp][0] = 0;
          Throughput [nAp][0] = 0;
          Energy [nAp][0] = 0;
          Delay [nAp][0] = 0;
          Range [nAp][0] = 0;
        }

//Atribuir valores dos Parâmetros
        srand((unsigned)time(0));
        for (int l = 0; l<nAp; ++l){

          LostPackets [l][0]= Lost[l][0];
                            
          Throughput [l][0]= Vazao[l][0];

          Energy [l][0]= Energia[l][0];

          Delay [l][0]= Atraso[l][0];

          Range [l][0]= rand()%(100);
        }


//Escreve os valores recivuto   
        std::cout << "Valor de parametros: " <<std::endl;
        for(int l=0;l<nAp; l++){
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
        float mMR [nAp][nPar+1];

        for (int l = 0; l < nAp; ++l)
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

float somaPerdaPct = 0;
float somaVazao = 0;
float somaEnergia = 0;
float somaDelay = 0;
float somaAlcance = 0; 

            for (int l = 0; l < nAp; ++l){

              somaPerdaPct =  somaPerdaPct +  mMR[l][1];  
              somaVazao    =  somaVazao    +  mMR[l][2];
              somaEnergia  =  somaEnergia  +  mMR[l][3];
              somaDelay    =  somaDelay    +  mMR[l][4];
              somaAlcance  =  somaAlcance  +  mMR[l][5];

            }
            for(int l = 0; l<1; l++){
              std::cout << " " <<std::endl;
              std::cout << " " << " " <<std::endl;
              std::cout << "Soma de LostPackets " << somaPerdaPct  << " " <<std::endl;
              std::cout << "Soma de Throughput " << somaVazao  << " " <<std::endl;
              std::cout << "Soma de Energy " << somaEnergia  << " " <<std::endl;
              std::cout << "Soma de Delay " << somaDelay  << " " <<std::endl;
              std::cout << "Soma de Range " << somaAlcance  << " " <<std::endl;
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
        normalmMR[l][5] = (mMR[l][5]/somaAlcance);

        FinalScore[l][0]=l;
        FinalScore[l][1]=((((1-normalmMR[l][1])/(nAp-1))*0.3)+(normalmMR[l][2]*0.25)+(normalmMR[l][3]*0.2)+(((1-normalmMR[l][4])/(nAp-1))*0.15)+(((1-normalmMR[l][5])/(nAp-1))*0.1))*100;
        
        // std::cout << "Nó " << l << " Pontuação perda de pacotes " << ((1-normalmMR[l][1])*100)*0.3<<std::endl;
        // std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25<<std::endl;
        // std::cout << "Nó " << l << " Pontuação Energia " << (normalmMR[l][3]*100)*0.2<<std::endl;
        // std::cout << "Nó " << l << " Pontuação Delay " << ((1-normalmMR[l][4])*100)*0.15<<std::endl;
        // std::cout << "Nó " << l << " Pontuação Alcance " << ((1-normalmMR[l][5])*100)*0.1<<std::endl;
        // std::cout << " " <<std::endl;
        }

//Escrever de forma agrupada os pontos de cada nó 

        for (int l = 0; l < nAp; ++l){
                
          std::cout << "Nó " << l << " Pontuação perda de pacotes " << ((1-normalmMR[l][1])*100)*0.3<<std::endl;
          
        }
        std::cout << " " <<std::endl;
        for (int l = 0; l < nAp; ++l){
            
          std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25 <<std::endl;
          
        }
        std::cout << " " <<std::endl;
        for (int l = 0; l < nAp; ++l){
                    
          std::cout << "Nó " << l << " Pontuação energia " << (normalmMR[l][3]*100)*0.2 <<std::endl;
          
        }
        std::cout << " " <<std::endl;           
        for (int l = 0; l < nAp; ++l){
                                
          std::cout << "Nó " << l << " Pontuação Delay " << ((1-normalmMR[l][4])*100)*0.15<<std::endl;
          
        }
        std::cout << " " <<std::endl;
        for (int l = 0; l < nAp; ++l){
                    
          std::cout << "Nó " << l << " Pontuação Alcance " << ((1-normalmMR[l][5])*100)*0.1<<std::endl;
                  
        }
        std::cout << " " <<std::endl;


//Escrevendo o resultado final (FinalScore) referente à cada Nó
    
        for(int l=0; l<nAp; ++l){           
        std::cout << "Nó " << FinalScore[l][0] << " Pontuação Geral " << FinalScore[l][1] <<std::endl;
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
    

//***tome esse referência kkkkk
