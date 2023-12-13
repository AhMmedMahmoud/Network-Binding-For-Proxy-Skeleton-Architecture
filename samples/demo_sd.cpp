#include <iostream>
#include "../someip/sd/network_layer.h"
#include "../someip/sd/SomeIpSdServer.h"
#include "../helper/ipv4_address.h"

using namespace ara::com::someip::sd;
using namespace ara::com::helper;

using namespace AsyncBsdSocketLib;


const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cPort{5555};


int main()
{
    Poller* poller;
    poller = new Poller();
    

    NetworkLayer *myNetwork;
    myNetwork = new NetworkLayer(poller,cNicIpAddress,cMulticastGroup,cPort);

    /*
    SomeIpSdServer(
                        helper::AbstractNetworkLayer<SomeIpSdMessage> *networkLayer,

                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint32_t minorVersion,

                        helper::Ipv4Address ipAddress,
                        uint16_t port,

                        int initialDelayMin,
                        int initialDelayMax,
                        int repetitionBaseDelay = 30,
                        int cycleOfferDelay = 1000,
                        uint32_t repetitionMax = 3);
    */

    Ipv4Address address("127.0.0.1");
    SomeIpSdServer server(myNetwork,1234,12,12,13,address,9090,5,10);
    
    server.Start();

    const int cTimeoutMs = 100;

    // Create thread using a lambda expression
    std::thread t1([poller,cTimeoutMs](){
       while(1)
       {
         poller->TryPoll(cTimeoutMs);
       }
    });

    
    // Join the thread with the main thread
    t1.join();

   delete poller;
   return 0;
}
