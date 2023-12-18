#include "rpcs_requester.h"
#include <iostream>
// for delay
#include <thread>
#include <chrono>

// #define debuging 0
#include "../../config.h" 

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                /*********************** useful for constructor of my child *****************/

                RpcsRequester::RpcsRequester(
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion) noexcept : mProtocolVersion{protocolVersion},
                                                         mInterfaceVersion{interfaceVersion},
                                                         mLock(mMutex, std::defer_lock),
                                                         mValidNotify{true}
                {}



                /**************************** useful for my child *************************/

                /*
                - take serialized SOMEIP received that represents response message
                - check if messageID for this message is one of my messages ids
                    - invoke registed handler that takes this message and processes it
                */
                void RpcsRequester::InvokeHandler(const std::vector<uint8_t> &payload)
                {
                    std::cout << "InvokeHandler is called\n";
                    try
                    {
                        //const SomeIpRpcMessage _message{SomeIpRpcMessage::Deserialize(payload)};
                        SomeIpRpcMessage _message{SomeIpRpcMessage::Deserialize(payload)};
                        

                        bool _enqueued = mMessageBuffer.TryEnqueue(std::move(_message));
                        if (_enqueued)
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(100));
                            std::cout << "wake up thread that setting promise\n";
                            mConditionVariable.notify_one();
                        }
                        else
                        {
                            std::cout << "88888888888888888888888888888888\n";
                        }  

                        /*
                        auto _itr{mHandlers.find(_message.MessageId())};
                        if (_itr != mHandlers.end())
                        {
                            std::cout << "handle is registed\n";
                            _itr->second(_message);
                        }
                        else
                        {
                            std::cout << "handle isnot registed\n";
                            bool _enqueued = mMessageBuffer.TryEnqueue(std::move(_message));
                            if (_enqueued)
                            {
                               std::this_thread::sleep_for(std::chrono::milliseconds(100));
                               std::cout << "wake up thread that setting promise\n";
                               mConditionVariable.notify_one();
                            }
                            else
                            {
                                std::cout << "88888888888888888888888888888888\n";
                            }   
                        }
                        */
                    }
                    catch (const std::exception& e) {
                        std::cout << "error are invoKeHal\n";
                    }
                    /*
                    catch (std::out_of_range)
                    {
                        std::cout << "error are invoKeHal\n";
                        // Ignore the corrupted RPC server response
                    }
                    */
                }



                /**************************** fundemental functions *************************/
                /*
                void RpcsRequester::SetHandler(
                    uint16_t serviceId, uint16_t methodId, HandlerType handler)
                {
                    try
                    {
                        auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                        _messageId |= methodId;
                        //mHandlers[_messageId] = handler;
                    }
                    catch(const std::bad_function_call& ex)
                    {
                        std::cerr << "111111111111111" << ex.what() << std::endl;
                    }
                    catch(const std::exception& e)
                    {
                        std::cerr << "aaaaaaaaaaaa" << e.what() << std::endl;
                    }


                    // original
                    
                    //auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                    //_messageId |= methodId;
                    //mHandlers[_messageId] = handler;
                    
                }
                */
                
                void RpcsRequester::Request(
                    uint16_t serviceId,
                    uint16_t methodId,
                    uint16_t clientId,
                    const std::vector<uint8_t> &rpcPayload)
                {
                    try
                    {
                        const uint16_t cInitialSessionId{1};

                        auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                        _messageId |= methodId;

                        auto _itr{mSessionIds.find(_messageId)};
                        uint16_t _sessionId{(_itr != mSessionIds.end()) ? _itr->second : cInitialSessionId};

                        // create message that represents a request
                        SomeIpRpcMessage _request(_messageId, clientId, _sessionId, mProtocolVersion, mInterfaceVersion, rpcPayload);

                        Send(_request.Payload());

                        // for printing
                        std::cout << "\n------------------------------------------------\n";
                        std::cout << ".....sent message..... \n";
                        _request.print();
                        std::cout << "--------------------------------------------------\n";

                        // Increment the session ID for that specific message ID for the next send
                        _request.IncrementSessionId();
                        mSessionIds[_messageId] = _request.SessionId();
                    }
                    catch (const std::bad_function_call& ex)
                    {
                        std::cerr << "1212121212121212" << ex.what() << std::endl;
                        // Handle the exception as needed
                    }
                    //catch(const std::bad_function_call& ex)
                    catch(const std::exception& e)
                    {
                        std::cerr << "2222222222222222222" << e.what() << std::endl;
                    }
                }

                











                std::future<bool> RpcsRequester::RequestWithoutHandler(
                    uint16_t serviceId,
                    uint16_t methodId,
                    uint16_t clientId,
                    const std::vector<uint8_t> &rpcPayload, std::vector<uint8_t> &data)
                {
                    const uint16_t cInitialSessionId{1};

                    auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                    _messageId |= methodId;

                    auto _itr{mSessionIds.find(_messageId)};
                    uint16_t _sessionId{(_itr != mSessionIds.end()) ? _itr->second : cInitialSessionId};

                    // create message that represents a request
                    SomeIpRpcMessage _request(_messageId, clientId, _sessionId, mProtocolVersion, mInterfaceVersion, rpcPayload);

                    std::vector<uint8_t> p = _request.Payload();
                    Send(p);

                    // for printing
                    std::cout << "\n------------------------------------------------\n";
                    std::cout << ".....sent message..... \n";
                    _request.print();
                    std::cout << "--------------------------------------------------\n";

                    // Increment the session ID for that specific message ID for the next send
                    _request.IncrementSessionId();
                    mSessionIds[_messageId] = _request.SessionId();

                    std::shared_ptr<std::promise<bool>> promisePtr = std::make_shared<std::promise<bool>>();
                    std::future<bool> future = promisePtr->get_future();

                    // In your waiting function
                    if (mMessageBuffer.Empty()) 
                    {
                        std::thread([&, promisePtr] 
                        {
                            std::cout << "-- buffer of received messages is empty --\n";
                            std::cout << "-- acquire the lock --\n";
                            mLock.lock();
                            std::cout << "----- waiting -----\n";
                            mConditionVariable.wait(mLock);
                            std::cout << "-- releasing lock the lock --\n";
                            mLock.unlock();
                            
                            if (mValidNotify) 
                            {
                                rpc::SomeIpRpcMessage message;
                                bool _result = mMessageBuffer.TryDequeue(message);
                                if (_result) 
                                {
                                    data = message.RpcPayload();
                                    promisePtr->set_value(true);
                                } else {
                                    promisePtr->set_value(false);
                                }
                            } 
                            else {  promisePtr->set_value(false);   }
                        }).detach();
                    } 
                    else 
                    {
                        std::cout << "buffer isnot empty\n";
                        rpc::SomeIpRpcMessage message;
                        bool _result = mMessageBuffer.TryDequeue(message);
                        if (_result) {
                            data = message.RpcPayload();
                        }
                        promisePtr->set_value(_result);
                    }

                    return future;
                }    
            }
        }
    }
}