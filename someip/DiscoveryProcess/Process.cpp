#include "Process.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                const size_t ServiceDiscoveryProcess::cBufferSize{256};
                const std::string ServiceDiscoveryProcess::cAnyIpAddress("0.0.0.0");


                /******************************* constructors  ******************************/

                ServiceDiscoveryProcess::ServiceDiscoveryProcess(
                    AsyncBsdSocketLib::Poller *poller,
                    std::string nicIpAddress,
                    std::string multicastGroup,
                    uint16_t offeringPort,
                    uint16_t findingPort) : cNicIpAddress{nicIpAddress},
                                     cMulticastGroup{multicastGroup},
                                     cOfferingPort{offeringPort},
                                     cFindingPort{findingPort},
                                     mPoller{poller},
                                     //mFindingRequestsLock(mFindingRequestsMutex, std::defer_lock),
                                     mOfferingUdpSocket(cAnyIpAddress, offeringPort, nicIpAddress, multicastGroup),
                                     mFindingUdpSocket(cAnyIpAddress, findingPort, nicIpAddress, multicastGroup)
                {
                    bool _successful{mFindingUdpSocket.TrySetup()};
                    if (!_successful)
                    {
                        throw std::runtime_error("UDP socket setup failed.");
                    }

                    auto _receiver{std::bind(&ServiceDiscoveryProcess::onReceiveFinding, this)};
                    _successful = mPoller->TryAddReceiver(&mFindingUdpSocket, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    auto _sender{std::bind(&ServiceDiscoveryProcess::onSendOfferingOrAck, this)};
                    _successful = mPoller->TryAddSender(&mFindingUdpSocket, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }


                    _successful = mOfferingUdpSocket.TrySetup();
                    if (!_successful)
                    {
                        throw std::runtime_error("UDP socket setup failed.");
                    }

                    auto _receiver2{std::bind(&ServiceDiscoveryProcess::onReceiveOffering, this)};
                    _successful = mPoller->TryAddReceiver(&mOfferingUdpSocket, _receiver2);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    /*
                    auto _sender2{std::bind(&ServiceRegistryProcess::onSendSubscribing, this)};
                    _successful = mPoller->TryAddSender(&mOfferingUdpSocket, _sender2);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }
                    */
                }



                /**************************** poller functions  **********************************/

                void ServiceDiscoveryProcess::onReceiveOffering()
                {
                    std::cout << "------------ onReceiveOffering ------------------\n";

                    // define array to receive serialized SOMEIP/SD message
                    std::array<uint8_t, cBufferSize> _buffer;

                    std::string _ipAddress;
                    uint16_t _port;

                    // receive serialized SOMEIP/SD message in form of array not vector
                    ssize_t _receivedSize{mOfferingUdpSocket.Receive(_buffer, _ipAddress, _port)};
                    std::cout << "_port " << cOfferingPort << std::endl; 
                    if (_receivedSize > 0 && _port == cOfferingPort && _ipAddress == cNicIpAddress)
                    {
                        std::cout << "inside offeringPort\n";
                        const std::vector<uint8_t> cRequestPayload(
                            std::make_move_iterator(_buffer.begin()),
                            std::make_move_iterator(_buffer.begin() + _receivedSize));

                        // Create the received message from the received payload
                        sd::SomeIpSdMessage _receivedMessage{sd::SomeIpSdMessage::Deserialize(cRequestPayload)};
                        
                        _receivedMessage.print();

                        // call function that contain what to do with received message
                        handleOffering(std::move(_receivedMessage));
                        //handleFinding(std::move(_receivedMessage));

                        printRegistry();
                    }
                }

                void ServiceDiscoveryProcess::onReceiveFinding()
                {
                    std::cout << "------------ onReceiveFinding ----------------\n";

                    // define array to receive serialized SOMEIP/SD message
                    std::array<uint8_t, cBufferSize> _buffer;

                    std::string _ipAddress;
                    uint16_t _port;

                    ssize_t _receivedSize{mFindingUdpSocket.Receive(_buffer, _ipAddress, _port)};
                    std::cout << "_port " << cFindingPort << std::endl; 
                    if (_receivedSize > 0 && _port == cFindingPort && _ipAddress == cNicIpAddress)
                    {
                        std::cout << "inside findPort\n";
                        const std::vector<uint8_t> cRequestPayload(
                            std::make_move_iterator(_buffer.begin()),
                            std::make_move_iterator(_buffer.begin() + _receivedSize));

                        // Create the received message from the received payload
                        sd::SomeIpSdMessage _receivedMessage{sd::SomeIpSdMessage::Deserialize(cRequestPayload)};
                        
                        _receivedMessage.print();

                        // call function that contain what to do with received message
                        //handleOffering(std::move(_receivedMessage));
                        handleFinding(std::move(_receivedMessage));
                        //handleSubscribing(std::move(_receivedMessage));

                        printRegistry();
                    }
                }

                void ServiceDiscoveryProcess::onSendOfferingOrAck()
                {
                    while (!mSendingQueueForOffering.Empty())
                    {
                        std::cout << "-------- onSendOfferingOrAck -------------\n";

                        std::vector<uint8_t> _payload;
                        bool _dequeued{mSendingQueueForOffering.TryDequeue(_payload)};
                        if (_dequeued)
                        {
                            std::array<uint8_t, cBufferSize> _buffer;
                            std::copy_n(
                                std::make_move_iterator(_payload.begin()),
                                _payload.size(),
                                _buffer.begin());

                            mFindingUdpSocket.Send(_buffer, cMulticastGroup, cFindingPort);
                        }
                    }
                }

                /*
                void ServiceRegistryProcess::onSendSubscribing()
                {
                    while (!mSendingQueueForSubscring.Empty())
                    {
                        std::cout << "-------- onSendOfferingOrAck -------------\n";

                        std::vector<uint8_t> _payload;
                        bool _dequeued{mSendingQueueForSubscring.TryDequeue(_payload)};
                        if (_dequeued)
                        {
                            std::array<uint8_t, cBufferSize> _buffer;
                            std::copy_n(
                                std::make_move_iterator(_payload.begin()),
                                _payload.size(),
                                _buffer.begin());

                            mOfferingUdpSocket.Send(_buffer, cMulticastGroup, cOfferingPort);
                        }
                    }
                }
                */



                /***************************** main functions ****************************/

                // function take any someip/sd message 
                void ServiceDiscoveryProcess::handleOffering(SomeIpSdMessage &&message)
                {
                    uint32_t _ttl;
                    bool _successful = hasOfferingEntry(message, _ttl);
                    if (_successful)
                    {
                        std::string _ipAddress;
                        uint16_t _port;
                        uint16_t _serviceId;
                        uint16_t _instanceId;
                        _successful = ExtractInfoToStore(message, _ipAddress, _port,_serviceId,_instanceId);
                        if (_successful)
                        {
                            storeInfoOfServiceInstance({_serviceId,_instanceId},{_ipAddress,_port,protocol::tcp});
                        }
                    }
                }

                bool ServiceDiscoveryProcess::hasOfferingEntry(
                    const SomeIpSdMessage &message, uint32_t &ttl) const
                {
                    // Iterate over all the message entry to search for the first Service Offering entry
                    for (auto &_entry : message.Entries())
                    {
                        if (_entry->Type() == entry::EntryType::Offering)
                        {
                            if (auto _serviceEnty = dynamic_cast<entry::ServiceEntry *>(_entry.get()))
                            {
                                // Compare service 
                                bool _result = serviceRegistry.find( {_serviceEnty->ServiceId(),_serviceEnty->InstanceId()} ) == serviceRegistry.end();
                                if(_result)
                                {
                                    ttl = _serviceEnty->TTL();
                                }
                                return _result;
                            }
                        }
                    }
                    return false;
                }


                bool ServiceDiscoveryProcess::ExtractInfoToStore(
                    const SomeIpSdMessage &message,
                    std::string &ipAddress,
                    uint16_t &port,
                    uint16_t &_serviceId, uint16_t &_instanceId) const
                {
                    for (size_t i = 0; i < message.Entries().size(); ++i)
                    {
                        auto entry = message.Entries().at(i).get();

                        // Endpoints are end-up in the first options
                        for (size_t j = 0; entry->FirstOptions().size(); ++j)
                        {
                            auto option = entry->FirstOptions().at(j).get();

                            if (option->Type() == option::OptionType::IPv4Endpoint)
                            {
                                auto cEndpoint{
                                    dynamic_cast<const option::Ipv4EndpointOption *>(
                                        option)};

                                if (cEndpoint &&
                                    cEndpoint->L4Proto() == option::Layer4ProtocolType::Tcp)
                                {
                                    _serviceId = entry->ServiceId();
                                    _instanceId = entry->InstanceId();
                                    ipAddress = cEndpoint->IpAddress().ToString();
                                    port = cEndpoint->Port();

                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }



                bool ServiceDiscoveryProcess::hasFindingEntry(const SomeIpSdMessage &message,
                 transportInfo &info,
                 uint16_t &serviceId,
                 uint16_t &instanceId,
                 uint8_t &majorVersion,
                 uint32_t &minorVersion )
                {
                    // Iterate over all the message entry to search for the first Service Finding entry
                    for (auto &_entry : message.Entries())
                    {
                        if (_entry->Type() == entry::EntryType::Finding)
                        {
                            if (auto _serviceEnty = dynamic_cast<entry::ServiceEntry *>(_entry.get()))
                            {
                                registryKey k{_serviceEnty->ServiceId(),_serviceEnty->InstanceId()};
                                serviceId = _serviceEnty->ServiceId() ;
                                instanceId = _serviceEnty->InstanceId();
                                majorVersion = _serviceEnty->MajorVersion();
                                minorVersion = _serviceEnty->MinorVersion();
                                bool _result = isRegisted(k, info);
                                /*
                                if(!_result)
                                {
                                    requestData request= { _serviceEnty->ServiceId(),
                                                           _serviceEnty->InstanceId(),
                                                           _serviceEnty->MajorVersion(),
                                                           _serviceEnty->MinorVersion()
                                                         };
                                    mFindingRequestsToNotOfferedServices.push_back(request);
                                    
                                    mFindingRequestsConditionVariable.notify_one();
                                }
                                */
                                return _result;
                                /*
                                // Compare service ID, instance ID, major version, and minor version
                                bool _result =
                                    (_serviceEnty->ServiceId() == mServiceId) &&
                                    (_serviceEnty->InstanceId() == entry::Entry::cAnyInstanceId ||

                                     _serviceEnty->InstanceId() == mInstanceId) &&
                                    (_serviceEnty->MajorVersion() == entry::Entry::cAnyMajorVersion ||

                                     _serviceEnty->MajorVersion() == mMajorVersion) &&
                                    (_serviceEnty->MinorVersion() == entry::ServiceEntry::cAnyMinorVersion ||
                                     _serviceEnty->MinorVersion() == mMinorVersion);

                                return _result;
                                */
                            }
                        }
                    }

                    return false;
                }

                /*
                void ServiceRegistryProcess::run()
                {
                    if(mFindingRequestsToNotOfferedServices.size() != 0)
                    {
                        mFindingRequestsLock.lock();
                        
                        std::cout << "before mFindingRequestsConditionVariable.wait(mFindingRequestsLock);\n";
                        
                        mFindingRequestsConditionVariable.wait(mFindingRequestsLock);
                        
                        std::cout << "after mFindingRequestsConditionVariable.wait(mFindingRequestsLock);\n";
                        
                        for (const auto& request : mFindingRequestsToNotOfferedServices) 
                        {
                            myKey key = {request.serviceId, request.instanceId};
                            transportInfo info;
                            bool _result = isRegisted(key, info);
                            if(_result)
                            {
                                SomeIpSdMessage mOfferServiceMessage;

                                // prepare offering entry
                                auto _offerServiceEntry
                                { 
                                    entry::ServiceEntry::CreateOfferServiceEntry( request.serviceId,
                                                                                request.instanceId,
                                                                                request.majorVersion,
                                                                                request.minorVersion
                                                                                )
                                };
                                
                                
                                // prepare unicast endpoint option
                                auto _offerEndpointOption
                                {
                                    option::Ipv4EndpointOption::CreateUnitcastEndpoint( false,
                                                                                        info.ipAddress,
                                                                                        option::Layer4ProtocolType::Tcp,
                                                                                        info.port
                                                                                    )
                                };

                                // prepare SOMEIP/SD message contain offering entry to use at need
                                _offerServiceEntry->AddFirstOption(std::move(_offerEndpointOption));
                                mOfferServiceMessage.AddEntry(std::move(_offerServiceEntry));

                                SendOfferingOrAck(mOfferServiceMessage);
                                std::cout << "offer message from process is sent\n";
                            }
                        }
                        mFindingRequestsLock.unlock();
                    }
                }
                */

                void ServiceDiscoveryProcess::handleFinding(SomeIpSdMessage &&message)
                {
                    std::cout << "-----handleFinding is called----\n";
                    transportInfo info;
                    uint16_t serviceId;
                    uint16_t instanceId;
                    uint8_t majorVersion;
                    uint32_t minorVersion; 
                    bool _matches = hasFindingEntry(message,info,serviceId,instanceId,majorVersion,minorVersion);
                    // Enqueue the offer if the finding message matches the service
                    if (_matches)
                    {
                        SomeIpSdMessage mOfferServiceMessage;

                        // prepare offering entry
                        auto _offerServiceEntry
                        { 
                            entry::ServiceEntry::CreateOfferServiceEntry( serviceId,
                                                                        instanceId,
                                                                        majorVersion,
                                                                        minorVersion
                                                                        )
                        };
                        
                        
                        // prepare unicast endpoint option
                        auto _offerEndpointOption
                        {
                            option::Ipv4EndpointOption::CreateUnitcastEndpoint( false,
                                                                                info.ipAddress,
                                                                                option::Layer4ProtocolType::Tcp,
                                                                                info.port
                                                                            )
                        };

                        // prepare SOMEIP/SD message contain offering entry to use at need
                        _offerServiceEntry->AddFirstOption(std::move(_offerEndpointOption));
                        mOfferServiceMessage.AddEntry(std::move(_offerServiceEntry));

                        SendOfferingOrAck(mOfferServiceMessage);
                        std::cout << "offer message from process is sent\n";
                    }
                }

                bool ServiceDiscoveryProcess::hasSubscribingEntry(const SomeIpSdMessage &message)
                {
                    // Iterate over all the message entry to search for the first Event-group Subscribing entry
                    for (auto &_entry : message.Entries())
                    {
                        if (_entry->Type() == entry::EntryType::Subscribing)
                        {
                            return true;
                        }
                    }
                    return false;
                }

                /*
                void ServiceRegistryProcess::handleSubscribing(sd::SomeIpSdMessage &&message)
                {
                    bool _matches = hasSubscribingEntry(message);
                    // Enqueue the offer if the finding message matches the service
                    if (_matches)
                    {
                        std::cout << "has subscribing entry\n";
                        SendSubscring(message);
                        std::cout << "offer message from process is sent\n";
                    }
                }
                */


                bool ServiceDiscoveryProcess::isRegisted(const registryKey& k, transportInfo &info) const
                {
                    auto it = serviceRegistry.find(k);
                    if (it != serviceRegistry.end()) {
                        std::cout << "Data found:" << std::endl;
                        std::cout << "Service ID: " << it->first.serviceId << std::endl;
                        std::cout << "Instance ID: " << it->first.instanceId << std::endl;
                        std::cout << "IP Address: " << it->second.ipAddress << std::endl;
                        std::cout << "Port: " << it->second.port << std::endl;
                        std::cout << "Protocol: " << (it->second.proto == protocol::tcp ? "TCP" : "UDP") << std::endl;
                        // Return the data associated with the key
                        info = it->second;
                        return true;
                    } else {
                        std::cout << "Data not found" << std::endl;
                        return false;
                    }
                }

                void ServiceDiscoveryProcess::printRegistry()
                {
                    std::cout << "\n--------------- Service Registry -----------\n";
                    // Print the data in the map
                    for (const auto& entry : serviceRegistry) 
                    {
                        std::cout << entry.first.serviceId << ", " << entry.first.instanceId;
                        std::cout << "     " << entry.second.ipAddress << "  " 
                        << entry.second.port << "   " << static_cast<int>(entry.second.proto) << std::endl;
                    }
                   std::cout << "------------------------------------------------\n\n";
                }



                void ServiceDiscoveryProcess::SendOfferingOrAck(const SomeIpSdMessage &message)
                {
                    std::vector<uint8_t> _payload{message.Payload()};
                    mSendingQueueForOffering.TryEnqueue(std::move(_payload));
                }

                /*
                void ServiceRegistryProcess::SendSubscring(const SomeIpSdMessage &message)
                {
                    std::vector<uint8_t> _payload{message.Payload()};
                    mSendingQueueForSubscring.TryEnqueue(std::move(_payload));
                }
                */



                /****************************  deconstructor  ************************/

                ServiceDiscoveryProcess::~ServiceDiscoveryProcess()
                {
                    mPoller->TryRemoveSender(&mFindingUdpSocket);
                    mPoller->TryRemoveReceiver(&mFindingUdpSocket);
                    
                    //mPoller->TryRemoveSender(&mOfferingUdpSocket);
                    mPoller->TryRemoveReceiver(&mOfferingUdpSocket);                    
                }
            }
        }
    }
}