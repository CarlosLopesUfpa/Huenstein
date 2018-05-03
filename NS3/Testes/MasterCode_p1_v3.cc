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

#include <ns3/boolean.h>
#include <string>

#include <stdio.h>
#include <iostream> 
#include <ostream>
#include <time.h>



using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MasterCode");


//Proposta Conexão por P2P

         // void avalParam(int nAp);

int main (int argc, char *argv[])
{
// Step 1: Reconhecimento da rede.

//WIFI
//Configure WIFI
		
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

		  NetDeviceContainer staDeviceA, staDeviceAa, apDevice;
		  Ssid ssid;

		  //Network A
		  ssid = Ssid ("network-A");
		  phy.Set ("ChannelNumber", UintegerValue (36));
		  mac.SetType ("ns3::StaWifiMac",
		               "Ssid", SsidValue (ssid));
		  apDevice = wifi.Install (phy, mac, wifiApNodes.Get (0));

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
		  Ipv4InterfaceContainer apInterface;
		  apInterface = addresswifi.Assign (apDevice);

		  Ipv4InterfaceContainer staInterface;
		  staInterface = addresswifi.Assign (staDeviceA);

//APLICATIONS
			 
		  uint16_t port = 9;
		  Time interPacketInterval = Seconds (0.1);
      uint32_t payloadSize = 4096;//bytes
      uint64_t simulationTime = 10; //seconds
      //string server;
      //string client;

		  ApplicationContainer serverApps;
		  ApplicationContainer clientApps;
		  for (uint32_t ap = 0; ap <  wifiApNodes.GetN (); ++ap){
            //server = "server_" + ap;

            for(uint32_t sta = 0; sta < wifiStaNodes.GetN (); ++sta){
            //client = "client_" + sta;

				    UdpEchoServerHelper server (port);
            ApplicationContainer serverApps = server.Install (wifiApNodes.Get (ap));
            
            UdpEchoClientHelper client (apInterface.GetAddress (ap), port);
            client.SetAttribute ("MaxPackets", UintegerValue (100000));
            client.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
            client.SetAttribute ("PacketSize", UintegerValue (payloadSize));
            
            client.Install (wifiStaNodes.Get (0));
            ApplicationContainer clientApps = client.Install (wifiStaNodes.Get (sta));

            }
        }
				// Install and start applications on UEs and remote host
				  serverApps.Start (Seconds (0.01));
				  clientApps.Start (Seconds (0.01));
				 // lteHelper->EnableTraces ();

				Simulator::Stop (Seconds (simulationTime));




		  // UdpEchoServerHelper serverA (port);
		  // serverA.Install (wifiApNodes.Get (0));
		  // serverAppA.Start (Seconds (0.0));
		  // serverAppA.Stop (Seconds (simulationTime - 1));
  
  		//   Time interPacketInterval = Seconds (0.1);

		  // UdpEchoClientHelper clientA (apInterfaceA.GetAddress (0), port);
		  // clientA.SetAttribute ("MaxPackets", UintegerValue (1));
		  // clientA.SetAttribute ("Interval", TimeValue (interPacketInterval)); //packets/s
		  // clientA.SetAttribute ("PacketSize", UintegerValue (payloadSize));
		  
		  // clientA.Install (wifiStaNodes.Get (0));
		  // clientAppA.Start (Seconds (1.0));
		  // clientAppA.Stop (Seconds (simulationTime - 1));

//LÓGICA DE SELEÇÃO
     // avalParam(nAp);


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


		  Simulator::Destroy ();
		  return 0;
}