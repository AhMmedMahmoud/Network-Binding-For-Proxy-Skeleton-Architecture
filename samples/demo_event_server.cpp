#include <iostream>

#include "../someip/events/socket_event_server.h"
#include "../helper/ipv4_address.h"
#include "../sockets/include/poller.h"
#include "../helper/state.h"

// for delay
#include <thread>
#include <chrono>

using namespace ara::com::someip::pubsub;
using namespace ara::com::someip::sd;
using namespace ara::com::someip::rpc;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;





/************************************ constants ******************************/

const int cTimeoutMs = 100;

const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cPort{5555};

const uint16_t cServiceId = 10;
const uint16_t cInstanceId = 3;
const uint8_t cMajorVersion = 10;
const uint8_t cMinorVersion = 4;
const uint16_t cEventgroupId = 5;
const uint8_t cProtocolVersion = 20;
const uint16_t cInterfaceVersion = 2;
const uint16_t cClientId = 1;

int main()
{
  Poller* poller;
  poller = new Poller();
  
  /*
  SockeKEventServer(
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint16_t eventgroupId, 
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);
  */
  
  SockeKEventServer server( cServiceId,
                            cInstanceId,
                            cMajorVersion,
                            cEventgroupId,
                            poller,
                            cNicIpAddress,
                            cMulticastGroup,
                            cPort,
                            cProtocolVersion);
  
  // Create thread using a lambda expression
  std::thread t1([poller]()
  {
    while(1)
    {
      poller->TryPoll(cTimeoutMs);
    }
  });


  std::vector<uint8_t> currentValue = {47,48,49};
  bool _result = server.putCurrentValue(currentValue);
  if(_result)
  {
    server.Start();
    
    int counter = false;
    std::vector<uint8_t> data;
    while(1)
    {    
        if(server.GetState() == PubSubState::Subscribed && counter == false)
        {
          // Introduce a delay of 7 seconds
          std::this_thread::sleep_for(std::chrono::seconds(7));
          std::cout << "---preparing sample1 to send---\n";
          data= {1,2,3,4};
          server.update(data);

          // Introduce a delay of 7 seconds
          std::this_thread::sleep_for(std::chrono::seconds(7));
          std::cout << "\n---preparing sample2 to send---\n";
          data= {4,5,6,7};
          server.update(data);

          // Introduce a delay of 7 seconds
          std::this_thread::sleep_for(std::chrono::seconds(7));
          std::cout << "\n---preparing sample2 to send---\n";
          data= {8,9,10,11};
          server.update(data);

          counter = true;
        } 
    }
  }

  
  

   // Join the thread with the main thread
  t1.join();

  delete poller;
  return 0;
}