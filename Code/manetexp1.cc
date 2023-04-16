#include <fstream>
#include <iostream>
#include <random>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/aodv-module.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/flow-monitor-helper.h"

using namespace ns3;

static std::string prefix_file_name = "m5-jacob";

static bool isitWeightedMedian = false;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Begin trace setup code
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  *stream->GetStream () << newCwnd << std::endl;

}
// static bool firstRtt = true;
// static bool firstRealRtt = true;
static void RttTracer(Ptr<OutputStreamWrapper> stream, Time oldval, Time newval) {
    // std::cout<<" Ami RTT tracer e ------------------------\n";
    /*
    if (firstRtt)
    {
      *stream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRtt = false;
    }
    */
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;

}
static void
RtoTracer (Ptr<OutputStreamWrapper> stream,Time oldval, Time newval)
{
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}
/*
static void
RealRttTracer (Ptr<OutputStreamWrapper> stream,Time oldval, Time newval)
{
  if (firstRealRtt)
    {
      *stream->GetStream () << "0.0 " << oldval.GetSeconds () << std::endl;
      firstRealRtt = false;
    }
  *stream->GetStream ()  << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
}
*/ 
void SetCallback(Ptr<BulkSendApplication> app)
{
  // Get socket from app
  Ptr<Socket> sock = app->GetSocket();

  // Setup stream
  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream ("outputFile/manet5.cwnd", std::ios::app);
  std::string rttfilename1 = "outputFile/" + prefix_file_name + "-rtt.data"; 
  std::string rtofilename1 = "outputFile/" + prefix_file_name + "-rto.data"; 


  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream (rttfilename1, std::ios::app);
  Ptr<OutputStreamWrapper> stream3 = asciiTraceHelper.CreateFileStream (rtofilename1, std::ios::app);


  sock->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
  sock->TraceConnectWithoutContext ("RTT", MakeBoundCallback (&RttTracer, stream2));
  sock->TraceConnectWithoutContext ("RTO", MakeBoundCallback (&RtoTracer, stream3));

  
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// End trace setup code
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class RttCheck1
{
public:
  RttCheck1();
  RttCheck1 (int port1, int numflow1, int numNode1); 
  void setNode(int nNode1){
    nNode = nNode1; 
  }
  void setFlow(int nFlow1)
  {
    nFlow = nFlow1; 
  }
  void setPort(int nPort1)
  {
    nPort = nPort1; 
  }
  void Run ();
  int getMyFlowNumber(){
    return nFlow; 
  }
  int getMyTotalNode(){
    return nNode; 
  }


private:
  uint32_t nPort;
  //BulkSendHelper sendHelper ("ns3::TcpSocketFactory", (InetSocketAddress (adhocInterfaces.GetAddress (receiverIndex), port))); // To address
  //ekhane Port kaje lage
  uint32_t nFlow;
  uint32_t nNode;

};

NS_LOG_COMPONENT_DEFINE ("RttCheck1");

RttCheck1::RttCheck1 ()
{
  nPort = 1024 ; 
  nFlow = 3; // To change number of flows (3	7	17	34	68	100	130)
  nNode = 20; 
}

RttCheck1::RttCheck1 (int port1, int numflow1, int numNode1)
{
    nPort = port1 ; 
    nFlow =numflow1; // To change number of flows (3	7	17	34	68	100	130)
    nNode =numNode1;  // To change number of nodes
}



void RttCheck1::Run()
{
    //NS_LOG_DEBUG("Ami PR1 ---------------------------------------------------------------------------");
    std::cout<<"Run(): nPort:"<<nPort<<" nNode:"<<nNode<<" nFlow:"<<nFlow<<"\n"; 

    double minuteTime1 = 60.0; 

    // Seed RNG for multiple future uses
    srand(time(NULL));

    Packet::EnablePrinting ();


    double mysimTime = 5.0*minuteTime1; 
    // In seconds
    int mynodeSpeed = 30; // Max speed in meters/second
    int mynodePause = 0;
    std::string my_basicasic_rate ("DsssRate1Mbps");

    std::string myAreaRangeStr ("ns3::UniformRandomVariable[Min=0.0|Max=1500.0]");
    /*
    MYNOTE: 
    https://www.nsnam.org/wiki/HOWTO_add_basic_rates_to_802.11
    https://en.wikipedia.org/wiki/IEEE_802.11_RTS/CTS

    This means that control response frames may be sent using different rate even though the ConstantRateWifiManager is used.
    The ControlMode attribute of the ConstantRateWifiManager is used for RTS frames only. 
    The rate of CTS and ACK frames are selected according to the 802.11 standard.
    
    RTS/CTS (Request To Send / Clear To Send) 


    */ 


    // Set default attributes to match paper

    // Comment below line for simulation to use MeanDevation method ==========================
    // Config::SetDefault ("ns3::TcpL4Protocol::RttEstimatorType", TypeIdValue(RttFixedShare::GetTypeId()));
    ///--------------------------                 AMAR       RttMyWeightedMedian ADD KORSI -----------------------------------------
    if(isitWeightedMedian){
        Config::SetDefault ("ns3::TcpL4Protocol::RttEstimatorType", TypeIdValue(RttMyWeightedMedian::GetTypeId()));
        prefix_file_name = "m5-weighted"; 
    }


    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (my_basicasic_rate));
    Config::SetDefault ("ns3::BulkSendApplication::Protocol",   TypeIdValue (TcpSocketFactory::GetTypeId ()));
    /*
    BulkSendApplication => sSend as much traffic as possible, trying to fill the bandwidth.
    Config Paths: "/NodeList/[i]/ApplicationList/[i]/$ns3::BulkSendApplication"
    Protocol: The type of protocol to use.
    Set with class: TypeIdValue
    Underlying type: TypeId
    Initial value: ns3::TcpSocketFactory
    TcpSocketFactory => API to create TCP socket instances.
    */ 
    
    
    Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (16384));
    Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (16384));
    Config::SetDefault ("ns3::RangePropagationLossModel::MaxRange", DoubleValue (100.0));
    Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue(1));


    /*

    https://www.nsnam.org/doxygen/wifi-simple-adhoc-grid_8cc_source.html


    */ 


    NodeContainer adhocNodes;
    adhocNodes.Create (getMyTotalNode());

    /*
    WifiHelper::SetStandard()
    WifiHelper::SetStandard () is a method required to set various parameters 
    NOTE: 
    Wifi MAC layer : 
    1. Coordinates access to the shared physical air interface .

    */ 

    WifiHelper mywifi;

    mywifi.SetStandard (WIFI_STANDARD_80211b);
    // 802.11a uses a 5 GHz frequency, while 802.11b operates on a 2.4 GHz band.

    /*
    Make it easy to create and manage PHY objects for the YANS model.
    The YANS PHY model is described in "Yet Another Network Simulator",
    */
    YansWifiPhyHelper wifiPhy;
    YansWifiChannelHelper wifiChannel;
    /*
    YansWifiChannelHelper: manage and create wifi channel objects for the YANS model.
    The intent of this class is to make it easy to create a channel object which implements the YANS channel model. 
    */


    
    /*
    https://www.nsnam.org/doxygen/wifi-simple-adhoc-grid_8cc_source.html
    */ 
     
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    //classns3_1_1_constant_speed_propagation_delay_model:  the propagation speed is constant
    wifiChannel.AddPropagationLoss ("ns3::RangePropagationLossModel");
    // https://www.nsnam.org/doxygen/classns3_1_1_range_propagation_loss_model.html
    // The propagation loss depends only on the distance (range) between transmitter and receiver. 

    wifiPhy.SetChannel (wifiChannel.Create ());

    WifiMacHelper myWifiMac;
    /*
    ns3::ConstantRateWifiManager 
    use constant rates for data and RTS transmissions
    This class uses always the same transmission rate for every packet sent.

    =====

    */ 
    mywifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager","DataMode",StringValue (my_basicasic_rate),"ControlMode",StringValue (my_basicasic_rate));


    myWifiMac.SetType ("ns3::AdhocWifiMac");
    /*
    https://www.nsnam.org/doxygen/classns3_1_1_adhoc_wifi_mac.html#details

    Wifi MAC high model for an ad-hoc Wifi MAC.
    ns3::AdhocWifiMac is accessible through the following paths with Config::Set and Config::Connect:

    */ 

    NetDeviceContainer adhocDevices = mywifi.Install (wifiPhy, myWifiMac, adhocNodes);

    MobilityHelper mobilityAdhoc;
    int64_t streamIndex = 0; // used to get consistent mobility across scenarios

    ObjectFactory pos;
    pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
    pos.Set ("X", StringValue (myAreaRangeStr));
    pos.Set ("Y", StringValue (myAreaRangeStr));

    Ptr<PositionAllocator> myPosAlloc = pos.Create ()->GetObject<PositionAllocator> ();
    streamIndex += myPosAlloc->AssignStreams (streamIndex);

    std::stringstream ssSpeed;

    ssSpeed << "ns3::UniformRandomVariable[Min=1.0|Max=" << mynodeSpeed << "]";
    std::stringstream ssPause;
    ssPause << "ns3::ConstantRandomVariable[Constant=" << mynodePause << "]";
    mobilityAdhoc.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                                    "Speed", StringValue (ssSpeed.str ()),
                                    "Pause", StringValue (ssPause.str ()),
                                    "PositionAllocator", PointerValue (myPosAlloc));
    mobilityAdhoc.SetPositionAllocator (myPosAlloc);
    mobilityAdhoc.Install (adhocNodes);
    streamIndex += mobilityAdhoc.AssignStreams (adhocNodes, streamIndex);

    AodvHelper aodv;
    Ipv4ListRoutingHelper list;
    InternetStackHelper internet;

    internet.SetTcp("ns3::TcpL4Protocol");
    list.Add (aodv, 100);
    internet.SetRoutingHelper (list);
    internet.Install (adhocNodes);

    Ipv4AddressHelper addressAdhoc;
    addressAdhoc.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer adhocInterfaces;
    adhocInterfaces = addressAdhoc.Assign (adhocDevices);  

    std::vector<Ptr<BulkSendApplication> > apps;
    std::vector<int> startTimes;


    int randomeNodeMax = 99999; 
    randomeNodeMax -= 1000; 



    //NS_LOG_DEBUG("Ami PR2 ---------------------------------------------------------------------------");
    for (int i = 0; i < getMyFlowNumber(); i++)
    {
      int mySenderidx = rand() % getMyTotalNode();
      int receiverIndex = rand() % getMyTotalNode();



      while (mySenderidx == receiverIndex)
      {

          receiverIndex = rand() % getMyTotalNode();

      }


      int numPackets = 1000 + (rand() % randomeNodeMax);


      int startTime = rand() % static_cast<int>(mysimTime); 
      // Time when to start sending data
      startTimes.push_back(startTime);

      //Sender 
      Ptr<Node> senderNode = adhocNodes.Get(mySenderidx);

      //Receiver 
      Ptr<Node> nextNode = adhocNodes.Get(receiverIndex);

      BulkSendHelper sendHelper ("ns3::TcpSocketFactory", (InetSocketAddress (adhocInterfaces.GetAddress (receiverIndex), nPort))); 
      // To address
      ApplicationContainer mysenderApp = sendHelper.Install(senderNode); // Install onto source

      // Ensure max data sent and set up cwnd trace
      Ptr<BulkSendApplication> bsApp = DynamicCast<BulkSendApplication> (mysenderApp.Get(0));
      apps.push_back(bsApp);

      bsApp->SetMaxBytes(512 * numPackets);

      // Set up receiver 
      PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress(adhocInterfaces.GetAddress (receiverIndex), nPort++)); // To address
      ApplicationContainer mysinkApp = sinkHelper.Install(nextNode); // Install onto sink

      mysinkApp.Start (Seconds (startTime));
      mysenderApp.Start (Seconds (startTime));
      mysinkApp.Stop (Seconds (mysimTime));
      mysenderApp.Stop (Seconds (mysimTime));

      //NS_LOG_DEBUG("Ami PR3 ---------------------------------------------------------------------------");
    }

    for (int i = 0; i < (int) apps.size(); i++)
    {
      //NS_LOG_DEBUG("Ami PR4 ---------------------------------------------------------------------------");
      Simulator::Schedule(Seconds ((double)startTimes[i] + 0.00001), &SetCallback, apps[i]);
    }

    // Set up flow monitoring
    Ptr<FlowMonitor> flowmon;
    FlowMonitorHelper flowmonHelper;
    flowmon = flowmonHelper.InstallAll ();

    //NS_LOG_DEBUG("Ami PR5 ---------------------------------------------------------------------------");

    Simulator::Stop (Seconds (mysimTime));
    //NS_LOG_DEBUG("Ami PR61 ---------------------------------------------------------------------------");
    Simulator::Run ();
    //NS_LOG_DEBUG("Ami PR62 ---------------------------------------------------------------------------");

    flowmon->SerializeToXmlFile ("outputFile/manet5.flowmon", false, false);
    //NS_LOG_DEBUG("Ami PR7 ---------------------------------------------------------------------------");
}


