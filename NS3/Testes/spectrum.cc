/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 MIRKO BANCHI
 * Copyright (c) 2015 University of Washington
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
 * Authors: Mirko Banchi <mk.banchi@gmail.com>
 *          Sebastien Deronne <sebastien.deronne@gmail.com>
 *          Tom Henderson <tomhend@u.washington.edu>
 *
 * Adapted from ht-wifi-network.cc example
 */

#include <iomanip>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/internet-module.h"
#include <cstdio>
#include <cstdlib>
// This is a simple example of an IEEE 802.11n Wi-Fi network.
//
// The main use case is to enable and test SpectrumWifiPhy vs YansWifiPhy
// for packet error ratio
//
// Network topology:
//
//  Wi-Fi 192.168.1.0
//
//   STA                  AP
//    * <-- distance -->  *
//    |                   |
//    n1                  n2
//
// Users may vary the following command-line arguments in addition to the
// attributes, global values, and default values typically available:
//
//    --simulationTime:  Simulation time in seconds [10]
//    --udp:             UDP if set to 1, TCP otherwise [true]
//    --distance:        meters separation between nodes [50]
//    --index:           restrict index to single value between 0 and 31 [256]
//    --wifiType:        select ns3::SpectrumWifiPhy or ns3::YansWifiPhy [ns3::SpectrumWifiPhy]
//    --errorModelType:  select ns3::NistErrorRateModel or ns3::YansErrorRateModel [ns3::NistErrorRateModel]
//    --enablePcap:      enable pcap output [false]
//
// By default, the program will step through 32 index values, corresponding
// to the following MCS, channel width, and guard interval combinations:
//   index 0-7:    MCS 0-7, long guard interval, 20 MHz channel
//   index 8-15:   MCS 0-7, short guard interval, 20 MHz channel
//   index 16-23:  MCS 0-7, long guard interval, 40 MHz channel
//   index 24-31:  MCS 0-7, short guard interval, 40 MHz channel
// and send UDP for 10 seconds using each MCS, using the SpectrumWifiPhy and the
// NistErrorRateModel, at a distance of 50 meters.  The program outputs
// results such as:
//
// wifiType: ns3::SpectrumWifiPhy distance: 50m; time: 10; TxPower: 1 dBm (1.3 mW)
// index   MCS  Rate (Mb/s) Tput (Mb/s) Received Signal (dBm) Noise (dBm) SNR (dB)
//     0     0      6.50        5.77    7414      -79.71      -93.97       14.25
//     1     1     13.00       11.58   14892      -79.71      -93.97       14.25
//     2     2     19.50       17.39   22358      -79.71      -93.97       14.25
//     3     3     26.00       22.96   29521      -79.71      -93.97       14.25
//   ...
//

using namespace ns3;

// Global variables for use in callbacks.
double g_signalDbmAvg;
double g_noiseDbmAvg;
uint32_t g_samples;


// float** createTable(int rows, int columns){
//     int** table = new float*[rows];
//     for(int i = 0; i < rows; i++) {
//         table[i] = new float[columns]; 
//         for(int j = 0; j < columns; j++){ table[i][j] = (i+j); }// sample set value;    
//     }
//     return table;
// }


void MonitorSniffRx (Ptr<const Packet> packet,
                     uint16_t channelFreqMhz,
                     WifiTxVector txVector,
                     MpduInfo aMpdu,
                     SignalNoiseDbm signalNoise)

{
  g_samples++;
  g_signalDbmAvg += ((signalNoise.signal - g_signalDbmAvg) / g_samples);
  g_noiseDbmAvg += ((signalNoise.noise - g_noiseDbmAvg) / g_samples);
}

void avalParam(int nAp, float vazao_aval[][1]);

NS_LOG_COMPONENT_DEFINE ("WifiSpectrumPerExample");


