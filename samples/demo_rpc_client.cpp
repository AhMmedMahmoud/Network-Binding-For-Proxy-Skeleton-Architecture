#include <iostream>
#include "../someip/rpc/socket_rpc_client.h"
#include "../sockets/include/poller.h"

using namespace ara::com::someip::rpc;
using namespace AsyncBsdSocketLib;
using HandlerType = std::function<void(const SomeIpRpcMessage &)>;

/************************************ constants ******************************/

const std::string cIpAddresss{"127.0.0.1"};
const uint16_t cPort{9900};

const int cTimeoutMs = 1;

const uint16_t cServiceId = 4500;
const uint16_t cSumationOverVectorMethodId = 1000;
const uint16_t cMultiplicationOverVectorMethodID = 2000;
const uint16_t cGetSumMethodID = 3000;

const uint8_t cProtocolVersion = 20;
const uint16_t cInterfaceVersion = 2;
const uint16_t cClientId = 1;


Poller* poller = new Poller();
SocketRpcClient client(poller, cIpAddresss, cPort, cProtocolVersion, cInterfaceVersion);


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

    if(  message.MessageId() == ((((uint32_t)cServiceId) <<16) | ((uint32_t)cSumationOverVectorMethodId))  )   
    {
        std::vector<uint8_t> payload = message.RpcPayload();
        // Print each element in the payload of response message
        std::cout << "\nresult received of summation over vector: ";
        for (uint8_t byte : payload) {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }
    else if(  message.MessageId() == ((((uint32_t)cServiceId) <<16) | ((uint32_t)cMultiplicationOverVectorMethodID))  )   
    {
         std::vector<uint8_t> payload = message.RpcPayload();
        // Print each element in the payload of response message
        std::cout << "\nresult received of multication over vector: ";
        for (uint8_t byte : payload) {
            std::cout << static_cast<int>(byte) << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "else\n";
    }
}

class sumOverVector
{
    public:
    void operator() (const std::vector<uint8_t> &payload)
    {
        client.Request(cServiceId, cSumationOverVectorMethodId, cClientId, payload);
    }
};

class multiplicationOverVector
{
    public:
    void operator() (const std::vector<uint8_t> &payload)
    {
        client.Request(cServiceId, cMultiplicationOverVectorMethodID, cClientId, payload);
    }
};


class getSum
{
    public:
    std::future<bool> operator() ( const std::vector<uint8_t> &payload,
                                   std::vector<uint8_t> &data)
    {
        return client.RequestWithoutHandler(cServiceId, cGetSumMethodID, cClientId, payload, data);
    }
};


int main()
{
      // Create thread using a lambda expression
    std::thread t1([](){
       while(1)
       {
         poller->TryPoll(cTimeoutMs);
       }
    });

    
    /*
    // regist handler for result of a method that calculates sum of all elements in vector
    client.SetHandler(cServiceId, cSumationOverVectorMethodId, (HandlerType)myHandle);
    
    // regist handler for result of a method that calculates sum of all elements in vector
    client.SetHandler(cServiceId, cMultiplicationOverVectorMethodID, (HandlerType)myHandle);

    sumOverVector sumOverVector;
    multiplicationOverVector multiplicationOverVector;



    std::vector<uint8_t> payload = {1, 2, 3, 4, 5};
    sumOverVector(payload);
    multiplicationOverVector(payload);
    */

    getSum getSum;
    std::vector<uint8_t> input = {1, 2, 3, 4, 5};
    std::vector<uint8_t> output;

    std::cout << "before calling getSum\n";
    std::future<bool> futureObj = getSum(input, output);
    std::cout << "after calling getSum\n";
    if(futureObj.get())
    {
        std::cout << "inside if\n";
        for (uint8_t val : output) {
          std::cout << static_cast<int>(val) ;
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "rrrrrrrr\n";
    }

    std::cout << "done\n\n";

    // Join the thread with the main thread
    t1.join();

    delete poller;
    return 0;
}