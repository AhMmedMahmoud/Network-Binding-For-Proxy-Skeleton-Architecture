#include <iostream>
#include "../someip/rpc/socket_rpc_client.h"
#include "../asyncbsdsocket/include/poller.h"

using namespace ara::com::someip::rpc;
using namespace AsyncBsdSocketLib;
using HandlerType = std::function<void(const SomeIpRpcMessage &)>;



/// @brief Invoke when server sent message
/// @param message response message
void myHandle(const SomeIpRpcMessage &message) 
{
    /*
    std::cout << "\n------------------------------------------------\n";
    std::cout << "myHandle is executed\n";

    std::cout << ".....response message..... \n";
    std::cout << "sevice id: " << (message.MessageId() >> 16) << std::endl;
    std::cout << "method id: "<< ((message.MessageId() << 16) >> 16) << std::endl;
    std::cout << "lenght: " << message.Length() << std::endl;
    std::cout << "client id: " << message.ClientId() << std::endl;
    std::cout << "session id: " << message.SessionId() << std::endl;
    std::cout << "protocol version: " << 12 << std::endl;
    std::cout << "interface version: " << 1 << std::endl;
    std::cout << "message type: " << static_cast<int>(message.MessageType()) << std::endl;
    std::cout << "return code: " << static_cast<int>(message.ReturnCode()) << std::endl;
    */

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
    std::cout << "------------------------------------------------\n";
}


int main()
{
    Poller* poller;
    poller = new Poller();

    /*
        SocketRpcServer(
                        AsyncBsdSocketLib::Poller *poller,
                        std::string ipAddress,
                        uint16_t port,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1
                        );
    */
    SocketRpcClient client(poller, "127.0.0.1", 9900, 12);

    /*
        void SetHandler(uint16_t serviceId, uint16_t methodId, HandlerType handler);
    */
    // regist handler for result of a method that calculates sum of all elements in vector
    // this method of service whose id is 4660
    // this method has id 0x1111
    client.SetHandler(4660, 4369,(HandlerType)myHandle);
    
    client.SetHandler(1000, 2000,(HandlerType)myHandle);


    /*
       void Send(uint16_t serviceId, uint16_t methodId, uint16_t clientId, const std::vector<uint8_t> &rpcPayload);
    */
    /*  send someip message
        -->   serviceId = 4660     <--
        -->   methodId = 4369      <--
        -->   clientId = 1           <--
        -->   sessionId = sequence number will start from 1 and keep increase after each send
        -->   protocolVersion = 12 
        -->   interfaceVersion = 1
        -->   messageType = Request
        -->   returnCode = eOk
        -->   payload =  1,2,3,4,5   <--
    */
    std::vector<uint8_t> payload = {1, 2, 3, 4, 5};

    client.RpcClient::Send(4660, 4369, 1, payload);


    client.RpcClient::Send(1000, 2000, 1, payload);



    const int cTimeoutMs = 1;
    while(1)
    {
        poller->TryPoll(cTimeoutMs);
    }

    return 0;
}