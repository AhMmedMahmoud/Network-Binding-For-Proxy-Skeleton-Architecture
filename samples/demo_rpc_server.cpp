#include <iostream>
#include "../someip/rpc/socket_rpc_server.h"
#include "../sockets/include/poller.h"

using namespace ara::com::someip::rpc;
using namespace AsyncBsdSocketLib;
using namespace std;
using HandlerType = std::function<bool(const std::vector<uint8_t> &, std::vector<uint8_t> &)>;



uint8_t summationOverVectorImp(const std::vector<uint8_t> &list)
{
   uint8_t sum = 0;
   for (int i = 0; i < list.size(); i++) {
        sum += list[i];
   }
   
   return sum;
}


uint8_t multiplicationOverVectorImp(const std::vector<uint8_t> &list)
{
   int sum = 1;
    for (int i = 0; i < list.size(); i++) {
        sum *= list[i];
    }
   
   return sum;
}


bool summationOverVector(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) 
{
    std::cout << "\nsummationOverVector is called\n";

    uint8_t funcResult = summationOverVectorImp(input);

    output.push_back(funcResult); // Put the sum in the output vector

    return true;
}


bool multiplicationOverVector(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) 
{
    std::cout << "\nmultiplicationOverVector is called\n";

    uint8_t funcResult = multiplicationOverVectorImp(input);

    output.push_back(funcResult); // Put the sum in the output vector

    return true;
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
 
    SocketRpcServer server(poller,cIpAddresss, cPort, cProtocolVersion, cInterfaceVersion);
    
    
    server.SetHandler(4660, 1000, (HandlerType)summationOverVector);

    server.SetHandler(4660, 2000, (HandlerType)multiplicationOverVector);


    while(1)
    {
        poller->TryPoll(cTimeoutMs);
    }


    return 0;
}