#include <iostream>
#include "../sm_interfaces/TriggerOutProxy.h"

#include <thread>
// for delay
#include <chrono>


using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;
using namespace ara::com;
using namespace ara::com::proxy;


bool executing = true;
Poller* poller;


int main()
{
    std::cout << "--------- before finding ----------\n";

    InstanceIdentifier id("4");
    auto handles = TriggerOutProxy::findSerivce(id);
    
    std::cout << "--------- after finding ----------\n";


    if(!handles.empty())
    {
        TriggerOutProxy myProxy(handles[0]);

        // Create thread using a lambda expression
        poller = handles[0].getRequester()->getPoller();
        std::thread t1([](){
            while(executing)
            {
            poller->TryPoll(cTimeoutMs);
            }
        });


        std::cout << "---------- subscription state before requesting subscribe ---------\n";
        myProxy.printSubscriptionState();
        // Introduce a delay of 7 seconds
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        
        std::cout << "\n\n\n----------------------- requesting subscribe ----------------------\n";
        myProxy.subscribe(256);
        while(myProxy.GetSubscriptionState() != helper::SubscriptionState::kSubscribed)
        {
            std::cout << "not subscribed yet ...\n";
        }        
        std::cout << "subscribed\n";


        std::cout << "test is done\n";
        t1.join();
    }
    else
    {
        std::cout << "handles are empty\n";
    }

    return 0;   
}