#include <iostream>
#include "../someip/service/provider.h"
#include <thread>

#include "../helper/ipv4_address.h"

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;


const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cServiceDiscoveryOfferingPort{5555};


const uint16_t cServiceId = 10;
const uint16_t cInstanceId = 3;
const uint8_t cMajorVersion = 10;
const uint8_t cMinorVersion = 4;
const uint16_t mEventgroupId = 5;
const uint8_t cProtocolVersion = 20;
const uint16_t cInterfaceVersion = 2;
const uint16_t cClientId = 1;


int main()
{
    Poller* poller;
    poller = new Poller();
    
    /*
    Provider(  uint16_t serviceId,
               uint16_t instanceId,
               uint8_t majorVersion,
               uint8_t minorVersion,
               uint16_t eventgroupId, 
               AsyncBsdSocketLib::Poller *poller,
               std::string nicIpAddress,
               std::string multicastGroup,
               uint16_t port,
               uint16_t endpointPort,
               uint8_t protocolVersion,
               uint8_t interfaceVersion = 1);
    */

    Ipv4Address endpointIp("127.0.0.1");
    Provider *provider;
    provider = new Provider( cServiceId,
                             cInstanceId,
                             cMajorVersion,
                             cMinorVersion,
                             mEventgroupId,
                             poller,
                             cNicIpAddress,
                             cMulticastGroup,
                             cServiceDiscoveryOfferingPort,
                             7000,
                             8000,
                             cProtocolVersion);


   const int cTimeoutMs = 100;

   // Create thread using a lambda expression
   std::thread t1([poller,cTimeoutMs](){
      while(1)
      {
      poller->TryPoll(cTimeoutMs);
      }
   });

   provider->init();

   std::cout << "before offering the service\n";

   provider->offerService(endpointIp);
   
   std::cout << "after offering the service\n";

   int counter = false;
   std::vector<uint8_t> data;
   while(1)
   {    
      if(provider->eventServer->GetState() == PubSubState::Subscribed && counter == false)
      {
         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "---preparing sample1 to send---\n";
         data= {1,2,3,4};
         provider->eventServer->update(data);

         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "\n---preparing sample2 to send---\n";
         data= {4,5,6,7};
         provider->eventServer->update(data);

         // Introduce a delay of 7 seconds
         std::this_thread::sleep_for(std::chrono::seconds(7));
         std::cout << "\n---preparing sample2 to send---\n";
         data= {8,9,10,11};
         provider->eventServer->update(data);

         counter = true;
      } 
   }
   





   // Join the thread with the main thread
   t1.join();
   delete poller;
   return 0;
}