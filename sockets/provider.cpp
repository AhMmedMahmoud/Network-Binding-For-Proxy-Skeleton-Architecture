#include "provider.h"

Provider::Provider( AsyncBsdSocketLib::Poller *poller,
                    std::string ipAddress,
                    uint16_t port,
                    ReceivingHandler h
                  ) noexcept :
                            mPoller{poller},
                            mServer{AsyncBsdSocketLib::TcpListener(ipAddress, port)},
                            myReceivingHandle{h},
                            mLock(mMutex, std::defer_lock)
{
    bool _successful{mServer.TrySetup()};
    if (!_successful)
    {
        throw std::runtime_error("TCP server socket setup failed.");
    }

    auto _listener{std::bind(&Provider::onAccept, this)};
    _successful = mPoller->TryAddListener(&mServer, _listener);
    if (!_successful)
    {
        throw std::runtime_error("Adding TCP server socket listener failed.");
    }
}


void Provider::InvokeHandler(const std::vector<uint8_t> &payload)
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

void Provider::Send(const std::vector<uint8_t> &payload)
{
    mSendingQueue.TryEnqueue(payload);                    
}


/**************************** poller functions  **********************************/

void Provider::onAccept()
{
    bool _successful{mServer.TryAccept()};

    if (!_successful)
    {
        throw std::runtime_error("Accepting RPC client TCP connection failed.");
    }

    _successful = mServer.TryMakeConnectionNonblock();
    if (!_successful)
    {
        throw std::runtime_error("Making non-blocking TCP connection failed.");
    }

    auto _receiver{std::bind(&Provider::onReceive, this)};
    _successful = mPoller->TryAddReceiver(&mServer, _receiver);
    if (!_successful)
    {
        throw std::runtime_error("Adding TCP server socket receiver failed.");
    }

    auto _sender{std::bind(&Provider::onSend, this)};
    _successful = mPoller->TryAddSender(&mServer, _sender);
    if (!_successful)
    {
        throw std::runtime_error(
            "Adding TCP server socket sender failed.");
    }
}

void Provider::onReceive()
{
    // define array to receive serialized SOMEIP message
    std::array<uint8_t, cBufferSize> _buffer;

    // receive serialized SOMEIP message in form of array not vector
    ssize_t _receivedSize{mServer.Receive(_buffer)};
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

void Provider::onSend()
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

            mServer.Send(_buffer);
        }
        else
        {
            std::cout << "cannot dequeue the message\n";
        }
    }
}


/****************************  deconstructor  ************************/

Provider::~Provider()
{
    mPoller->TryRemoveSender(&mServer);
    mPoller->TryRemoveReceiver(&mServer);
    mPoller->TryRemoveListener(&mServer);
}