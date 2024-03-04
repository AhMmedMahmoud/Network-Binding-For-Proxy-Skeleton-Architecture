#include "event_requester.h"
#include <thread>
#include <chrono>
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace pubsub
            {
                bool EventSubscripter::isValidNotification(const rpc::SomeIpRpcMessage &request)
                {
                    uint32_t _serviceId = (request.MessageId() & 0xffff0000);
                    uint32_t _eventgroupId =  (request.MessageId() & 0x0000ffff);
                    if( (request.MessageType() == SomeIpMessageType::Notification) &&  
                        (request.ProtocolVersion() == mProtocolVersion) &&
                        (request.InterfaceVersion() == mInterfaceVersion) &&
                        (request.MessageId() == (_serviceId | _eventgroupId))
                    )
                    {
                        return true;
                    }
                    else 
                    {
                        return false;
                    }
                }

                bool EventSubscripter::isSubscriptionAck(const rpc::SomeIpRpcMessage &request)
                {
                    uint32_t _serviceId = (request.MessageId() & 0xffff0000);
                    uint32_t _eventgroupId =  (request.MessageId() & 0x0000ffff);
                    if( (request.MessageType() == SomeIpMessageType::Response) &&  
                        (request.ProtocolVersion() == mProtocolVersion) &&
                        (request.InterfaceVersion() == mInterfaceVersion) &&
                        (request.MessageId() == (_serviceId | _eventgroupId))
                    )
                    {
                        return true;
                    }
                    else 
                    {
                        return false;
                    }
                }
                
                /******************************* constructor ******************************/

                EventSubscripter::EventSubscripter(
                            uint16_t serviceId,
                            uint16_t instanceId,
                            uint8_t majorVersion,
                            uint8_t minorVersion,
                            uint16_t eventgroupId,
                            uint8_t counter,
                            uint8_t protocolVersion,
                            uint8_t interfaceVersion)  : 
                                                mServiceId{serviceId},
                                                mInstanceId{instanceId},
                                                mMajorVersion{majorVersion},
                                                mMinorVersion{minorVersion},
                                                mEventgroupId{eventgroupId},
                                                mCounter{counter},
                                                mProtocolVersion{protocolVersion},
                                                mInterfaceVersion{interfaceVersion},
                                                mSubscriptionLock(mSubscriptionMutex, std::defer_lock),
                                                mValidNotify{true},
                                                state(helper::SubscriptionState::kNotSubscribed)
                {}



                void EventSubscripter::InvokeEventHandler(rpc::SomeIpRpcMessage &&message)
                {
                    if(isSubscriptionAck(message))
                    {
                        // update state to subscribed if i already request subscription
                        if(state == helper::SubscriptionState::kSubscriptionPending)
                        {
                            std::cout << "it is SubscriptionAck\n";

                            state = helper::SubscriptionState::kSubscribed;

                            return;
                        }
                    }
                    
                    if(isValidNotification(message))
                    {
                        if(state == helper::SubscriptionState::kSubscribed)
                        {
                            try
                            {
                                if(myReceivingHandle)
                                {
                                    std::cout << "there is handler for receiving registed\n";
                                    std::vector<uint8_t> receivedData = message.RpcPayload();
                                    myReceivingHandle(receivedData);
                                }
                                else
                                {
                                    std::cout << "no handler for receiving registed\n";
                                    bool _enqueued = mMessageBuffer.TryEnqueue(std::move(message));
                                    if (_enqueued)
                                    {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                        std::cout << "wake up thread that setting promise\n";
                                        mSubscriptionConditionVariable.notify_one();
                                    }
                                }
                            }
                            catch (const std::bad_function_call& ex)
                            {
                                std::cout << "failed due to received handler" << ex.what() << std::endl;
                                // Handle the exception as needed
                            }
                        }
                    }
                }


                
                /******************************* fundemental functions *********************/

                void EventSubscripter::Subscribe(size_t maxSampleCount)
                {
                    if(state != helper::SubscriptionState::kSubscribed)
                    {
                        std::vector<uint8_t> data;
                        uint32_t _serviceId = mServiceId;
                        uint32_t _eventgroupId =  mEventgroupId;
                        rpc::SomeIpRpcMessage message(
                                                    ( (((uint32_t)_serviceId)<<16) | ((uint32_t)_eventgroupId)),
                                                    mCounter,
                                                    cInitialSessionId++,
                                                    mProtocolVersion,
                                                    mInterfaceVersion,
                                                    data
                                                    );
                        Send(message);
                        state = helper::SubscriptionState::kSubscriptionPending;
                    }
                }

                std::future<bool> EventSubscripter::getter(std::vector<uint8_t> &data) 
                {
                    std::shared_ptr<std::promise<bool>> promisePtr = std::make_shared<std::promise<bool>>();
                    std::future<bool> future = promisePtr->get_future();

                    // In your waiting function
                    if (mMessageBuffer.Empty()) 
                    {
                        std::thread([&, promisePtr] 
                        {
                            std::cout << "-- buffer of received messages is empty --\n";
                            std::cout << "-- acquire the lock --\n";
                            mSubscriptionLock.lock();
                            std::cout << "----- waiting -----\n";
                            mSubscriptionConditionVariable.wait(mSubscriptionLock);
                            std::cout << "-- releasing lock the lock --\n";
                            mSubscriptionLock.unlock();
                            //std::cout << "-- if --\n";
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
                        rpc::SomeIpRpcMessage message;
                        bool _result = mMessageBuffer.TryDequeue(message);
                        if (_result) {
                            data = message.RpcPayload();
                        }
                        promisePtr->set_value(_result);
                    }

                    return future;
                }
                
                helper::SubscriptionState EventSubscripter::GetSubscriptionState() const
                {
                    return state;
                }

                void EventSubscripter::printCurrentState() const
                {
                    switch(state)
                    {
                        case helper::SubscriptionState::kNotSubscribed:
                        std::cout << "current state: NotSubscribed\n";
                        break;

                        case helper::SubscriptionState::kSubscribed:
                        std::cout << "current state: Subscribed\n";
                        break;

                        case helper::SubscriptionState::kSubscriptionPending :
                        std::cout << "current state: Subscription Pending\n";
                        break;

                        default:
                        break;
                    }
                }

                void EventSubscripter::SetReceiveHandler(ReceivingHandler h)
                {
                    myReceivingHandle = h;
                }

                void EventSubscripter::UnsetReceiveHandler()
                {
                    myReceivingHandle = nullptr;
                }

                /******************************* destructor ******************************/

                EventSubscripter::~EventSubscripter()
                {
                    // Condition variable notifications are not valid anymore during destruction.
                    mValidNotify = false;
                    // Release the threads waiting for the condition variables before desctruction
                    mSubscriptionConditionVariable.notify_one();
                }
            }
        }
    }
}