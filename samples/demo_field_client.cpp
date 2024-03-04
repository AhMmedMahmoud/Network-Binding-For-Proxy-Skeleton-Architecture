#include <iostream>

#include "../someip/fields/socket_field_client.h"
#include "../sockets/include/poller.h"
#include <thread>

// for delay
#include <thread>
#include <chrono>

using namespace ara::com::someip::pubsub;
using namespace ara::com::someip::rpc;
using namespace ara::com::someip::sd;
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
  SockeKEventClient(
                      uint16_t serviceId,
                      uint16_t instanceId,
                      uint8_t majorVersion,
                      uint8_t minorVersion,
                      uint16_t eventgroupId,
                      uint8_t counter,
                      AsyncBsdSocketLib::Poller *poller,
                      std::string nicIpAddress,
                      std::string multicastGroup,
                      uint16_t port,
                      uint8_t protocolVersion,
                      uint8_t interfaceVersion = 1
                    );
  */

  SockeKFieldClient client( cServiceId,
                             cInstanceId,
                             cMajorVersion,
                             cMinorVersion,
                             cEventgroupId,
                             cClientId,
                             poller,
                             cNicIpAddress,
                             cMulticastGroup,
                             cPort,
                             cProtocolVersion);

  client.Subscribe(256);

  // Create thread using a lambda expression
  std::thread t1([poller]()
  {
    while(1)
    {
      poller->TryPoll(cTimeoutMs);
    }
  });


  while(client.GetSubscriptionState() != SubscriptionState::kSubscribed)
  {
      //std::cout << "not subscribed yet ...\n";
  }
  std::cout << "subscribe\n\n";


  std::vector<uint8_t> data;
  std::future<bool> futureObj = client.getter(data);
  if(futureObj.get())
  {
    std::cout << "data received\n";
    for (int i = 0; i < data.size(); i++) {
      std::cout << static_cast<int>(data[i])  << " ";
    }
    std::cout << "\n";
  }

  std::vector<uint8_t> data2;
  std::future<bool> futureObj2 = client.getter(data2);
  if(futureObj2.get())
  {
    std::cout << "data received\n";
    for (int i = 0; i < data2.size(); i++) {
      std::cout << static_cast<int>(data2[i])  << " ";
    }
    std::cout << "\n";
  }

  /*
  // Introduce a delay of 7 seconds
  std::this_thread::sleep_for(std::chrono::seconds(4));
  std::vector<uint8_t> data3 = {99,102,88};
  std::future<bool>futureObj3 = client.setter(data3);
  std::cout << "waiting for setting function\n";
  if(futureObj3.get())
  {
    std::cout << "setter function is executed\n";
  }
  */

  // Join the thread with the main thread
  t1.join();

  delete poller;
  return 0;
}