#include <iostream>

#include "../someip/pubsub/socket_pubSub_client.h"
#include "../sockets/include/poller.h"
#include <thread>

using namespace ara::com::someip::pubsub;
using namespace ara::com::someip::sd;
using namespace AsyncBsdSocketLib;


const std::string cAnyIpAddress{"0.0.0.0"};
const std::string cNicIpAddress{"127.0.0.1"};
const std::string cMulticastGroup{"239.0.0.1"};
const uint16_t cPort{5555};
const uint8_t cMajorVersion = 12;
const int cTimeoutMs = 1;


int main()
{
  Poller* poller;
  poller = new Poller();
  
  SockeKPubSubClient client(poller,cNicIpAddress,cMulticastGroup,cPort,cMajorVersion);

  
  /*
    uint16_t serviceId = 4369
    uint16_t instanceId = 1234
    uint16_t eventgroupId = 1234
  */
  client.RequestSubscribe(4369, 1234, cMajorVersion, 1234);

  // Create thread using a lambda expression
  std::thread t1([poller]()
  {
    while(1)
    {
      poller->TryPoll(cTimeoutMs);
    }
  });


  SomeIpSdMessage message;
  if(client.TryGetProcessedSubscription(3000,message) == 1)
  {
    std::cout << "subscription is done\n";
    // message.print();
    // subscription is done
  }
  else
  {
    std::cout << "subscription is failed\n";
    // timeout
  }

  // Join the thread with the main thread
  t1.join();

  delete poller;
  return 0;
}