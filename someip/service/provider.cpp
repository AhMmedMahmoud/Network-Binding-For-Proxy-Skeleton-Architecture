#include <algorithm>
#include "provider.h"
#include <iostream>
#include "../someipSdMsg/entry/eventgroup_entry.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                const std::string Provider::cAnyIpAddress("0.0.0.0");



                /*************************** methods that i provide ***************************/

                uint8_t summationOverVectorImp(const std::vector<uint8_t> &list)
                {
                uint8_t sum = 0;
                for (int i = 0; i < list.size(); i++) {
                        sum += list[i];
                }
                return sum;
                }

                uint8_t multiplicationOverVectorImp(const std::vector<uint8_t> &list)
                {
                int sum = 1;
                    for (int i = 0; i < list.size(); i++) {
                        sum *= list[i];
                    }
                return sum;
                }

                bool summationOverVector(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) 
                {
                    std::cout << "\nsummationOverVector is called\n";

                    uint8_t funcResult = summationOverVectorImp(input);

                    output.push_back(funcResult); // Put the sum in the output vector

                    return true;
                }

                bool multiplicationOverVector(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) 
                {
                    std::cout << "\nmultiplicationOverVector is called\n";

                    uint8_t funcResult = multiplicationOverVectorImp(input);

                    output.push_back(funcResult); // Put the sum in the output vector

                    return true;
                }


                void Provider::init()
                {
                    rpcServer = new rpc::SocketRpcServer(mPoller,"127.0.0.1", mEndpointPort, mProtocolVersion, mInterfaceVersion);

                    rpcServer->SetHandler(mServiceId, cSumationOverVectorMethodId, (HandlerTypeFunc)summationOverVector);
                    rpcServer->SetHandler(mServiceId, cMultiplicationOverVectorMethodID, (HandlerTypeFunc)multiplicationOverVector);
                }

                /******************************* constructors  ******************************/

                Provider::Provider(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint8_t minorVersion,
                    uint16_t eventgroupId, 
                    AsyncBsdSocketLib::Poller *poller,
                    std::string nicIpAddress,
                    std::string multicastGroup,
                    uint16_t port,
                    uint16_t endpointPort,
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion): mServiceId{serviceId},
                                            mInstanceId{instanceId},
                                            mMajorVersion{majorVersion},
                                            mMinorVersion{minorVersion},
                                            mEventgroupId{eventgroupId},
                                            mProtocolVersion{protocolVersion},
                                            mInterfaceVersion{interfaceVersion},
                                            mPoller{poller},
                                            cNicIpAddress{nicIpAddress},
                                            cMulticastGroup{multicastGroup},
                                            cPort{port},
                                            mEndpointPort{endpointPort},
                                            mUdpSocket(cAnyIpAddress, port, nicIpAddress, multicastGroup)
                {
                    mStateMachine.Initialize({&mServiceDownState,
                                              &mNotSubscribedState,
                                              &mSubscribedState},
                                             helper::PubSubState::ServiceDown);

                    bool _successful{mUdpSocket.TrySetup()};
                    if (!_successful)
                    {
                        throw std::runtime_error("UDP socket setup failed.");
                    }

                    auto _receiver{std::bind(&Provider::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mUdpSocket, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    auto _sender{std::bind(&Provider::onSend, this)};
                    _successful = mPoller->TryAddSender(&mUdpSocket, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }
                }


                /******************************* fundemental functions *********************/

                void Provider::Start()
                {                  
                    helper::PubSubState _state = GetState();
                    if (_state == helper::PubSubState::ServiceDown)
                    {
                        mServiceDownState.Started();
                    }
                }

                helper::PubSubState Provider::GetState() const noexcept
                {
                    return mStateMachine.GetState();
                }

                void Provider::Stop()
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

                void Provider::update()
                {
                    sd::SomeIpSdMessage _request;
                    
                    Send(_request);

                    // Increment the session ID for that specific message ID for the next send
                    _request.IncrementSessionId();
                }

                void Provider::offerService(helper::Ipv4Address ipAddress)
                {
                    init();
                    
                    // make SOMEIP/SD message
                    SomeIpSdMessage mOfferServiceMessage;

                    // prepare offering entry
                    auto _offerServiceEntry
                    {
                        entry::ServiceEntry::CreateOfferServiceEntry( mServiceId,
                                                                      mInstanceId,
                                                                      mMajorVersion,
                                                                      mMinorVersion
                                                                    )
                    };
                               
                    // prepare unicast endpoint option
                    auto _offerEndpointOption
                    {
                        option::Ipv4EndpointOption::CreateUnitcastEndpoint( false,
                                                                            ipAddress,
                                                                            option::Layer4ProtocolType::Tcp,
                                                                            mEndpointPort
                                                                          )
                    };

                    // add created option to created entry 
                    _offerServiceEntry->AddFirstOption(std::move(_offerEndpointOption));
                    
                    // add created entry to created message
                    mOfferServiceMessage.AddEntry(std::move(_offerServiceEntry));

                    // send the message
                    Send(mOfferServiceMessage);
                }

                

                /******************** internal functions *****************/

                void Provider::printCurrentState()
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

                // function take any someip/sd message
                void Provider::InvokeEventHandler(sd::SomeIpSdMessage &&message)
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

                void Provider::processEntry(const entry::EventgroupEntry *entry)
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



                /**************************** poller functions  **********************************/

                void Provider::onReceive()
                {
                    // define array to receive serialized SOMEIP/SD message
                    std::array<uint8_t, cBufferSize> _buffer;

                    std::string _ipAddress;
                    uint16_t _port;

                    // receive serialized SOMEIP/SD message in form of array not vector
                    ssize_t _receivedSize{mUdpSocket.Receive(_buffer, _ipAddress, _port)};
                    if (_receivedSize > 0 && _port == cPort && _ipAddress == cNicIpAddress)
                    {
                        const std::vector<uint8_t> cRequestPayload(
                            std::make_move_iterator(_buffer.begin()),
                            std::make_move_iterator(_buffer.begin() + _receivedSize));

                        // Create the received message from the received payload
                        sd::SomeIpSdMessage _receivedMessage{sd::SomeIpSdMessage::Deserialize(cRequestPayload)};
                        
                        // call function that contain what to do with received message
                        InvokeEventHandler(std::move(_receivedMessage));
                    }
                }

                
                void Provider::onSend()
                {
                    while (!mSendingQueue.Empty())
                    {
                        //std::cout << "------------ onSend ------------\n";
                        std::vector<uint8_t> _payload;
                        bool _dequeued{mSendingQueue.TryDequeue(_payload)};
                        if (_dequeued)
                        {       
                            // for printing
                            std::cout << "\n------------------------------------------------\n";
                            std::cout << ".....sent message..... \n";
                            SomeIpSdMessage toSend  = SomeIpSdMessage::Deserialize(_payload);
                            toSend.print();
                            std::cout << "--------------------------------------------------\n";
                            
                            std::array<uint8_t, cBufferSize> _buffer;
                            std::copy_n(
                                std::make_move_iterator(_payload.begin()),
                                _payload.size(),
                                _buffer.begin());

                            mUdpSocket.Send(_buffer, cMulticastGroup, cPort);
                        }
                    }
                }

                void Provider::Send(const SomeIpSdMessage &message)
                {
                    std::vector<uint8_t> _payload{message.Payload()};
                    mSendingQueue.TryEnqueue(std::move(_payload));
                }



                /**************************** deconstructor  ************************/

                Provider::~Provider()
                {
                    Stop();
                    mPoller->TryRemoveSender(&mUdpSocket);
                    mPoller->TryRemoveReceiver(&mUdpSocket);
                }
            }
        }
    }
}