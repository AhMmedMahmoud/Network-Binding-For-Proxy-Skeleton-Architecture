#include <iostream>
#include "../someip/service/requester.h"
#include <thread>

#include "../helper/ipv4_address.h"

using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;


const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cPort{6666};


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
    Requester(
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint8_t minorVersion,
                        uint16_t eventgroupId, 
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);
    */

    Requester *requester;
    requester = new Requester(cServiceId,
                              cInstanceId,
                              cMajorVersion,
                              cMinorVersion,
                              mEventgroupId,
                              poller,
                              cNicIpAddress,
                              cMulticastGroup,
                              cPort,
                              cProtocolVersion);

    const int cTimeoutMs = 100;

    // Create thread using a lambda expression
    std::thread t1([poller,cTimeoutMs](){
       while(1)
       {
         poller->TryPoll(cTimeoutMs);
       }
    });

    
    requester->findService();

    std::string ipAddress;
    uint16_t port;

    /*
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


    // Join the thread with the main thread
    t1.join();

   delete poller;
   return 0;
}