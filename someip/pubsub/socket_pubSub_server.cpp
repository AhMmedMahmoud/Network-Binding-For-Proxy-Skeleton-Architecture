#include <algorithm>
#include "./socket_pubSub_server.h"
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
                const std::string SockeKPubSubServer::cAnyIpAddress("0.0.0.0");

                /******************************* constructors  ******************************/

                SockeKPubSubServer::SockeKPubSubServer(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint16_t eventgroupId, 
                    AsyncBsdSocketLib::Poller *poller,
                    std::string nicIpAddress,
                    std::string multicastGroup,
                    uint16_t port) : EventProvider(serviceId,instanceId,majorVersion,eventgroupId),
                                     cNicIpAddress{nicIpAddress},
                                     cMulticastGroup{multicastGroup},
                                     cPort{port},
                                     mPoller{poller},
                                     mUdpSocket(cAnyIpAddress, port, nicIpAddress, multicastGroup)
                {
                    bool _successful{mUdpSocket.TrySetup()};
                    if (!_successful)
                    {
                        throw std::runtime_error("UDP socket setup failed.");
                    }

                    auto _receiver{std::bind(&SockeKPubSubServer::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mUdpSocket, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    auto _sender{std::bind(&SockeKPubSubServer::onSend, this)};
                    _successful = mPoller->TryAddSender(&mUdpSocket, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }
                }



                /**************************** poller functions  **********************************/

                void SockeKPubSubServer::onReceive()
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

                void SockeKPubSubServer::onSend()
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



                /******************** function that parent need *****************/

                void SockeKPubSubServer::Send(const SomeIpSdMessage &message)
                {
                    std::vector<uint8_t> _payload{message.Payload()};
                    mSendingQueue.TryEnqueue(std::move(_payload));
                }



                /**************************** override deconstructor  ************************/

                SockeKPubSubServer::~SockeKPubSubServer()
                {
                    mPoller->TryRemoveSender(&mUdpSocket);
                    mPoller->TryRemoveReceiver(&mUdpSocket);
                }
            }
        }
    }
}