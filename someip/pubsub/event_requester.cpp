#include "event_requester.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace pubsub
            {
                /******************************* constructor ******************************/

                EventRequester::EventRequester(
                    uint8_t counter,
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion )  : mProtocolVersion{protocolVersion},
                                                 mInterfaceVersion{interfaceVersion},
                                                 mCounter{counter},
                                                 mSubscriptionLock(mSubscriptionMutex, std::defer_lock),
                                                 mValidNotify{true}
                {}



                /******************** function take any someip/sd message *****************/
                static bool gotAck = false;
                void EventRequester::InvokeEventHandler(sd::SomeIpSdMessage &&message)
                {
                    // for printing
                    if(message.Entries().size() == 0)
                    {
                        std::cout << "\n------------------------------------------------\n";
                        std::cout << ".t.t.t.t.received message.t.t.t.t. \n";
                        message.print();
                        std::cout << "-------------------------------------------------\n\n";
                    }

                    for (auto &entry : message.Entries())
                    {
                        if (entry->Type() == entry::EntryType::Acknowledging && !gotAck)
                        {
                            std::cout << "\n------------------------------------------------\n";
                            std::cout << ".....received message..... \n";
                            message.print();
                            std::cout << "-------------------------------------------------\n\n";
                            bool _enqueued = mMessageBuffer.TryEnqueue(std::move(message));
                            if (_enqueued)
                            {
                                mSubscriptionConditionVariable.notify_one();
                            }

                            gotAck = true;
                        }
                    }
                }

                /*
                void EventRequester::InvokeEventHandler(rpc::SomeIpRpcMessage &&message)
                {
                   // for printing
                    std::cout << "\n------------------------------------------------\n";
                    std::cout << ".....sample message..... \n";
                    message.print();
                    std::cout << "-------------------------------------------------\n\n";
                }
                */
                
                
                /******************************* fundemental functions *********************/

                void EventRequester::RequestSubscribe(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint16_t eventgroupId)
                {
                    // create someip/sd message with no entries and options
                    sd::SomeIpSdMessage _message;

                    // make entry to request subsription
                    auto _entry{
                        entry::EventgroupEntry::CreateSubscribeEventEntry(
                            serviceId, instanceId, majorVersion, mCounter, eventgroupId)};
                            
                    // add this entry to someip/sd message
                    _message.AddEntry(std::move(_entry));

                    // put this message into queue
                    Send(_message);
                }

                void EventRequester::RequestUnsubscribe(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint16_t eventgroupId)
                {
                    // create someip/sd message with no entries and options
                    sd::SomeIpSdMessage _message;

                    // make entry to request subsription
                    auto _entry{
                        entry::EventgroupEntry::CreateUnsubscribeEventEntry(
                            serviceId, instanceId, majorVersion, mCounter, eventgroupId)};

                    // add this entry to someip/sd message
                    _message.AddEntry(std::move(_entry));

                    Send(_message);
                }

                bool EventRequester::TryGetProcessedSubscription(
                    int duration,
                    sd::SomeIpSdMessage &message)
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



                /******************************* destructor ******************************/

                EventRequester::~EventRequester()
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