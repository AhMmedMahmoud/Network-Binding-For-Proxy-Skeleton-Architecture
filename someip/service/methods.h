#ifndef METHODS_H
#define METHODS_H

#include <iostream>
#include "../rpc/socket_rpc_client.h"

using namespace ara::com::someip::rpc;


class sumOverVector
{
    private:
    SocketRpcClient *mRpcClient;
    uint16_t mserviceId;
    uint16_t mClientId;
    uint16_t mMethodId;

    public:
    sumOverVector(SocketRpcClient* client, uint16_t serviceId, uint16_t clientId, uint16_t methodId) 
    {
       this->mRpcClient = client;
       this->mMethodId = methodId;
       this->mserviceId = serviceId;
       this->mClientId = clientId;
    }

    void operator() (const std::vector<uint8_t> &payload)
    {
        mRpcClient->Request(mserviceId, mMethodId, mClientId, payload);
    }
};

class multiplicationOverVector
{
    private:
    SocketRpcClient *mRpcClient;
    uint16_t mserviceId;
    uint16_t mClientId;
    uint16_t mMethodId;

    public:
    multiplicationOverVector(SocketRpcClient* client, uint16_t serviceId, uint16_t clientId, uint16_t methodId) 
    {
       this->mRpcClient = client;
       this->mMethodId = methodId;
       this->mserviceId = serviceId;
       this->mClientId = clientId;
    }

    void operator() (const std::vector<uint8_t> &payload)
    {
        mRpcClient->Request(mserviceId, mMethodId, mClientId, payload);
    }
};

class method
{
private:
    SocketRpcClient *mRpcClient;
    uint16_t mserviceId;
    uint16_t mClientId;
    uint16_t mMethodId;

public:
    method(SocketRpcClient* client, uint16_t serviceId, uint16_t clientId, uint16_t methodId) 
    {
       this->mRpcClient = client;
       this->mMethodId = methodId;
       this->mserviceId = serviceId;
       this->mClientId = clientId;
    }

    void operator() (const std::vector<uint8_t> &payload)
    {
        mRpcClient->Request(mserviceId, mMethodId, mClientId, payload);
    }
};



#endif