#include <iostream>
#include "../sm_interfaces/TriggerInProxy.h"

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

    InstanceIdentifier id("3");
    auto handles = TriggerInProxy::findSerivce(id);
    
    std::cout << "--------- after finding ----------\n";


    if(!handles.empty())
    {
        TriggerInProxy myProxy(handles[0]);

        // Create thread using a lambda expression
        poller = handles[0].getRequester()->getPoller();
        std::thread t1([](){
            while(executing)
            {
            poller->TryPoll(cTimeoutMs);
            }
        });

 
        std::cout << "---------- requesting subscribe ---------\n";
        myProxy.subscribe();

        if(myProxy.isSubscribed(3000) == 1)
        {
            std::cout << "----------subscription is done ---------\n";

            std::cout << "---------------- getter ----------------\n";
            std::vector<uint8_t> data;
            std::future<bool> futureObj = myProxy.getter(data);
            if(futureObj.get())
            {
                std::cout << "data received\n";
                for (int i = 0; i < data.size(); i++) {
                    std::cout << static_cast<int>(data[i])  << " ";
                }
                std::cout << "\n";
            }

            std::cout << "\n\n------------------------------------\n";
            std::cout << "sleep for 10 seconds\n";
            std::cout << "---------------------------------------\n\n\n";

            // Introduce a delay of 7 seconds
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::vector<uint8_t> data3 = {4,4,4};
            std::cout << "waiting for setting function\n";
            std::future<bool>futureObj3 = myProxy.setter(data3);
            if(futureObj3.get())
            {
            std::cout << "setter function is executed\n";
            }

            std::cout << "\n\n------------------------------------\n";
            std::cout << "sleep for 10 seconds\n";
            std::cout << "---------------------------------------\n\n\n";
            std::this_thread::sleep_for(std::chrono::seconds(10));

            /* 
            std::cout << "before requesting getting\n";
            myProxy.requestGetting();
            std::cout << "after requesting getting\n";
            */

            std::vector<uint8_t> data2;
            std::cout << "---------------- getter ----------------\n";
            std::future<bool> futureObj2 = myProxy.getter(data2);
            if(futureObj2.get())
            {
                std::cout << "data received\n";
                for (int i = 0; i < data2.size(); i++) {
                    std::cout << static_cast<int>(data2[i])  << " ";
                }
                std::cout << "\n";
            }
        }
        else
        {
            std::cout << "subscription is failed\n";
        }

        std::cout << "test is done\n";
        t1.join();
    }
    else
    {
        std::cout << "handles are empty\n";
    }

    return 0;   
}