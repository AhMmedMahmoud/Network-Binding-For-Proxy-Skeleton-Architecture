#include "event_requester.h"

#define debuging 1

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace pubsub
            {
                void EventSubscripter::requestSetting(const std::vector<uint8_t> &data)
                {
                    rpc::SomeIpRpcMessage message(  ( (((uint32_t)mServiceId)<<16) | ((uint32_t)cRequestSetValueBySubscriberMethodId)),
                                                    mCounter,
                                                    cInitialSessionId++,
                                                    mProtocolVersion,
                                                    mInterfaceVersion,
                                                    SomeIpMessageType::Request,
                                                    SomeIpReturnCode::eOK,
                                                    data);
                    
                    Send(message);
                }

                void EventSubscripter::requestGetting()
                {
                    std::vector<uint8_t> data;
                    rpc::SomeIpRpcMessage message(  ( (((uint32_t)mServiceId)<<16) | ((uint32_t)cRequestGetValueBySubscriberMethodId)),
                                                    mCounter,
                                                    cInitialSessionId++,
                                                    mProtocolVersion,
                                                    mInterfaceVersion,
                                                    SomeIpMessageType::Request,
                                                    SomeIpReturnCode::eOK,
                                                    data);
                    
                    Send(message);
                }



                bool EventSubscripter::isResponseToSettingValue(const rpc::SomeIpRpcMessage &request)
                {
                    uint32_t _serviceId = (request.MessageId() & 0xffff0000);
                    uint32_t _eventgroupId =  (request.MessageId() & 0x0000ffff);
                    if( (request.MessageType() == SomeIpMessageType::Response) &&  
                        (request.ProtocolVersion() == mProtocolVersion) &&
                        (request.InterfaceVersion() == mInterfaceVersion) &&
                        (request.MessageId() == (_serviceId | cRequestSetValueBySubscriberMethodId))
                    )
                    {
                        return true;
                    }
                    else 
                    {
                        return false;
                    }
                }

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
                    if(isValidNotification(message))
                    {
                        // update state to subscribed if i already request subscription
                        if(state == helper::SubscriptionState::kSubscriptionPending)
                        {
#if(debuging == 1)
                            std::cout << "it is notification\n";
                            std::cout << "------------------------------------------------\n";
                            std::cout << ".....received message..... \n";
                            message.print();
                            std::cout << "-------------------------------------------------\n\n";
#endif
                            state = helper::SubscriptionState::kSubscribed;
                        }

                        if(state == helper::SubscriptionState::kSubscribed)
                        {
                            bool _enqueued = mMessageBuffer.TryEnqueue(std::move(message));
                            if (_enqueued)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                std::cout << "wake up thread that setting promise\n";
                                mSubscriptionConditionVariable.notify_one();
                            }
                        }
                    }
                    else if(isResponseToSettingValue(message))
                    {
#if(debuging == 1)
                        std::cout << "it is ResponseToSettingValue\n";
                        std::cout << "------------------------------------------------\n";
                        std::cout << ".....received message..... \n";
                        message.print();
                        std::cout << "-------------------------------------------------\n\n";
#endif
                    }
                    else
                    {
                        //std::cout << "ignore\n";
                    }
                }


                
                /******************************* fundemental functions *********************/

                void EventSubscripter::Subscribe(size_t maxSampleCount)
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

                /*
                bool EventSubscripter::isSubscribed(
                    int duration,
                    rpc::SomeIpRpcMessage &message)
                {
                    bool _result;
                    if (mMessageBuffer.Empty())
                    {
                        std::cout << "-- buffer of received messages is empty --\n";
                        mSubscriptionLock.lock();
                        std::cv_status _status = mSubscriptionConditionVariable.wait_for(
                                mSubscriptionLock, std::chrono::milliseconds(duration));
                        mSubscriptionLock.unlock();
                        _result = mValidNotify && (_status != std::cv_status::timeout);
                    }
                    else
                    {
                        std::cout << "-- buffer of receiver messages has messages --\n";
                        // There are still processed subscription messages in the buffer, so no need to wait.
                        _result = true;
                    }

                    // In the case of successful get, set the first processed subscription to the output argument
                    if (_result)
                    {
                        std::cout << "-- fill passed message with received message --\n";
                        _result = mMessageBuffer.TryDequeue(message);
                    }

                    return _result;
                }

                bool EventSubscripter::isSubscribed(int duration)
                {
                    if(state ==helper::SubscriptionState::kSubscribed)
                        return true;
                    else if(state ==helper::SubscriptionState::kNotSubscribed)
                        return false;

                    bool _result;
                    if (mMessageBuffer.Empty())
                    {
                        std::cout << "-- buffer of received messages is empty --\n";
                        mSubscriptionLock.lock();
                        std::cv_status _status = mSubscriptionConditionVariable.wait_for(
                                mSubscriptionLock, std::chrono::milliseconds(duration));
                        mSubscriptionLock.unlock();
                        _result = mValidNotify && (_status != std::cv_status::timeout);
                    }
                    else
                    {
                        std::cout << "-- buffer of receiver messages has messages --\n";
                        // There are still processed subscription messages in the buffer, so no need to wait.
                        _result = true;
                    }

                    return _result;
                }
                */

                

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
             
                std::future<bool> EventSubscripter::setter(const std::vector<uint8_t> &data)
                {
                    auto promise_ = std::make_shared<std::promise<bool>>(); // Create a local shared_ptr
                    std::future<bool> future = promise_->get_future();

                    requestSetting(data);

                    std::async(std::launch::async, [this, promise_]() { // Capture promise_ in the lambda
                        std::vector<uint8_t> received;
                        bool result = getter(received).get();  // Use get() to retrieve the value from the future
                        promise_->set_value(result);
                    });

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