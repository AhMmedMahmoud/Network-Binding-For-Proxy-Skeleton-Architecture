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
                bool EventServer::isRequestingToSubscription(const rpc::SomeIpRpcMessage &request)
                {     
                    if(
                        (request.MessageType() == SomeIpMessageType::Request)
                        && (request.ProtocolVersion() == mProtocolVersion)
                        && (request.InterfaceVersion() == mInterfaceVersion)
                        && (request.MessageId() == ( (((uint32_t)mServiceId)<<16) | (uint32_t)mEventgroupId))
                    )
                    {
                        return true;
                    }
                    return false;
                }

                void EventServer::SetHandler(ProcessingHandler handler)
                {
                    myProcessHandle = handler;
                }
           
                void EventServer::InvokeEventHandler(const rpc::SomeIpRpcMessage &request)
                {  
                    if(isRequestingToSubscription(request))
                    {
                        std::cout << "it is request to Subscribe\n";
                        request.print();

                        std::vector<uint8_t> emptyVec;
                        rpc::SomeIpRpcMessage message(  request.MessageId(),
                                                        request.ClientId(),
                                                        request.SessionId(),
                                                        mProtocolVersion,
                                                        mInterfaceVersion,
                                                        SomeIpMessageType::Response,
                                                        SomeIpReturnCode::eOK,
                                                        emptyVec);

                        helper::PubSubState _state = GetState();
                        //printCurrentState();
                        
                        if (_state == helper::PubSubState::NotSubscribed)
                        {
                            //std::cout <<"$$$$ current state NotSubscribed $$$$\n";
                            mNotSubscribedState.Subscribed();
                            printCurrentState();
                        }
                        else if (_state == helper::PubSubState::Subscribed)
                        {
                            //std::cout <<"$$$$ current state Subscribed $$$$\n";
                            mSubscribedState.Subscribed();
                            printCurrentState();
                        }

                        Send(message.Payload());
                    }
                }


                /******************************* constructor ******************************/

                EventServer::EventServer( uint16_t serviceId,
                                                uint16_t instanceId,
                                                uint8_t majorVersion,
                                                uint16_t eventgroupId,
                                                uint8_t protocolVersion,
                                                uint8_t interfaceVersion): mServiceId{serviceId},
                                                                        mInstanceId{instanceId},
                                                                        mMajorVersion{majorVersion},
                                                                        mEventgroupId{eventgroupId},
                                                                        mProtocolVersion{protocolVersion},
                                                                        mInterfaceVersion{interfaceVersion}
                {
                    mStateMachine.Initialize({&mServiceDownState,
                                              &mNotSubscribedState,
                                              &mSubscribedState},
                                             helper::PubSubState::ServiceDown);
                }


                void EventServer::printCurrentState() const
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

                void EventServer::Start()
                {                  
                    helper::PubSubState _state = GetState();
                    if (_state == helper::PubSubState::ServiceDown)
                    {
                        mServiceDownState.Started();
                        printCurrentState();
                    }
                }

                helper::PubSubState EventServer::GetState() const noexcept
                {
                    return mStateMachine.GetState();
                }

                void EventServer::Stop()
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

                void EventServer::update(const std::vector<uint8_t> &data)
                {
                    rpc::SomeIpRpcMessage message(  ( (((uint32_t)mServiceId)<<16) | ((uint32_t)mEventgroupId)),
                                                    1,
                                                    cInitialSessionId++,
                                                    mProtocolVersion,
                                                    mInterfaceVersion,
                                                    SomeIpMessageType::Notification,
                                                    SomeIpReturnCode::eOK,
                                                    data);
                    
                    Send(message.Payload());
                }


                /******************************* destructor ******************************/

                EventServer::~EventServer()
                {
                    Stop();
                }
            }
        }
    }
}