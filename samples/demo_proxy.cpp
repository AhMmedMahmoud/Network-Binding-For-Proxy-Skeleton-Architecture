#include <iostream>
#include "../Proxy_Skeleton/Proxy.h"
#include <thread>
#include <chrono> // for delay


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
    auto handles = Proxy::findSerivce(id);
    
    std::cout << "--------- after finding ----------\n";

    if(!handles.empty())
    {
        Proxy myProxy(handles[0]);

        // Create thread using a lambda expression
        poller = handles[0].getRequester()->getPoller();
        std::thread t1([](){
            while(executing)
            {
            poller->TryPoll(cTimeoutMs);
            }
        });

#if(EXAMPLE == RPCS)

#if(debuging == 1)
    std::cout << "before calling getSum\n";
#endif

    std::vector<uint8_t> input = {4, 2, 13, 4, 45};

    std::vector<uint8_t> output3;

    std::future<bool> futureObj3 = myProxy.calculateSum(input,output3);

#if(debuging == 1)
    std::cout << "after calling getSum\n";
#endif

    if(futureObj3.get())
    {
        std::cout << "result of calculateSum : ";
        for (uint8_t val : output3) {
          std::cout << static_cast<int>(val) ;
        }
        std::cout << "\n";
    }

#elif(EXAMPLE == PUBSUB)    
        std::cout << "\n\n\n----------------------- requesting subscribe ----------------------\n";
        myProxy.subscribe(256);
        while(myProxy.GetSubscriptionState() != helper::SubscriptionState::kSubscribed)
        {
            std::cout << "not subscribed yet ...\n";
        }
        
        std::cout << "subscribed\n";

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


        std::vector<uint8_t> data2;
        std::future<bool> futureObj2 = myProxy.getter(data2);
        if(futureObj2.get())
        {
            std::cout << "data received\n";
            for (int i = 0; i < data2.size(); i++) {
                std::cout << static_cast<int>(data2[i])  << " ";
            }
            std::cout << "\n";
        }


        // Introduce a delay of 7 seconds
        std::this_thread::sleep_for(std::chrono::seconds(4));
        std::vector<uint8_t> data3 = {99,102,88};
        std::cout << "waiting for setting function\n";
        std::future<bool>futureObj3 = myProxy.setter(data3);
        if(futureObj3.get())
        {
          std::cout << "setter function is executed\n";
        }
#endif

        std::cout << "test is done\n";
        t1.join();
    }
    else
    {
        std::cout << "handles are empty\n";
    }

    return 0;   
}