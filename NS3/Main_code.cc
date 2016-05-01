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

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/gnuplot.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/stats-module.h"
#include "ns3/nstime.h"

using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("ns3_assignment");

int main (int argc, char *argv[])
{
	bool udp = true,verbose=false;
	double simulationTime = 10; // Simulation time in seconds
	double frequency = 5.0; //whether 2.4 or 5.0 GHz
	int nStanodes=10;   // number of stations
	double payloadSize=1472; // payloadsize
	double DataModevalue=3; // datarate mode (1-5)
	int MobilityMethod=1; //mobility method (1-4)
 
    // Starting file for values for current parameters
	std::ofstream data_file("data_values.csv", std::ios::out | std::ios::app);
	
	//Command line arguments for variables
	CommandLine cmd;
	cmd.AddValue ("frequency", "Whether working in the 2.4 or 5.0 GHz band (other values gets rejected)", frequency);
	cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
	cmd.AddValue ("udp", "UDP if set to 1, TCP otherwise", udp);
	cmd.AddValue ("nStanodes", "Number of stations", nStanodes);
	cmd.AddValue ("payloadSize", "payloadsize for UDP", payloadSize);
	cmd.AddValue ("DataModevalue", "DataMode value for MAC", DataModevalue);
	cmd.AddValue ("MobilityMethod", "Type of Mobility Method (1-4)", MobilityMethod);
	
	cmd.Parse (argc,argv);
	
	//Enabeling the log component
	LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
	
	//Declaring Number of nodes
	NodeContainer wifiStaNode;
	wifiStaNode.Create (nStanodes);
	NodeContainer wifiApNode;
	wifiApNode.Create (1);
	
	//Changing physical layer attributes
	YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
	//channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel"); // for propogation delay
	//channel.AddPropagationLoss ("ns3::FriisPropagationLossModel"); // for packet loss
	phy.SetChannel (channel.Create ());
	phy.Set ("ShortGuardEnabled", BooleanValue (true));
	
	//setting Wi-Fi standards
	WifiHelper wifi = WifiHelper::Default ();
	if (frequency ==5)
	wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
	else
	wifi.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
	if (verbose)
	{
		wifi.EnableLogComponents ();
	}
	
	//Initialising MAC layer
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
	
	//Defining different Modility Methods for Nodes
	MobilityHelper mobility;
	switch(MobilityMethod)
	{
		case 1:	 
		data_file << "GridPositionAllocator,";
		AnimationInterface anim1("GridPositionAllocator.xml");
		mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
		"MinX", DoubleValue (0.0),
		"MinY", DoubleValue (0.0),
		"DeltaX", DoubleValue (10.0),
		"DeltaY", DoubleValue (10.0),
		"GridWidth", UintegerValue (4),
		"LayoutType", StringValue ("ColumnFirst"));
		break;
		case 2:		
		data_file << "UniformDiscPositionAllocator,";
		AnimationInterface anim2("UniformDiscPositionAllocator.xml");
		mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
		"X", DoubleValue (5.0),
		"Y", DoubleValue (5.0),
		"rho", DoubleValue (5.0));
		break;
		case 3:
		data_file << "RandomRectanglePositionAllocator,";
		AnimationInterface anim3("RandomRectanglePositionAllocator.xml");
		mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
		"X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"),
		"Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max=1.0]"));
		break;
		case 4:		
		data_file << "RandomBoxPositionAllocator,";
		AnimationInterface anim4("RandomBoxPositionAllocator.xml");
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
	
    //Keeping Access Point fixed
	MobilityHelper mobility1;
	mobility1.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility1.Install (wifiApNode);
	
	//Installing Internet Stack on all nodes
	InternetStackHelper stack;
	stack.Install (wifiApNode);
	stack.Install (wifiStaNode);
	
	//Providing IP address to All Nodes
	Ipv4AddressHelper address;
	address.SetBase ("192.168.1.0", "255.255.255.0");
	
	Ipv4InterfaceContainer staNodeInterface;
	Ipv4InterfaceContainer apNodeInterface;
	
	apNodeInterface = address.Assign (apDevice);
	staNodeInterface = address.Assign (staDevice);
	
	//Installation of UDP protocol
	ApplicationContainer serverApp, sinkApp;
	UdpServerHelper myServer (9);
	serverApp = myServer.Install (wifiApNode);
	
	//Initialising Simulation time
	serverApp.Start (Seconds (1.0));
	serverApp.Stop (Seconds (simulationTime));
	clientApp.Start (Seconds (2.0));
	clientApp.Stop (Seconds (simulationTime));
	
	//Setting up UDP protocol attributes 
	UdpClientHelper myClient (apNodeInterface.GetAddress (0), 9);
	myClient.SetAttribute ("Interval", TimeValue (Seconds (1))); //packets/s
	myClient.SetAttribute ("PacketSize", UintegerValue (payloadSize));//payload
	myClient.SetAttribute ("MaxPackets", UintegerValue (nStanodes));
	
	ApplicationContainer clientApp = myClient.Install (wifiStaNode);
	
	//Displaying of IP Address of nodes
	std::cout<<"Address for Access Point node: "<<apNodeInterface.GetAddress(0)<<std::endl;
		std::cout<<"Address for Nodes\n";
		for(int i=0;i<nStanodes;++i) 
		{
		std::cout<<"Node "<<i<<" : "<<staNodeInterface.GetAddress(i)<<std::endl;
	}
	
	//Populating routing tables of nodes
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	
	//Writing packet data to pcap file (readable by WireShark)
	phy.EnablePcap("Simulation_nodes",wifiStaNode);
	phy.EnablePcap("Simulation_apnodes",wifiapNode);
	
	//Calculation of Throughput and Delay
	double throughput[100];
	double Ddelay[100];
	
	FlowMonitorHelper flowmonitor;
	Ptr<FlowMonitor> monitor = flowmonitor.InstallAll();
	
	//Specifing run time
	Simulator::Stop (Seconds (simulationTime));
	
	//Starting the simulation
	Simulator::Run ();
	
	monitor->CheckForLostPackets (); //Monitoring packet transfer
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonitor.GetClassifier ());
	std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
	
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i) 
	{
		throughput[i->first] = ((i->second.rxBytes) * 8.0) / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/nStanodes;
		Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
		
		if(throughput[i->first]==0) //Noting unreachable nodes
			Ddelay[i->first]=999998;
		else
		{
			Time Tdelay (i->second.delaySum / i->second.rxPackets);
			Ddelay[i->first]=Tdelay.ToDouble(Time::US);	
		}	
		
		std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
		std::cout << "Throughput for node "<<i->first<< " : " << throughput[i->first] << " kbps \n";
		std::cout << " Delay for node "<<i->first<< " : " << Ddelay[i->first] << "\n";
	}
	
	//Destroing simulation variables
	Simulator::Destroy ();
	
	// Calculating avarage throughput and delay for simulation scenario
	double avgthr=0,avgdel=0;
	for (int i=1;i<=nStanodes;++i)
	{
		avgthr = avgthr + throughput[i];
		avgdel = avgdel + Ddelay[i];
	}
	avgdel=avgdel/nStanodes; 
	avgthr=avgthr/nStanodes;
	
	//Printing Values to console
	std::cout << " Total average throughput "<< avgthr <<" kbit/s" << std::endl;
	std::cout << " Total average delay " << avgdel << "us" << std::endl;
	
	//Writing Datas to files
	data_file << "Number_of_nodes,Payload_size,DataRate_model,Frequency,Average_throughput,Average_delay\n";
	data_file << nStanodes << "," << payloadSize<< "," << DataModevalue << "," << frequency << "," << avgthr << "," << avgdel << "," << "\n";
	data_file.close();
	return 0;
}