int 
main (int argc, char *argv[])
{
    LogComponentEnable ("RttCheck1", LOG_LEVEL_INFO);

    uint32_t nFlow1 = 1;
    uint32_t nNode1 = 20;
    uint32_t nPort1 = 1024; 


    CommandLine cmd;

    cmd.AddValue ("nFlow", "Num of Flow", nFlow1);
    cmd.AddValue ("nNode", "Num of Node", nNode1);
    cmd.AddValue ("nPort", "Num of nPort", nPort1);
    cmd.AddValue ("isitWeightedMedian", "Check if isitWeightedMedian", isitWeightedMedian);



    cmd.Parse (argc, argv);

    RttCheck1 rttcheck1; 

    if(argc==1){

      
    }
    else if(argc==2)
    {
      rttcheck1.setFlow(nFlow1); 
    }
    else if(argc==3)
    {
      rttcheck1.setFlow(nFlow1); 
      rttcheck1.setNode(nNode1); 
    }
    else if(argc==4)
    {
      rttcheck1.setFlow(nFlow1); 
      rttcheck1.setNode(nNode1); 
      rttcheck1.setPort(nPort1); 
    }

    // eta print er jonno 
    std::string filename("flowvserror1.txt");
    std::ofstream file_out;

    file_out.open(filename, std::ios_base::app);
    file_out << nFlow1<< " " ;

    // recheck print: 
    std::string filename3("flowvserror3.txt");
    std::ofstream file_out3;

    file_out3.open(filename3, std::ios_base::app);
    file_out3 << nFlow1<< " " ;

    
    // RttCheck1 rttcheck1(nFlow, nNode, nPort); eta agei declare korsi ami 


    rttcheck1.Run();
    //NS_LOG_DEBUG("Ami PR8 ---------------------------------------------------------------------------");
}