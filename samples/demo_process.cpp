#include <iostream>
#include "../someip/DiscoveryProcess/Process.h"
#include <thread>

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;


const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cOfferingPort{5555};
const uint16_t cFindingPort{6666};


int main()
{
    Poller* poller;
    poller = new Poller();
    

    ServiceRegistryProcess *registry;
    registry = new ServiceRegistryProcess(poller,cNicIpAddress,cMulticastGroup,cOfferingPort,cFindingPort);


    registry->printRegistry();




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