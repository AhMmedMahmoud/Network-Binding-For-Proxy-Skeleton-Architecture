#include <iostream>

#include "../someip/pubsub/socket_pubSub_server.h"
#include "../helper/ipv4_address.h"
#include "../sockets/include/poller.h"
#include "../helper/machine_state.h"

// for delay
#include <thread>
#include <chrono>

using namespace ara::com::someip::pubsub;
using namespace ara::com::someip::sd;
using namespace ara::com::helper;
using namespace AsyncBsdSocketLib;

/************************************ constants ******************************/

const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cPort{5555};

const int cTimeoutMs = 1;

uint16_t cServiceId = 4500;
uint16_t cInstanceId = 1000;
uint16_t cEventgroupId = 2000;
uint8_t cMajorVersion = 12;

int main()
{
  Poller* poller;
  poller = new Poller();
  
  SockeKPubSubServer server(cServiceId,
                            cInstanceId,
                            cMajorVersion,
                            cEventgroupId,
                            poller,
                            cNicIpAddress,
                            cMulticastGroup,
                            cPort);

  server.Start();

  

  // Create thread using a lambda expression
  std::thread t1([poller]()
  {
    while(1)
    {
      poller->TryPoll(cTimeoutMs);
    }
  });

  bool counter = false;
  while(1)
  {    
    if(server.GetState() == PubSubState::Subscribed && counter == false)
    {
      // Introduce a delay of 7 seconds
      std::this_thread::sleep_for(std::chrono::seconds(7));
      std::cout << "---preparing sample to send---\n";
      server.update();
      counter = true;
    } 
  }

   // Join the thread with the main thread
  t1.join();

  delete poller;
  return 0;
}