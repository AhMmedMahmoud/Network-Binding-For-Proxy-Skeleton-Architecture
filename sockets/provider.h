#ifndef PROVIDER_H
#define PROVIDER_H

#include <stdint.h>
#include <vector>
#include <future>
#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <algorithm>
#include "../sockets/include/poller.h"
#include "../sockets/include/tcp_client.h"
#include "../helper/concurrent_queue.h"


#include "../../sockets/include/poller.h"
#include "../../helper/concurrent_queue.h"
#include "rpcs_responser.h"
#include <algorithm>
#include <iostream>

using namespace ara::com::helper;

class Provider
{
public:   
    using ReceivingHandler = std::function<void(const std::vector<uint8_t> &)>;

private:
    /************************  attributes ***************************/
    ReceivingHandler myReceivingHandle;

    // queue at which we get received data
    ConcurrentQueue<std::vector<uint8_t>> mMessageBuffer;
    // mutex object
    std::mutex mMutex;
    // trier to take mutex
    std::unique_lock<std::mutex> mLock;
    std::condition_variable mConditionVariable;
 

    /*********************** poller attributes  *******************/

    static const size_t cBufferSize{256};
    ConcurrentQueue<std::vector<uint8_t>> mSendingQueue;
    AsyncBsdSocketLib::Poller *const mPoller;
    AsyncBsdSocketLib::TcpListener mServer;



    /********************** poller functions  *********************/

    void onAccept();
    void onReceive();
    void onSend();


    /*********************** internal functions **********************/

    // this function is called at receiving data
    void InvokeHandler(const std::vector<uint8_t> &payload);


public:
    /**************************** Constructor  ***********************/
    Provider( AsyncBsdSocketLib::Poller *poller,
               std::string ipAddress,
               uint16_t port,
               ReceivingHandler h = nullptr);


    /****************** fundemental functions *************************/

    void Send(const std::vector<uint8_t> &payload);


    /********************* desctructor  *******************************/

    ~Provider();
};

#endif