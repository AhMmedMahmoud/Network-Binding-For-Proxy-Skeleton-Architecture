// headers
#include <iostream>
#include "../V2X_interfaces/CAM_VDP_Proxy.h"
#include <thread>
#include <chrono> // for delay


// namespaces
using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;
using namespace ara::com;
using namespace ara::com::proxy;


// global variables
bool executing = true;
Poller* poller;
const int cTimeoutMs = 100;


// class that simulates CAM_VDP class
class CAM_VDP
{
    public:
        void V2X_CAM_setCam(std::vector<uint8_t> data)
        {
            std::cout << "V2X_CAM_setCam function is called\n";
            for(uint8_t x : data) 
            {
                std::cout << static_cast<int>(x) << "\n";
            }
        }
};


int main()
{
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "before finding \n";
    std::cout << "-----------------------------------------------------------\n";

    InstanceIdentifier id("7");
    auto handles = CAM_VDP_Proxy::findSerivce(id);
    
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "after finding \n";
    std::cout << "-----------------------------------------------------------\n\n\n";

    if(!handles.empty())
    {
        CAM_VDP_Proxy myProxy(handles[0]);

        // Create thread using a lambda expression
        poller = handles[0].getRequester()->getPoller();
        std::thread t1([](){
            while(executing)
            {
            poller->TryPoll(cTimeoutMs);
            }
        });

        CAM_VDP myCAM_VDP_Object;
        auto handle = std::bind(&CAM_VDP::V2X_CAM_setCam, &myCAM_VDP_Object, std::placeholders::_1);
        myProxy.SetReceiveHandler(handle);
        
        std::cout << "---------- subscription state before requesting subscribe ---------\n";
        myProxy.printSubscriptionState();
        // Introduce a delay of 7 seconds
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        
        std::cout << "\n\n\n----------------------- requesting subscribe ----------------------\n";
        myProxy.subscribe(256);
        while(myProxy.GetSubscriptionState() != helper::SubscriptionState::kSubscribed)
        {
            //std::cout << "not subscribed yet ...\n";
        }
        
        std::cout << "subscribed\n";

        /*
        std::cout << "\n\n------------------------------------\n";
        std::cout << "sleep for 10 seconds\n";
        std::cout << "---------------------------------------\n\n\n";
        // Introduce a delay of 7 seconds
        std::this_thread::sleep_for(std::chrono::seconds(10));
        */

        std::cout << "\n\n\ntest is done\n";
        t1.join();
    }
    else
    {
        std::cout << "handles are empty\n";
    }

    return 0;   
}