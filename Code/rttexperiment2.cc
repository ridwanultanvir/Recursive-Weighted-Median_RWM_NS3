// this code is "xyz_" folder 
// so "xyz_" based code =====================================================


/*
https://www.nsnam.org/doxygen/tcp-variants-comparison_8cc_source.html

ekahn thekei main help

*/ 

#include <iostream>
#include <fstream>
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/error-model.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Experiment2");

/*
12 sec er shomoy (total time = 30 sec)

ami hotath kore 
Data rate komiye disi; 

So Amar RTT = round-trip time bere jabe; 

But Jacob e eta Stabilized hote onek time ney

*/ 




static bool isitWeightedMedian = true;

static bool firstRtt = true;
static bool firstRto = true;
static Ptr<OutputStreamWrapper> rttStream;
static Ptr<OutputStreamWrapper> rtoStream;



static uint32_t mydefinedrttweightedmedianvalue = 5; 



static void
RttTracer (Time oldval, Time newval)
{
  if (firstRtt)
    {
      *rttStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
  *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}

static void
RtoTracer (Time oldval, Time newval)
{
  if (firstRto)
    {
      *rtoStream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRto = false;
    }
  *rtoStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}



static void
TraceRtt (std::string rtt_tr_file_name)
{
  AsciiTraceHelper ascii;
  rttStream = ascii.CreateFileStream (rtt_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/RTT", MakeCallback (&RttTracer));
}

static void
TraceRto (std::string rto_tr_file_name)
{
  AsciiTraceHelper ascii;
  rtoStream = ascii.CreateFileStream (rto_tr_file_name.c_str ());
  Config::ConnectWithoutContext ("/NodeList/0/$ns3::TcpL4Protocol/SocketList/0/RTO", MakeCallback (&RtoTracer));
}


static void
rttvaluechange (PointToPointHelper sourceLink, PointToPointHelper sinkLink, PointToPointHelper bottleneckLink)
{
  sourceLink.SetDeviceAttribute ("DataRate", StringValue("10Mbps"));
  sinkLink.SetDeviceAttribute ("DataRate", StringValue("10Mbps"));
  bottleneckLink.SetDeviceAttribute ("DataRate", StringValue("1Mbps"));
  NS_LOG_UNCOND("rttvaluechange\n");
}



//edge_b = bandwidth; edge_d = delay 
uint32_t getq_size(DataRate edge_b, DataRate bottle_b, Time edge_d, Time bottle_d )
{

  uint32_t my_q_size = static_cast<uint32_t>((std::min (edge_b, bottle_b).GetBitRate () / 8) *
    ((edge_d + bottle_d) * 2).GetSeconds ());
  return my_q_size; 

}

int main (int argc, char *argv[])
{
  std::string transport_prot = "TcpWestwood";
  double error_p = 0.0;
  std::string bottleneck_bandwidth = "50Mbps";
  std::string bottleneck_delay = "35ms";
  std::string edge_bandwidth = "2Mbps";
  std::string edge_delay = "175ms";
  
  bool tracing = true;
  std::string prefix_file_name = "d2-jacob";
  uint64_t data_mbytes = 0;
  uint32_t mtu_bytes = 1000;//400
  uint16_t num_flows = 3;
  double myduration = 30;
  uint32_t run = 0;
  bool flow_monitor = true;
  bool pcap = false;
  bool sack = true;
  std::string queue_disc_type = "ns3::PfifoFastQueueDisc";
  std::string recovery = "ns3::TcpClassicRecovery";

  double my_start_time = 0.1;



  double rttvaluechangetime = 12.0; 

//   int myRcvBufSize = 16384; 
//   int mySndBufSize = 16384; 
  int myRcvBufSize = 1<<21; 
  int mySndBufSize = 1<<21; 


  uint16_t nPort = 64000;


  CommandLine cmd (__FILE__);

  /*
  https://www.nsnam.org/doxygen/tcp-variants-comparison_8cc_source.html
  this strategy taken from 

  tcp-variants-comparison.cc file 


  Ei file ei uint32_t tcp_adu_size egula define kora ase
  tchCoDel
  eta bad diye disi
  */ 

  cmd.AddValue ("error_p", "Packet error rate", error_p);
  cmd.AddValue ("bandwidth", "Bottleneck bandwidth", bottleneck_bandwidth);
  cmd.AddValue ("delay", "Bottleneck delay", bottleneck_delay);
  cmd.AddValue ("access_bandwidth", "Access link bandwidth", edge_bandwidth);
  cmd.AddValue ("access_delay", "Access link delay", edge_delay);
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue ("prefix_name", "Prefix of output trace file", prefix_file_name);
  cmd.AddValue ("data", "Number of Megabytes of data to transmit", data_mbytes);
  cmd.AddValue ("mtu", "Size of IP packets to send in bytes", mtu_bytes);
  cmd.AddValue ("num_flows", "Number of flows", num_flows);
  
  cmd.AddValue ("run", "Run index (for setting repeatable seeds)", run);
  cmd.AddValue ("flow_monitor", "Enable flow monitor", flow_monitor);
  cmd.AddValue ("pcap_tracing", "Enable or disable PCAP tracing", pcap);
  // cmd.AddValue ("queue_disc_type", "Queue disc type for gateway (e.g. ns3::CoDelQueueDisc)", queue_disc_type);
  cmd.AddValue ("sack", "Enable or disable SACK option", sack);
  cmd.AddValue ("recovery", "Recovery algorithm type to use (e.g., ns3::TcpPrrRecovery", recovery);

  cmd.AddValue ("isitWeightedMedian", "Check if isitWeightedMedian", isitWeightedMedian);
  cmd.AddValue ("nPort", "Num of nPort", nPort);
  cmd.AddValue ("rttvaluechangetime", "rttvaluechangetime", rttvaluechangetime); 
  cmd.AddValue ("myRcvBufSize", "myRcvBufSize", myRcvBufSize);
  cmd.AddValue ("mySndBufSize", "mySndBufSize", mySndBufSize);
  cmd.AddValue ("my_start_time", "my_start_time", my_start_time);

  cmd.AddValue ("myduration", "Time to allow flows to run in seconds", myduration);
  cmd.Parse (argc, argv);


  ///--------------------------                 AMAR       RttMyWeightedMedian ADD KORSI -----------------------------------------
  if(isitWeightedMedian){
      Config::SetDefault ("ns3::TcpL4Protocol::RttEstimatorType", TypeIdValue(RttMyWeightedMedian::GetTypeId()));
      Config::SetDefault ("ns3::TcpSocketBase::CheckIfRttWeightedMedian",UintegerValue (mydefinedrttweightedmedianvalue));
      prefix_file_name = "d2-weighted"; 
  }
  

  transport_prot = std::string ("ns3::") + transport_prot;

  SeedManager::SetSeed (1);
  SeedManager::SetRun (run);

  //Application Data Unit (ADU) 
  // Calculate the ADU size
  Header* temp_header = new Ipv4Header ();
  uint32_t ip_header = temp_header->GetSerializedSize ();
  NS_LOG_LOGIC ("IP Header size is: " << ip_header);
  delete temp_header;
  temp_header = new TcpHeader ();
  uint32_t tcp_header = temp_header->GetSerializedSize ();
  NS_LOG_LOGIC ("TCP Header size is: " << tcp_header);
  delete temp_header;
  uint32_t tcp_adu_size = mtu_bytes - 20 - (ip_header + tcp_header);
  NS_LOG_LOGIC ("TCP ADU size is: " << tcp_adu_size);


  

  // Set the simulation start and stop time
  
  double my_stop_time = my_start_time + myduration;

  // 2 MB of TCP buffer
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (myRcvBufSize));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (mySndBufSize));
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));

  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",
                      TypeIdValue (TypeId::LookupByName (recovery)));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
  

  // Create routers, sources, and sinks
  int numRouter = 2; 
  NodeContainer sources;
  sources.Create (num_flows);
  NodeContainer sinks;
  sinks.Create (num_flows);
  NodeContainer routers;
  routers.Create (numRouter);

  /*
  dhoro ei A ==== B ; 
  egulai bottleneck hobe pore
  */ 

  // Configure the error model
  // Here we use RateErrorModel with packet error rate
  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable> ();
  uv->SetStream (50);
  RateErrorModel error_model;
  error_model.SetRandomVariable (uv);
  error_model.SetUnit (RateErrorModel::ERROR_UNIT_PACKET);
  error_model.SetRate (error_p);

  PointToPointHelper bottleneckLink;
  bottleneckLink.SetDeviceAttribute ("DataRate", StringValue (bottleneck_bandwidth));
  bottleneckLink.SetChannelAttribute ("Delay", StringValue (bottleneck_delay));
  bottleneckLink.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));

  InternetStackHelper stack;
  stack.InstallAll ();

  TrafficControlHelper tchPfifo;
  tchPfifo.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");



  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");

  // Configure the sources and sinks net devices
  // and the channels between the sources/sinks and the gateways
  PointToPointHelper sourceLink;
  sourceLink.SetDeviceAttribute ("DataRate", StringValue (edge_bandwidth));
  sourceLink.SetChannelAttribute ("Delay", StringValue (edge_delay));

  PointToPointHelper sinkLink;
  sinkLink.SetDeviceAttribute ("DataRate", StringValue (edge_bandwidth));
  sinkLink.SetChannelAttribute ("Delay", StringValue (edge_delay));
  //sinkLink.SetDeviceAttribute ("ReceiveErrorModel", PointerValue (&error_model));

  Ipv4InterfaceContainer sink_currinterfaces;

  DataRate edge_b (edge_bandwidth);
  DataRate bottle_b (bottleneck_bandwidth);
  Time edge_d (edge_delay);
  Time bottle_d (bottleneck_delay);

  uint32_t q_size = getq_size(edge_b, bottle_b, edge_d, bottle_d); 

  Config::SetDefault ("ns3::PfifoFastQueueDisc::MaxSize",
                      QueueSizeValue (QueueSize (QueueSizeUnit::PACKETS, q_size / mtu_bytes)));


  NetDeviceContainer routerDevices = bottleneckLink.Install (routers.Get (0), routers.Get (1));
  address.Assign(routerDevices);
  NetDeviceContainer sourceDevices[num_flows];
  NetDeviceContainer sinkDevices[num_flows];

  for (uint32_t i = 0; i < num_flows; i++)
    {
      sourceDevices[i] = sourceLink.Install (sources.Get (i), routers.Get (0));
      tchPfifo.Install (sourceDevices[i]);
      address.NewNetwork ();
      Ipv4InterfaceContainer currinterfaces = address.Assign (sourceDevices[i]);

      sinkDevices[i] = sinkLink.Install (routers.Get (1), sinks.Get (i));
      tchPfifo.Install (sinkDevices[i]);
      
      address.NewNetwork ();
      currinterfaces = address.Assign (sinkDevices[i]);
      sink_currinterfaces.Add (currinterfaces.Get (1));
    }

  NS_LOG_INFO ("Initialize Global Routing.\n");
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  
  Address sinkLocalAddress (InetSocketAddress (Ipv4Address::GetAny (), nPort));
  PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", sinkLocalAddress);
  ApplicationContainer mysinkApp;

  for (uint16_t i = 0; i < sources.GetN (); i++)
    {
      AddressValue remoteAddress (InetSocketAddress (sink_currinterfaces.GetAddress (i, 0), nPort));
      Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (tcp_adu_size));
      BulkSendHelper ftp ("ns3::TcpSocketFactory", Address ());
      ftp.SetAttribute ("Remote", remoteAddress);
      ftp.SetAttribute ("SendSize", UintegerValue (tcp_adu_size));
      ftp.SetAttribute ("MaxBytes", UintegerValue (data_mbytes * 1000000));

      ApplicationContainer mysourceApp = ftp.Install (sources.Get (i));
      mysourceApp.Start (Seconds (my_start_time * i));
      mysourceApp.Stop (Seconds (my_stop_time - 3));

      sinkHelper.SetAttribute ("Protocol", TypeIdValue (TcpSocketFactory::GetTypeId ()));
      mysinkApp = sinkHelper.Install (sinks.Get (i));
      mysinkApp.Start (Seconds (my_start_time * i));
      mysinkApp.Stop (Seconds (my_stop_time));
    }
  
  // Set up tracing if enabled
  if (tracing)
  {
    AsciiTraceHelper ascii;
    Simulator::Schedule (Seconds (0.00001), &TraceRtt, prefix_file_name + "-rtt.data");
    Simulator::Schedule (Seconds (0.00001), &TraceRto, prefix_file_name + "-rto.data");
  }
    Simulator::Schedule(Seconds(12.0), &rttvaluechange, sourceLink, sinkLink, bottleneckLink);

  if (pcap)
  {
    bottleneckLink.EnablePcapAll (prefix_file_name, true);
    sourceLink.EnablePcapAll (prefix_file_name, true);
  }

  // Flow monitor
  FlowMonitorHelper flowHelper;
  Ptr<FlowMonitor> myflowMonitor;
  if (flow_monitor)
  {
    myflowMonitor = flowHelper.InstallAll ();
  }

  Simulator::Stop (Seconds (my_stop_time));
  Simulator::Run ();

  //metric calculation
  if (flow_monitor) {
      myflowMonitor->SerializeToXmlFile ("rttexp2.flowmon", false, false);

  }

  Simulator::Destroy ();


  return 0;
}


/*

Links: 
https://www.nsnam.org/doxygen/tcp-variants-comparison_8cc_source.html

https://groups.google.com/g/ns-3-users/c/n9CnA8GYmBo


*/ 