#include <iostream>
#include "../someip/service/event_rpcs/requester.h"
#include <thread>

//#include "../helper/ipv4_address.h"

// for delay
#include <chrono>

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;


const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cServiceDiscoveryFindingPort{6666};


const uint16_t cServiceId = 10;
const uint16_t cInstanceId = 3;
const uint8_t cMajorVersion = 10;
const uint8_t cMinorVersion = 4;
const uint16_t mEventgroupId = 5;


const uint8_t cProtocolVersion = 20;
const uint16_t cInterfaceVersion = 2;
const uint16_t cClientId = 1;

bool running = true;
const int cTimeoutMs = 100;
Poller* poller;


int main()
{
    poller = new Poller();

    Requester *requester;
    requester = new Requester(cServiceId,
                              cInstanceId,
                              cMajorVersion,
                              cMinorVersion,
                              mEventgroupId,
                              poller,
                              cNicIpAddress,
                              cMulticastGroup,
                              cServiceDiscoveryFindingPort,
                              cProtocolVersion);
  

    // Create thread using a lambda expression
    std::thread t1([](){
        while(running)
        {
        poller->TryPoll(cTimeoutMs);
        }
    });

    
    std::cout << "--------- before finding ----------\n";

    bool _result = requester->findService();
    if(!_result)
    {
        std::cout << "service not found\n";
        running = false;
        t1.join();
        delete poller;
        return 0;
    }
    std::cout << "--------- after finding ----------\n";


#if(EXAMPLE == RPCS)

    std::vector<uint8_t> input = {4, 2, 13, 4, 45};

/*
    requester->sum(input);

    std::this_thread::sleep_for(std::chrono::seconds(7));

#if(debuging == 1)
    std::cout << "***** after 7 seconds ******\n";
#endif

    requester->multiply(input);
  
    std::this_thread::sleep_for(std::chrono::seconds(7));

#if(debuging == 1)
    std::cout << "***** after 7 seconds ******\n";
#endif
*/    

#if(debuging == 1)
    std::cout << "before calling getSum\n";
#endif
    std::vector<uint8_t> output3;

    std::future<bool> futureObj3 = requester->calculateSum(input,output3);

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
    std::cout << "---------- requesting subscribe ---------\n";
    
    requester->eventClient->Subscribe();

    if(requester->eventClient->isSubscribed(3000) == 1)
    {
        std::cout << "subscription is done\n";

        std::vector<uint8_t> data;
        std::future<bool> futureObj = requester->eventClient->getter(data);
        if(futureObj.get())
        {
            std::cout << "data received\n";
            for (int i = 0; i < data.size(); i++) {
                std::cout << static_cast<int>(data[i])  << " ";
            }
            std::cout << "\n";
        }

        std::vector<uint8_t> data2;
        std::future<bool> futureObj2 = requester->eventClient->getter(data2);
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
        std::future<bool>futureObj3 = requester->eventClient->setter(data3);
        if(futureObj3.get())
        {
        std::cout << "setter function is executed\n";
        }
    }
    else
    {
        std::cout << "subscription is failed\n";
        // timeout
    }
#endif



    std::cout << "test is done\n";




    // Join the thread with the main thread
    t1.join();
   delete poller;
   return 0;
}