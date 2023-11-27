#include <iostream>

#include "../someip/pubsub/someip_pubsub_client.h"
#include "../someip/pubsub/socket_pubSub.h"

#include "../asyncbsdsocket/include/poller.h"
#include <thread>

using namespace ara::com::someip::pubsub;
using namespace ara::com::someip::sd;

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
    SomeIpPubSubClient(
                      helper::SockeKPubSub<sd::SomeIpSdMessage> *networkLayer,
                      uint8_t counter
                      );
    */
     
    SomeIpPubSubClient client(sock, 1);
    
    /*
    void Subscribe(uint16_t serviceId, uint16_t instanceId, uint8_t majorVersion, uint16_t eventgroupId);
    */
   client.Subscribe(4369,1234,12,1234);


    /*
    bool SomeIpPubSubClient::TryGetProcessedSubscription(int duration, sd::SomeIpSdMessage &message)
    */
   SomeIpSdMessage message;
   


   const int cTimeoutMs = 100;

    // Create thread using a lambda expression
    std::thread t1([poller,cTimeoutMs](){
       while(1)
       {
         poller->TryPoll(cTimeoutMs);
       }
    });

    if(client.TryGetProcessedSubscription(150,message) == 1)
    {
            std::cout << "subscription is done\n";
            // subscription is done
    }
    else
    {
        std::cout << "subscription is failed\n";
        // timeout
    }

    // Join the thread with the main thread
    t1.join();

   delete poller;
   return 0;
}