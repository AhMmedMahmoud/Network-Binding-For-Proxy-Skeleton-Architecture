// headers
#include <iostream>
#include "../V2X_interfaces/DENM_VDP_Proxy.h"
#include "../V2X_interfaces/DENM_Control_Proxy.h"
#include <thread>
#include <chrono>   // for delay

// namespaces
using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;
using namespace ara::com;
using namespace ara::com::proxy;


// global variables
bool executing = true;
Poller* poller_DENM_VDP;
Poller* poller_DENM_Control;
const int cTimeoutMs = 100;


int main()
{
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "before finding DENM_VDP\n";
    std::cout << "-----------------------------------------------------------\n";


    InstanceIdentifier id_DENM_VDP("8");
    auto handles_DENM_VDP = DENM_VDP_Proxy::findSerivce(id_DENM_VDP);


    if(!handles_DENM_VDP.empty())
    {
        DENM_VDP_Proxy myProxy_DENM_VDP(handles_DENM_VDP[0]);

        std::cout << "-----------------------------------------------------------\n";
        std::cout << "before finding DENM_Control\n";
        std::cout << "-----------------------------------------------------------\n";

        InstanceIdentifier id_DENM_Control("9");
        auto handles_DENM_Control = DENM_Control_Proxy::findSerivce(id_DENM_Control);


        if(!handles_DENM_Control.empty())
        {
            DENM_Control_Proxy myProxy_DENM_Control(handles_DENM_Control[0]);

            // Create thread using a lambda expression
            poller_DENM_VDP = handles_DENM_VDP[0].getRequester()->getPoller();
            poller_DENM_Control = handles_DENM_Control[0].getRequester()->getPoller();
            std::thread t1([](){
                while(executing)
                {
                poller_DENM_VDP->TryPoll(cTimeoutMs);
                poller_DENM_Control->TryPoll(cTimeoutMs);
                }
            });


            std::cout << "---------- subscription state before requesting subscribe ---------\n";
            myProxy_DENM_VDP.printSubscriptionState();
            // Introduce a delay of 7 seconds
            std::this_thread::sleep_for(std::chrono::seconds(10));
            
            
            std::cout << "\n\n\n----------------------- requesting subscribe ----------------------\n";
            myProxy_DENM_VDP.subscribe(256);
            while(myProxy_DENM_VDP.GetSubscriptionState() != helper::SubscriptionState::kSubscribed)
            {
                //std::cout << "not subscribed yet ...\n";
            }        
            std::cout << "subscribed\n";

            
            std::cout << "---------- subscription state before requesting subscribe ---------\n";
            myProxy_DENM_Control.printSubscriptionState();
            // Introduce a delay of 7 seconds
            std::this_thread::sleep_for(std::chrono::seconds(10));
            
            
            std::cout << "\n\n\n----------------------- requesting subscribe ----------------------\n";
            myProxy_DENM_Control.subscribe(256);
            while(myProxy_DENM_Control.GetSubscriptionState() != helper::SubscriptionState::kSubscribed)
            {
                //std::cout << "not subscribed yet ...\n";
            }        
            std::cout << "subscribed\n";
            

            std::cout << "test is done\n";
            t1.join();
        }
        else
        {
            std::cout << "handles for DENM_Control are empty\n";
        }
    }
    else
    {
        std::cout << "handles for DENM_VDP are empty\n";
    }

    return 0;   
}