int main (int argc, char *argv[])
{
  bool udp = true;
  double distance = 50;
  double simulationTime = 10; //seconds
  uint16_t index = 256;
  int nAp = 1;

  float vazao [nAp][1];


  std::string wifiType = "ns3::SpectrumWifiPhy";
  std::string errorModelType = "ns3::NistErrorRateModel";
  bool enablePcap = false;
  const uint32_t tcpPacketSize = 1448;

  CommandLine cmd;
  cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
  cmd.AddValue ("udp", "UDP if set to 1, TCP otherwise", udp);
  cmd.AddValue ("distance", "meters separation between nodes", distance);
  cmd.AddValue ("index", "restrict index to single value between 0 and 31", index);
  cmd.AddValue ("wifiType", "select ns3::SpectrumWifiPhy or ns3::YansWifiPhy", wifiType);
  cmd.AddValue ("errorModelType", "select ns3::NistErrorRateModel or ns3::YansErrorRateModel", errorModelType);
  cmd.AddValue ("enablePcap", "enable pcap output", enablePcap);
  cmd.Parse (argc,argv);

  uint16_t startIndex = 0;
  uint16_t stopIndex = 31;
  if (index < 32)
    {
      startIndex = index;
      stopIndex = index;
    }

  std::cout << "wifiType: " << wifiType << " distance: " << distance << "m; time: " << simulationTime << "; TxPower: 1 dBm (1.3 mW)" << std::endl;
  std::cout << std::setw (5) << "index" <<
    std::setw (6) << "MCS" <<
    std::setw (13) << "Rate (Mb/s)" <<
    std::setw (12) << "Tput (Mb/s)" <<
    std::setw (10) << "Received " <<
    std::setw (12) << "Signal (dBm)" <<
    std::setw (12) << "Noise (dBm)" <<
    std::setw (9) << "SNR (dB)" <<
    std::endl;
  for (uint16_t i = startIndex; i <= stopIndex; i++)
    {
      uint32_t payloadSize;
      if (udp)
        {
          payloadSize = 972; // 1000 bytes IPv4
        }
      else
        {
          payloadSize = 1448; // 1500 bytes IPv6
          Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));
        }

      NodeContainer wifiStaNode;
      wifiStaNode.Create (1);
      NodeContainer wifiApNode;
      wifiApNode.Create (nAp);

      YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
      SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default ();
      if (wifiType == "ns3::YansWifiPhy")
        {
          YansWifiChannelHelper channel;
          channel.AddPropagationLoss ("ns3::FriisPropagationLossModel",
                                      "Frequency", DoubleValue (5.180e9));
          channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
          phy.SetChannel (channel.Create ());
          phy.Set ("TxPowerStart", DoubleValue (1)); // dBm (1.26 mW)
          phy.Set ("TxPowerEnd", DoubleValue (1));
          phy.Set ("Frequency", UintegerValue (5180));

          if (i <= 7)
            {
              phy.Set ("ShortGuardEnabled", BooleanValue (false));
              phy.Set ("ChannelWidth", UintegerValue (20));
            }
          else if (i > 7 && i <= 15)
            {
              phy.Set ("ShortGuardEnabled", BooleanValue (true));
              phy.Set ("ChannelWidth", UintegerValue (20));
            }
          else if (i > 15 && i <= 23)
            {
              phy.Set ("ShortGuardEnabled", BooleanValue (false));
              phy.Set ("ChannelWidth", UintegerValue (40));
            }
          else
            {
              phy.Set ("ShortGuardEnabled", BooleanValue (true));
              phy.Set ("ChannelWidth", UintegerValue (40));
            }
        }
      else if (wifiType == "ns3::SpectrumWifiPhy")
        {
          //Bug 2460: CcaMode1Threshold default should be set to -62 dBm when using Spectrum
          Config::SetDefault ("ns3::WifiPhy::CcaMode1Threshold", DoubleValue (-62.0));

          Ptr<MultiModelSpectrumChannel> spectrumChannel
            = CreateObject<MultiModelSpectrumChannel> ();
          Ptr<FriisPropagationLossModel> lossModel
            = CreateObject<FriisPropagationLossModel> ();
          lossModel->SetFrequency (5.180e9);
          spectrumChannel->AddPropagationLossModel (lossModel);

          Ptr<ConstantSpeedPropagationDelayModel> delayModel
            = CreateObject<ConstantSpeedPropagationDelayModel> ();
          spectrumChannel->SetPropagationDelayModel (delayModel);

          spectrumPhy.SetChannel (spectrumChannel);
          spectrumPhy.SetErrorRateModel (errorModelType);
          spectrumPhy.Set ("Frequency", UintegerValue (5180));
          spectrumPhy.Set ("TxPowerStart", DoubleValue (1)); // dBm  (1.26 mW)
          spectrumPhy.Set ("TxPowerEnd", DoubleValue (1));

          if (i <= 7)
            {
              spectrumPhy.Set ("ShortGuardEnabled", BooleanValue (false));
              spectrumPhy.Set ("ChannelWidth", UintegerValue (20));
            }
          else if (i > 7 && i <= 15)
            {
              spectrumPhy.Set ("ShortGuardEnabled", BooleanValue (true));
              spectrumPhy.Set ("ChannelWidth", UintegerValue (20));
            }
          else if (i > 15 && i <= 23)
            {
              spectrumPhy.Set ("ShortGuardEnabled", BooleanValue (false));
              spectrumPhy.Set ("ChannelWidth", UintegerValue (40));
            }
          else
            {
              spectrumPhy.Set ("ShortGuardEnabled", BooleanValue (true));
              spectrumPhy.Set ("ChannelWidth", UintegerValue (40));
            }
        }
      else
        {
          NS_FATAL_ERROR ("Unsupported WiFi type " << wifiType);
        }


      WifiHelper wifi;
      wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
      WifiMacHelper mac;

      Ssid ssid = Ssid ("ns380211n");

      double datarate = 0;
      StringValue DataRate;
      if (i == 0)
        {
          DataRate = StringValue ("HtMcs0");
          datarate = 6.5;
        }
      else if (i == 1)
        {
          DataRate = StringValue ("HtMcs1");
          datarate = 13;
        }
      else if (i == 2)
        {
          DataRate = StringValue ("HtMcs2");
          datarate = 19.5;
        }
      else if (i == 3)
        {
          DataRate = StringValue ("HtMcs3");
          datarate = 26;
        }
      else if (i == 4)
        {
          DataRate = StringValue ("HtMcs4");
          datarate = 39;
        }
      else if (i == 5)
        {
          DataRate = StringValue ("HtMcs5");
          datarate = 52;
        }
      else if (i == 6)
        {
          DataRate = StringValue ("HtMcs6");
          datarate = 58.5;
        }
      else if (i == 7)
        {
          DataRate = StringValue ("HtMcs7");
          datarate = 65;
        }
      else if (i == 8)
        {
          DataRate = StringValue ("HtMcs0");
          datarate = 7.2;
        }
      else if (i == 9)
        {
          DataRate = StringValue ("HtMcs1");
          datarate = 14.4;
        }
      else if (i == 10)
        {
          DataRate = StringValue ("HtMcs2");
          datarate = 21.7;
        }
      else if (i == 11)
        {
          DataRate = StringValue ("HtMcs3");
          datarate = 28.9;
        }
      else if (i == 12)
        {
          DataRate = StringValue ("HtMcs4");
          datarate = 43.3;
        }
      else if (i == 13)
        {
          DataRate = StringValue ("HtMcs5");
          datarate = 57.8;
        }
      else if (i == 14)
        {
          DataRate = StringValue ("HtMcs6");
          datarate = 65;
        }
      else if (i == 15)
        {
          DataRate = StringValue ("HtMcs7");
          datarate = 72.2;
        }
      else if (i == 16)
        {
          DataRate = StringValue ("HtMcs0");
          datarate = 13.5;
        }
      else if (i == 17)
        {
          DataRate = StringValue ("HtMcs1");
          datarate = 27;
        }
      else if (i == 18)
        {
          DataRate = StringValue ("HtMcs2");
          datarate = 40.5;
        }
      else if (i == 19)
        {
          DataRate = StringValue ("HtMcs3");
          datarate = 54;
        }
      else if (i == 20)
        {
          DataRate = StringValue ("HtMcs4");
          datarate = 81;
        }
      else if (i == 21)
        {
          DataRate = StringValue ("HtMcs5");
          datarate = 108;
        }
      else if (i == 22)
        {
          DataRate = StringValue ("HtMcs6");
          datarate = 121.5;
        }
      else if (i == 23)
        {
          DataRate = StringValue ("HtMcs7");
          datarate = 135;
        }
      else if (i == 24)
        {
          DataRate = StringValue ("HtMcs0");
          datarate = 15;
        }
      else if (i == 25)
        {
          DataRate = StringValue ("HtMcs1");
          datarate = 30;
        }
      else if (i == 26)
        {
          DataRate = StringValue ("HtMcs2");
          datarate = 45;
        }
      else if (i == 27)
        {
          DataRate = StringValue ("HtMcs3");
          datarate = 60;
        }
      else if (i == 28)
        {
          DataRate = StringValue ("HtMcs4");
          datarate = 90;
        }
      else if (i == 29)
        {
          DataRate = StringValue ("HtMcs5");
          datarate = 120;
        }
      else if (i == 30)
        {
          DataRate = StringValue ("HtMcs6");
          datarate = 135;
        }
      else
        {
          DataRate = StringValue ("HtMcs7");
          datarate = 150;
        }

      wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", DataRate,
                                    "ControlMode", DataRate);

      NetDeviceContainer staDevice;
      NetDeviceContainer apDevice;

      if (wifiType == "ns3::YansWifiPhy")
        {
          mac.SetType ("ns3::StaWifiMac",
                       "Ssid", SsidValue (ssid),
                       "ActiveProbing", BooleanValue (false));
          staDevice = wifi.Install (phy, mac, wifiStaNode);
          mac.SetType ("ns3::ApWifiMac",
                       "Ssid", SsidValue (ssid));
          apDevice = wifi.Install (phy, mac, wifiApNode);

        }
      else if (wifiType == "ns3::SpectrumWifiPhy")
        {
          mac.SetType ("ns3::StaWifiMac",
                       "Ssid", SsidValue (ssid),
                       "ActiveProbing", BooleanValue (false));
          staDevice = wifi.Install (spectrumPhy, mac, wifiStaNode);
          mac.SetType ("ns3::ApWifiMac",
                       "Ssid", SsidValue (ssid));
          apDevice = wifi.Install (spectrumPhy, mac, wifiApNode);
        }

      // mobility.
      MobilityHelper mobility;
      Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

      positionAlloc->Add (Vector (0.0, 0.0, 0.0));
      positionAlloc->Add (Vector (distance, 0.0, 0.0));
      mobility.SetPositionAllocator (positionAlloc);

      mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

      mobility.Install (wifiApNode);
      mobility.Install (wifiStaNode);

      /* Internet stack*/
      InternetStackHelper stack;
      stack.Install (wifiApNode);
      stack.Install (wifiStaNode);

      Ipv4AddressHelper address;
      address.SetBase ("192.168.1.0", "255.255.255.0");
      Ipv4InterfaceContainer staNodeInterface;
      Ipv4InterfaceContainer apNodeInterface;

      staNodeInterface = address.Assign (staDevice);
      apNodeInterface = address.Assign (apDevice);

      /* Setting applications */
      ApplicationContainer serverApp;
      if (udp)
        {
          //UDP flow
          uint16_t port = 9;
          UdpServerHelper server (port);
          serverApp = server.Install (wifiStaNode.Get (0));
          serverApp.Start (Seconds (0.0));
          serverApp.Stop (Seconds (simulationTime + 1));

          UdpClientHelper client (staNodeInterface.GetAddress (0), port);
          client.SetAttribute ("MaxPackets", UintegerValue (4294967295u));
          client.SetAttribute ("Interval", TimeValue (Time ("0.00001"))); //packets/s
          client.SetAttribute ("PacketSize", UintegerValue (payloadSize));
          ApplicationContainer clientApp = client.Install (wifiApNode.Get (0));
          clientApp.Start (Seconds (1.0));
          clientApp.Stop (Seconds (simulationTime + 1));
        }
      else
        {
          //TCP flow
          uint16_t port = 50000;
          Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));
          PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", localAddress);
          serverApp = packetSinkHelper.Install (wifiStaNode.Get (0));
          serverApp.Start (Seconds (0.0));
          serverApp.Stop (Seconds (simulationTime + 1));

          OnOffHelper onoff ("ns3::TcpSocketFactory", Ipv4Address::GetAny ());
          onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
          onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
          onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));
          onoff.SetAttribute ("DataRate", DataRateValue (1000000000)); //bit/s
          AddressValue remoteAddress (InetSocketAddress (staNodeInterface.GetAddress (0), port));
          onoff.SetAttribute ("Remote", remoteAddress);
          ApplicationContainer clientApp = onoff.Install (wifiApNode.Get (0));
          clientApp.Start (Seconds (1.0));
          clientApp.Stop (Seconds (simulationTime + 1));
        }

      Config::ConnectWithoutContext ("/NodeList/0/DeviceList/*/Phy/MonitorSnifferRx", MakeCallback (&MonitorSniffRx));

      if (enablePcap)
        {
          std::stringstream ss;
          ss << "wifi-spectrum-per-example-" << i;
          phy.EnablePcap (ss.str (), apDevice);
        }
      g_signalDbmAvg = 0;
      g_noiseDbmAvg = 0;
      g_samples = 0;

      Simulator::Stop (Seconds (simulationTime + 1));
      Simulator::Run ();

      double throughput = 0;
      uint64_t totalPacketsThrough = 0;
      
      for(int l; l<nAp; ++l){
          if (udp)
            {
              //UDP
              totalPacketsThrough = DynamicCast<UdpServer> (serverApp.Get (0))->GetReceived ();
              throughput = totalPacketsThrough * payloadSize * 8 / (simulationTime * 1000000.0); //Mbit/s
            }
          else
            {
              //TCP
              uint32_t totalBytesRx = DynamicCast<PacketSink> (serverApp.Get (0))->GetTotalRx ();
              totalPacketsThrough = totalBytesRx / tcpPacketSize;
              throughput = totalBytesRx * 8 / (simulationTime * 1000000.0); //Mbit/s

              vazao[l][0] = throughput;
            }
          std::cout << std::setw (5) << i <<
            std::setw (6) << (i % 8) <<
            std::setprecision (2) << std::fixed <<
            std::setw (10) << datarate <<
            std::setw (12) << throughput <<
            std::setw (8) << totalPacketsThrough;
          if (totalPacketsThrough > 0)
            {
              std::cout << std::setw (12) << g_signalDbmAvg <<
                std::setw (12) << g_noiseDbmAvg <<
                std::setw (12) << (g_signalDbmAvg - g_noiseDbmAvg) <<
                std::endl;
            }
          else
            {
              std::cout << std::setw (12) << "N/A" <<
                std::setw (12) << "N/A" <<
                std::setw (12) << "N/A" <<
                std::endl;
            }
      }

      avalParam (nAp, vazao);

      Simulator::Destroy ();
    }
  return 0;
}

