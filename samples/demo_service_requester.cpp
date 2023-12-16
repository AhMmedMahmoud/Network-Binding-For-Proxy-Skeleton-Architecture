#include <iostream>
#include "../someip/service/requester.h"
#include <thread>

#include "../helper/ipv4_address.h"

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

#define RPCS      0
#define PUBSUB    1

#define EXAMPLE RPCS
#define debuging 0

int main()
{
    Poller* poller;
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

    const int cTimeoutMs = 100;

    // Create thread using a lambda expression
    std::thread t1([poller,cTimeoutMs](){
       while(1)
       {
         poller->TryPoll(cTimeoutMs);
       }
    });

    
    std::cout << "--------- before finding ----------\n";

    requester->findService();

    /*
    std::string ipAddress;
    uint16_t port;
    if( requester->TryGetTransportInfo(4000,ipAddress,port) )
    {
        std::cout << "--- transport info is get ---\n";
        std::cout << "ip address : " <<ipAddress << std::endl;
        std::cout << "port address : " << port << std::endl;
    }
    else
    {
        std::cout << "not transport info is get\n";
    }
    */

    //requester->RequestSubscribe(cServiceId,cInstanceId,cMajorVersion,mEventgroupId);
    
    /*
    bool _result = requester->init();
    if(_result)
    {
        std::vector<uint8_t> payload = {1, 2, 3, 4, 5};
        requester->sum(payload);
        requester->multiply(payload);
    }
    else{
        std::cout << "failed in initializion \n";
    }
    */
    
    std::cout << "--------- after finding ----------\n";

#if(EXAMPLE == RPCS)

    //std::vector<uint8_t> data = {1,2,3,4};
    //requester->rpcClient->SetHandler(data);

    std::vector<uint8_t> input = {1, 2, 3, 4, 5};

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
    

#if(debuging == 1)
    std::cout << "before calling getSum\n";
#endif
    std::vector<uint8_t> output;

    std::future<bool> futureObj = requester->calculateSum(input,output);

#if(debuging == 1)
    std::cout << "after calling getSum\n";
#endif

    if(futureObj.get())
    {
        std::cout << "result of calculateSum : ";
        for (uint8_t val : output) {
          std::cout << static_cast<int>(val) ;
        }
        std::cout << "\n";
    }

    

#elif(EXAMPLE == PUBSUB)    
    requester->eventClient->Subscribe(1);

    SomeIpRpcMessage message;
    if(requester->eventClient->isSubscribed(3000,message) == 1)
    {
        std::cout << "subscription is done\n";
        //message.print();
        // subscription is done

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
        std::future<bool>futureObj3 = requester->eventClient->setter(data3);
        std::cout << "waiting for setting function\n";
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