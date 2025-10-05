/* 
 * QoS-Aware Data Streaming in Mobile Networks (NS-3 Simulation)
 * 
 * Usage example:
 *   ./waf --run "scratch/qos_lte_streaming --ueSpeed=3.0"
 *
 * Generates qos-results.xml with FlowMonitor statistics.
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("QoSStreaming");

int main(int argc, char *argv[])
{
    double ueSpeed = 3.0;            // default UE speed (m/s)
    double simSeconds = 20.0;        // simulation duration
    uint16_t port = 8000;

    // Parse command-line arguments
    CommandLine cmd;
    cmd.AddValue("ueSpeed", "UE movement speed in m/s", ueSpeed);
    cmd.AddValue("simTime", "Simulation time in seconds", simSeconds);
    cmd.Parse(argc, argv);

    Time simTime = Seconds(simSeconds);

    // Create nodes
    NodeContainer ueNode; ueNode.Create(1);
    NodeContainer enbNode; enbNode.Create(1);
    NodeContainer remoteHostContainer; remoteHostContainer.Create(1);
    Ptr<Node> remoteHost = remoteHostContainer.Get(0);

    // LTE Helper + EPC
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    Ptr<Node> pgw = epcHelper->GetPgwNode();

    // Install internet stack
    InternetStackHelper internet;
    internet.Install(ueNode);
    internet.Install(remoteHost);
    internet.Install(pgw);

    // PGW <-> RemoteHost link
    PointToPointHelper p2ph;
    p2ph.SetDeviceAttribute("DataRate", StringValue("1Gbps"));
    p2ph.SetChannelAttribute("Delay", StringValue("10ms"));

    NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
    Ipv4AddressHelper ipv4h;
    ipv4h.SetBase("1.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign(internetDevices);
    Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress(1);

    // Routing for remoteHost
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> remoteStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
    remoteStaticRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

    // Mobility: eNB fixed, UE moves at ueSpeed
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNode);

    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(ueNode);
    ueNode.Get(0)->GetObject<ConstantVelocityMobilityModel>()
        ->SetVelocity(Vector(ueSpeed, 0.0, 0.0));

    // Install LTE devices
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNode);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNode);

    // Attach UE to eNodeB
    lteHelper->Attach(ueLteDevs.Get(0), enbLteDevs.Get(0));

    // Assign IP to UE
    Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address(ueLteDevs);

    // Set default route on UE
    Ptr<Ipv4StaticRouting> ueStaticRouting =
        ipv4RoutingHelper.GetStaticRouting(ueNode.Get(0)->GetObject<Ipv4>());
    ueStaticRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);

    // UDP server on UE
    UdpServerHelper udpServer(port);
    ApplicationContainer serverApp = udpServer.Install(ueNode.Get(0));
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(simTime);

    // UDP client (OnOff) on RemoteHost
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(ueIpIface.GetAddress(0), port));
    onoff.SetConstantRate(DataRate("1Mbps"));
    onoff.SetAttribute("StartTime", TimeValue(Seconds(2.0)));
    onoff.SetAttribute("StopTime", TimeValue(simTime));
    ApplicationContainer clientApp = onoff.Install(remoteHost);

    // FlowMonitor
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    Simulator::Stop(simTime);
    Simulator::Run();

    // Save results to XML
    monitor->SerializeToXmlFile("qos-results.xml", true, true);

    Simulator::Destroy();
    return 0;
}

