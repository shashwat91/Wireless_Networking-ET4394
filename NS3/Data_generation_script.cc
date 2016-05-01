/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
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
	* Authors: Shashwat Verma <s.verma@student.tudelft.nl>
*/
#include <fstream>
#include "string.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/stats-module.h"
#include "ns3/nstime.h"

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("ns3_assignment");

int main (int argc, char *argv[])
{
	int paySize[3]={500,1000,1472}; // payloadsize
	int MobilityMethod=1; //mobility method	
	CommandLine cmd;
	//cmd.AddValue ("payloadSize", "payloadsize for UDP", payloadSize);
	//cmd.AddValue ("MobilityMethod", "Type of Mobility Method (1-4)", MobilityMethod);
	
	cmd.Parse (argc,argv);
    std::ofstream out_del("try2_node_del.csv", std::ios::out | std::ios::app);
	std::ofstream out_thr("try2_node_thr.csv", std::ios::out | std::ios::app);
	out_del << "Payload_size," << "Data_Model," << "Nodes," << "Average_Delay"<<"\n";
	out_thr << "Payload_size," << "Data_Model," << "Nodes," << "Average_Throughput"<<"\n";
	for(int pl=0;pl<3;++pl) 
	for(int dr=1; dr<=5; ++dr)
	{
		for(int nodes=2;nodes<=25;++nodes)
		{	
			//bool udp = true;
			bool verbose=false;
			double simulationTime = 10; // Simulation time in seconds
			double frequency = 5.0; //whether 2.4 or 5.0 GHz
			int nStanodes=nodes;   // number of stations
			int payloadSize=paySize[pl]; // payloadsize
			//char DataRate[20] = "DsssRate5_5Mbps";
			double DataModevalue=dr; // datarate mode
			MobilityMethod=1; //mobility method
			
			LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
			LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
			
			NodeContainer wifiStaNode;
			wifiStaNode.Create (nStanodes);
			NodeContainer wifiApNode;
			wifiApNode.Create (1);
			
			YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
			YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
			//channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
			//channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
			phy.SetChannel (channel.Create ());
			
			phy.Set ("ShortGuardEnabled", BooleanValue (true));
			WifiHelper wifi = WifiHelper::Default ();
			if (frequency ==5)
			wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
			else
			wifi.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
			if (verbose)
			{
				wifi.EnableLogComponents ();
			}
			
			HtWifiMacHelper mac = HtWifiMacHelper::Default ();
			StringValue DataMode = HtWifiMacHelper::DataRateForMcs (DataModevalue);
			wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode", DataMode,"ControlMode", DataMode);       
			
			Ssid ssid = Ssid ("ns3-my-system");
			
			mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (false));
			NetDeviceContainer staDevice;
			staDevice = wifi.Install (phy, mac, wifiStaNode);
			
			mac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));
			NetDeviceContainer apDevice;
			apDevice = wifi.Install (phy, mac, wifiApNode);
			
			Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (20));
			
			MobilityHelper mobility;
			switch(MobilityMethod)
			{
				case 1:	 
				mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
				"MinX", DoubleValue (0.0),
				"MinY", DoubleValue (0.0),
				"DeltaX", DoubleValue (10.0),
				"DeltaY", DoubleValue (10.0),
				"GridWidth", UintegerValue (4),
				"LayoutType", StringValue ("ColumnFirst"));
				break;
				case 2:		
				mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
				"X", DoubleValue (5.0),
				"Y", DoubleValue (5.0),
				"rho", DoubleValue (5.0));
				break;
				case 3:
				mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
				"X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
				"Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"));
				break;
				case 4:		
				mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator",
				"X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
				"Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
				"Z", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"));
				break;
				default:
				std::cout<<"(0,0) positions assigned to all nodes";
			}   		
			
			mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
			mobility.Install (wifiStaNode);
			

			MobilityHelper mobility1;
			Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
				positionAlloc->Add (Vector (10.0, 0.0, 30.0));
			mobility1.SetPositionAllocator (positionAlloc);
			mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
			mobility1.Install (wifiApNode);
			
			/* Internet stack*/
			InternetStackHelper stack;
			stack.Install (wifiApNode);
			stack.Install (wifiStaNode);
			
			Ipv4AddressHelper address;
			
			address.SetBase ("192.168.1.0", "255.255.255.0");
			Ipv4InterfaceContainer staNodeInterface;
			Ipv4InterfaceContainer apNodeInterface;
			
			apNodeInterface = address.Assign (apDevice);
			staNodeInterface = address.Assign (staDevice);
			
			ApplicationContainer serverApp, sinkApp;
			UdpServerHelper myServer (9);
			serverApp = myServer.Install (wifiApNode);
			//serverApp = myServer.Install (wifiStaNode.Get (0));
			serverApp.Start (Seconds (1.0));
			serverApp.Stop (Seconds (simulationTime));
			
			//uint16_t sinkPort = 8080;
			//Address sinkAddress (InetSocketAddress (apNodeInterface.GetAddress (0),sinkPort));
			
			UdpClientHelper myClient (apNodeInterface.GetAddress (0), 9);
			myClient.SetAttribute ("Interval", TimeValue (Seconds (1))); //packets/s
			myClient.SetAttribute ("PacketSize", UintegerValue (payloadSize));//payload
			myClient.SetAttribute ("MaxPackets", UintegerValue (nStanodes));
			//myClient.SetAttribute ("DataRate",StringValue (datarate));
			
			/* OnOffHelper onOffHelper ("ns3::TcpSocketFactory", sinkAddress);
			onOffHelper.SetAttribute ("DataRate",StringValue (DataRate)); */
			
			ApplicationContainer clientApp = myClient.Install (wifiStaNode);
			
			std::cout<<"Address for Access Point node: "<<apNodeInterface.GetAddress(0)<<std::endl;
				std::cout<<"Address for Nodes\n";
				for(int i=0;i<nStanodes;++i) 
				{
				std::cout<<"Node "<<i<<" : "<<staNodeInterface.GetAddress(i)<<std::endl;
			}
			
			clientApp.Start (Seconds (2.0));
			clientApp.Stop (Seconds (simulationTime));
			Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
			
			phy.EnablePcap("Simulation",wifiStaNode);
			
			AnimationInterface anim("Simulation.xml");
			
			//calculate throughput
			double throughput[100];
			FlowMonitorHelper flowmonitor;
			double Ddelay[100];
			Ptr<FlowMonitor> monitor = flowmonitor.InstallAll();
			
			Simulator::Stop (Seconds (simulationTime + 1));
			
			Simulator::Run ();
			
			monitor->CheckForLostPackets ();
			Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonitor.GetClassifier ());
			std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
			for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i) 
			{
				throughput[i->first] = ((i->second.rxBytes) * 8.0) / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/nStanodes;
				Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
				if(throughput[i->first] == 0)
				Ddelay[i->first]=999998;
				else
				{
					Time Tdelay (i->second.delaySum / i->second.rxPackets);
					Ddelay[i->first]=Tdelay.ToDouble(Time::US);		
				}
				// std::cout <<  << std::endl;
				std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
					/*std::cout << " Tx Bytes: " << i->second.txBytes << "\n";
				std::cout << " Rx Bytes: " << i->second.rxBytes << "\n";*/
					std::cout << " Average Throughput: " << throughput[i->first] << " kbps \n";
				std::cout << " Delay : "<< Ddelay[i->first] << "\n";
			}
			
			Simulator::Destroy ();
			
			double avgthr=0,avgdel=0;
			for (int i=1;i<=nStanodes;++i)
			{
				avgthr = avgthr + throughput[i];
				avgdel = avgdel + Ddelay[i];
			}
			avgdel=avgdel/nStanodes; 
			avgthr=avgthr/nStanodes;
			
			out_del << payloadSize << "," << dr << "," << nStanodes << "," << avgdel<<"\n";
			out_thr << payloadSize << "," << dr << "," << nStanodes << "," << avgthr<<"\n";
		}
	}
	out_del.close();
	out_thr.close();
	return 0;
}
