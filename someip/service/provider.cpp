#include "provider.h"


namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
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
                    uint16_t endpointRpcsPort,
                    uint16_t endpointEventPort,
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
                                            mEndpointRpcsPort{endpointRpcsPort},
                                            mEndpointEventPort{endpointEventPort},
                                            mUdpSocket(cAnyIpAddress, port, nicIpAddress, multicastGroup)
                {
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
    
                void Provider::init()
                {
                    /********* initilization for rpcs */
                    rpcServer = new rpc::SocketRpcServer( mPoller,
                                                         "127.0.0.1",
                                                          mEndpointRpcsPort,
                                                          mProtocolVersion,
                                                          mInterfaceVersion);

                    rpcServer->SetHandler( mServiceId,
                                           cSumationOverVectorMethodId,
                                           (HandlerTypeFunc)summationOverVector
                                         );
                
                    rpcServer->SetHandler(mServiceId, cMultiplicationOverVectorMethodID, (HandlerTypeFunc)multiplicationOverVector);

                    /************** initialization for events ***************/            
                    /*
                    SockeKEventServer server(mServiceId,
                                            mInstanceId,
                                            mMajorVersion,
                                            mEventgroupId,
                                            mPoller,
                                            cNicIpAddress,
                                            cMulticastGroup,
                                            mEndpointEventPort,
                                            mProtocolVersion);
                    
                    eventServer->Start();
                    */
                }

                void Provider::offerService(helper::Ipv4Address ipAddress)
                {                    
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
                                                                            mEndpointRpcsPort
                                                                          )
                    };

                    // add created option to created entry 
                    _offerServiceEntry->AddFirstOption(std::move(_offerEndpointOption));
                    
                    // add created entry to created message
                    mOfferServiceMessage.AddEntry(std::move(_offerServiceEntry));

                    // send the message
                    Send(mOfferServiceMessage);
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
                        //InvokeEventHandler(std::move(_receivedMessage));
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
                    mPoller->TryRemoveSender(&mUdpSocket);
                    mPoller->TryRemoveReceiver(&mUdpSocket);
                }
            }
        }
    }
}