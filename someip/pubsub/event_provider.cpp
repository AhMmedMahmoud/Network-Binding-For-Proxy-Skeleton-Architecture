#include "../someipSdMsg/entry/eventgroup_entry.h"
#include "event_provider.h"
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

                EventProvider::EventProvider( uint16_t serviceId,
                                                uint16_t instanceId,
                                                uint8_t majorVersion,
                                                uint16_t eventgroupId): mServiceId{serviceId},
                                                                        mInstanceId{instanceId},
                                                                        mMajorVersion{majorVersion},
                                                                        mEventgroupId{eventgroupId}
                {
                    mStateMachine.Initialize({&mServiceDownState,
                                              &mNotSubscribedState,
                                              &mSubscribedState},
                                             helper::PubSubState::ServiceDown);
                }



                /******************** function take any someip/sd message *****************/

                void EventProvider::InvokeEventHandler(sd::SomeIpSdMessage &&message)
                {     
                    // Iterate over all the message entry to search for the first Event-group Subscribing entry
                    for (auto &_entry : message.Entries())
                    {
                        if (_entry->Type() == entry::EntryType::Subscribing)
                        {
                            // for printing
                            std::cout << "\n------------------------------------------------\n";
                            std::cout << ".....received message..... \n";
                            message.print();
                            std::cout << "-------------------------------------------------\n\n";
                            
                            if (auto _eventgroupEntry = dynamic_cast<entry::EventgroupEntry *>(_entry.get()))
                            {
                                // Compare service ID, instance ID, major version, and event-group ID
                                if ((_eventgroupEntry->ServiceId() == mServiceId) &&
                                    (_eventgroupEntry->InstanceId() == entry::Entry::cAnyInstanceId ||
                                     _eventgroupEntry->InstanceId() == mInstanceId) &&
                                    (_eventgroupEntry->MajorVersion() == entry::Entry::cAnyMajorVersion ||
                                     _eventgroupEntry->MajorVersion() == mMajorVersion) &&
                                    (_eventgroupEntry->EventgroupId() == mEventgroupId))
                                {
                                    if (_eventgroupEntry->TTL() > 0)
                                    {
                                        // Subscription
                                        processEntry(_eventgroupEntry);
                                    }
                                    else
                                    {
                                        // Unsubscription
                                        helper::PubSubState _state = GetState();
                                        if (_state == helper::PubSubState::Subscribed)
                                        {
                                            mSubscribedState.Unsubscribed();
                                        }
                                    }
                                    return;
                                }
                            }
                        }
                    }
                }

                void EventProvider::processEntry(const entry::EventgroupEntry *entry)
                {
                    const bool cDiscardableEndpoint{true};

                    sd::SomeIpSdMessage _acknowledgeMessage;

                    helper::PubSubState _state = GetState();
                    printCurrentState();
                    
                    if (_state == helper::PubSubState::NotSubscribed)
                    {
                        std::cout <<"$$$$ current state NotSubscribed $$$$\n";
                        mNotSubscribedState.Subscribed();
                        printCurrentState();
                    }
                    else if (_state == helper::PubSubState::Subscribed)
                    {
                        std::cout <<"$$$$ current state Subscribed $$$$\n";
                        mSubscribedState.Subscribed();
                        printCurrentState();
                    }

                    // Acknowledge the subscription if the service is up
                    auto _acknowledgeEntry =
                        _state == helper::PubSubState::ServiceDown ? entry::EventgroupEntry::CreateNegativeAcknowledgeEntry(entry)
                                                                   : entry::EventgroupEntry::CreateAcknowledgeEntry(entry);

                    // If the service is not down, add a multicast endpoint option to the acknowledgement entry
                    if (_state != helper::PubSubState::ServiceDown)
                    {
                        std::cout <<"$$$$ current state !ServiceDown $$$$\n";
                        helper::Ipv4Address mEndpointIp("239.0.0.1");
                        uint16_t mEndpointPort = 5555;                       
                        auto _multicastEndpoint = option::Ipv4EndpointOption::CreateMulticastEndpoint(
                                cDiscardableEndpoint, mEndpointIp, mEndpointPort);

                        _acknowledgeEntry->AddFirstOption(std::move(_multicastEndpoint));
                    }

                    _acknowledgeMessage.AddEntry(std::move(_acknowledgeEntry));
                    Send(_acknowledgeMessage);
                }


                
                void EventProvider::printCurrentState()
                {
                    switch(mStateMachine.GetState())
                    {
                        case helper::PubSubState::NotSubscribed:
                        std::cout << "current state: NotSubscribed\n";
                        break;

                        case helper::PubSubState::Subscribed:
                        std::cout << "current state: Subscribed\n";
                        break;

                        case helper::PubSubState::ServiceDown :
                        std::cout << "current state: ServiceDown\n";
                        break;

                        default:
                        break;
                    }
                }

                /******************************* fundemental functions *********************/

                void EventProvider::Start()
                {                  
                    helper::PubSubState _state = GetState();
                    if (_state == helper::PubSubState::ServiceDown)
                    {
                        mServiceDownState.Started();
                    }
                }

                helper::PubSubState EventProvider::GetState() const noexcept
                {
                    return mStateMachine.GetState();
                }

                void EventProvider::Stop()
                {
                    helper::PubSubState _state = GetState();
                    if (_state == helper::PubSubState::NotSubscribed)
                    {
                        mNotSubscribedState.Stopped();
                    }
                    else if (_state == helper::PubSubState::Subscribed)
                    {
                        mSubscribedState.Stopped();
                    }
                }

                void EventProvider::update()
                {
                    sd::SomeIpSdMessage _request;
                    
                    Send(_request);

                    // Increment the session ID for that specific message ID for the next send
                    _request.IncrementSessionId();
                }



                /******************************* destructor ******************************/

                EventProvider::~EventProvider()
                {
                    Stop();
                }
            }
        }
    }
}