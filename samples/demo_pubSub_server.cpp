#include <iostream>

#include "../someip/pubsub/someip_pubsub_server.h"
#include "../someip/pubsub/socket_pubSub.h"
#include "../helper/ipv4_address.h"
#include "../asyncbsdsocket/include/poller.h"

using namespace ara::com::someip::pubsub;
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
    
    /*
    SockeKPubSub(
            AsyncBsdSocketLib::Poller *poller,
            std::string nicIpAddress,
            std::string multicastGroup,
            uint16_t port);
    */

    SockeKPubSub* sock;
    sock = new SockeKPubSub(poller,cNicIpAddress,cMulticastGroup,cPort);


    /*
    SomeIpPubSubServer(
                        helper::NetworkLayer<sd::SomeIpSdMessage> *networkLayer,
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint16_t eventgroupId,
                        helper::Ipv4Address ipAddress,
                        uint16_t port);
    */
    Ipv4Address addr(cMulticastGroup);
    
    SomeIpPubSubServer server(sock,4369,1234,12,1234,addr,cPort);
    
    server.Start();

   const int cTimeoutMs = 1;
   while(1)
   {
       poller->TryPoll(cTimeoutMs);
   }
   return 0;
}