void avalParam(int nAp, float vazao[][1]){
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

          LostPackets [l][0]= rand()%(1000);
                            
          Throughput [l][0] = vazao[l][0];

          Energy [l][0]= rand()%(100);

          Delay [l][0]= rand()%(1000);

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
              std::cout << "LostPackets " << somaPerdaPct  << " " <<std::endl;
              std::cout << "Throughput " << somaVazao  << " " <<std::endl;
              std::cout << "Energy " << somaEnergia  << " " <<std::endl;
              std::cout << "Delay " << somaDelay  << " " <<std::endl;
              std::cout << "Range " << somaAlcance  << " " <<std::endl;
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
        
        std::cout << "Nó " << l << " Pontuação perda de pacotes " << ((1-normalmMR[l][1])*100)*0.3<<std::endl;
        std::cout << "Nó " << l << " Pontuação vazão " << (normalmMR[l][2]*100)*0.25<<std::endl;
        std::cout << "Nó " << l << " Pontuação Energia " << (normalmMR[l][3]*100)*0.2<<std::endl;
        std::cout << "Nó " << l << " Pontuação Delay " << ((1-normalmMR[l][4])*100)*0.15<<std::endl;
        std::cout << "Nó " << l << " Pontuação Alcance " << ((1-normalmMR[l][5])*100)*0.1<<std::endl;
        std::cout << " " <<std::endl;
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