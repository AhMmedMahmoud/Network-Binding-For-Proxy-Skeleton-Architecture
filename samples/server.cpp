#include <iostream>
#include "../someip/rpc/socket_rpc_server.h"
#include "../asyncbsdsocket/include/poller.h"

using namespace ara::com::someip::rpc;
using namespace AsyncBsdSocketLib;
using namespace std;



using HandlerType = std::function<bool(const std::vector<uint8_t> &, std::vector<uint8_t> &)>;

bool summationOverVector(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) 
{
    std::cout << "summationOverVector is called\n";

    int sum = 0;
    for (int i = 0; i < input.size(); i++) {
        sum += input[i];
    }

    output.push_back(sum); // Put the sum in the output vector

    return true;
}


bool multiplicationOverVector(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) 
{
    std::cout << "multiplicationOverVector is called\n";

    int sum = 1;
    for (int i = 0; i < input.size(); i++) {
        sum *= input[i];
    }

    output.push_back(sum); // Put the sum in the output vector

    return true;
}


int main()
{
    /*
     SocketRpcServer(
                        AsyncBsdSocketLib::Poller *poller,
                        std::string ipAddress,
                        uint16_t port,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);
    */
    Poller* poller;
    poller = new Poller();
    SocketRpcServer server(poller,"127.0.0.1",9900,12);
    


    /*
        void SetHandler(uint16_t serviceId, uint16_t methodId, HandlerType handler);
    */
    server.SetHandler(4660, 4369, (HandlerType)summationOverVector);

    server.SetHandler(1000, 2000, (HandlerType)multiplicationOverVector);


    const int cTimeoutMs = 1;
    while(1)
    {
        poller->TryPoll(cTimeoutMs);
    }


    return 0;
}