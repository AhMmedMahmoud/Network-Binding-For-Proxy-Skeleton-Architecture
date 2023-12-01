#include <iostream>
#include "../someip/rpc/socket_rpc_client.h"
#include "../sockets/include/poller.h"

using namespace ara::com::someip::rpc;
using namespace AsyncBsdSocketLib;
using HandlerType = std::function<void(const SomeIpRpcMessage &)>;



/// @brief Invoke when server sent message
/// @param message response message
void myHandle(const SomeIpRpcMessage &message) 
{
    // for printing
    std::cout << "\n------------------------------------------------\n";
    std::cout << "....myHandle is executed...\n";
    std::cout << ".....received message..... \n";
    message.print();
    std::cout << "--------------------------------------------------\n";

    if(  message.MessageId() == ((((uint32_t)4660) <<16) | ((uint32_t)4369))  )   
    {
        std::vector<uint8_t> payload = message.RpcPayload();
        // Print each element in the payload of response message
        std::cout << "\nresult received of summation over vector: ";
        for (uint8_t byte : payload) {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }
    else if(  message.MessageId() == ((((uint32_t)1000) <<16) | ((uint32_t)2000))  )   
    {
         std::vector<uint8_t> payload = message.RpcPayload();
        // Print each element in the payload of response message
        std::cout << "\nresult received of multication over vector: ";
        for (uint8_t byte : payload) {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }
}


const std::string cIpAddresss{"127.0.0.1"};
const uint16_t cPort{9900};
const uint8_t cProtocolVersion = 20;
const uint16_t cInterfaceVersion = 2;
const int cTimeoutMs = 1;

int main()
{
    Poller* poller;
    poller = new Poller();


    SocketRpcClient client(poller, cIpAddresss, cPort, cProtocolVersion, cInterfaceVersion);

    // regist handler for result of a method that calculates sum of all elements in vector
    // this method of service whose id is 4660
    // this method has id 1000
    client.SetHandler(4660, 1000,(HandlerType)myHandle);
    

    // regist handler for result of a method that calculates sum of all elements in vector
    // this method of service whose id is 4660
    // this method has id 2000
    client.SetHandler(4660, 2000,(HandlerType)myHandle);


    /*  
        serviceId = 4660
        methodId = 4369      
        clientId = 1           
    */
    std::vector<uint8_t> payload1 = {1, 2, 3, 4, 5};
    std::vector<uint8_t> payload2 = {5, 5, 0, 0, 10};

    client.Request(4660, 1000, 1, payload1);

    client.Request(4660, 1000, 1, payload2);



    const int cTimeoutMs = 1;
    while(1)
    {
        poller->TryPoll(cTimeoutMs);
    }

    return 0;
}