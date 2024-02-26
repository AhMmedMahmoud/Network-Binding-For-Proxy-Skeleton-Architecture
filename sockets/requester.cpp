#include "requester.h"

Requester::Requester( AsyncBsdSocketLib::Poller *poller,
                      std::string ipAddress,
                      uint16_t port,
                      ReceivingHandler h
                    ) noexcept :
                            mPoller{poller},
                            mClient{AsyncBsdSocketLib::TcpClient(ipAddress, port)},
                            myReceivingHandle{h},
                            mLock(mMutex, std::defer_lock)
{
    bool _successful{mClient.TrySetup()};
    if (!_successful)
    {
        throw std::runtime_error("TCP client socket setup failed.");
    }


    _successful = mClient.TryConnect();
    if (!_successful)
    {
        throw std::runtime_error("TCP client socket connection failed.");
    }


    auto _sender{std::bind(&Requester::onSend, this)};
    _successful = mPoller->TryAddSender(&mClient, _sender);
    if (!_successful)
    {
        throw std::runtime_error("Adding TCP client socket sender failed.");
    }


    auto _receiver{std::bind(&Requester::onReceive, this)};
    _successful = mPoller->TryAddReceiver(&mClient, _receiver);
    if (!_successful)
    {
        throw std::runtime_error("Adding TCP client socket receiver failed.");
    }
}


void Requester::InvokeHandler(const std::vector<uint8_t> &payload)
{
    std::cout << "InvokeHandler is called\n";
    try
    {
        if(myReceivingHandle)
        {
            std::cout << "there is handler for receiving registed\n";
            myReceivingHandle(payload);
        }
        else
        {
            std::cout << "no handler for receiving registed\n";
            bool _enqueued = mMessageBuffer.TryEnqueue(payload);
            if (_enqueued)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "wake up thread that setting promise\n";
                mConditionVariable.notify_one();
            }
        }
    }
    catch (const std::bad_function_call& ex)
    {
        std::cout << "failed due to received handler" << ex.what() << std::endl;
        // Handle the exception as needed
    }                        
}


/**************************** fundemental functions *************************/

void Requester::Send(const std::vector<uint8_t> &payload)
{
    mSendingQueue.TryEnqueue(payload);                    
}


/************************** poller functions  ************************/

void Requester::onSend()
{
    while (!mSendingQueue.Empty())
    {
        std::vector<uint8_t> _payload;
        bool _dequeued{mSendingQueue.TryDequeue(_payload)};
        if (_dequeued)
        {
            std::array<uint8_t, cBufferSize> _buffer;
            std::copy_n(
                std::make_move_iterator(_payload.begin()),
                _payload.size(),
                _buffer.begin());

            mClient.Send(_buffer);
        }
        else
        {
            std::cout << "cannot dequeue the message\n";
        }
    }
}

void Requester::onReceive()
{
    // define array to receive serialized SOMEIP message
    std::array<uint8_t, cBufferSize> _buffer;
    
    // receive serialized SOMEIP message in form of array not vector
    ssize_t _receivedSize{mClient.Receive(_buffer)};
    if (_receivedSize > 0)
    {
        // convert serialized SOMEIP message from array into vector
        const std::vector<uint8_t> cPayload(
            std::make_move_iterator(_buffer.begin()),
            std::make_move_iterator(_buffer.begin() + _receivedSize));

        // call function that contain what to do with received message
        InvokeHandler(cPayload);
    }
}


/****************************  deconstructor  ************************/

Requester::~Requester()
{
    mPoller->TryRemoveReceiver(&mClient);
    mPoller->TryRemoveSender(&mClient);
}