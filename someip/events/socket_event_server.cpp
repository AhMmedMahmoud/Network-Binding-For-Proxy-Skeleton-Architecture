#include "socket_event_server.h"
#include <iostream>
#include <algorithm>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                const std::string SockeKEventServer::cAnyIpAddress("0.0.0.0");

                /******************************* constructors  ******************************/

                SockeKEventServer::SockeKEventServer(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint16_t eventgroupId, 
                    AsyncBsdSocketLib::Poller *poller,
                    std::string nicIpAddress,
                    std::string multicastGroup,
                    uint16_t port,
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion) : EventServer(serviceId,instanceId,majorVersion,eventgroupId,protocolVersion,interfaceVersion),
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

                    auto _receiver{std::bind(&SockeKEventServer::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mUdpSocket, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    auto _sender{std::bind(&SockeKEventServer::onSend, this)};
                    _successful = mPoller->TryAddSender(&mUdpSocket, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }
                }



                /**************************** poller functions  **********************************/

                void SockeKEventServer::onReceive()
                {
                    //std::cout << "------------------------- onReceive ------------------\n";

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
                        rpc::SomeIpRpcMessage _receivedMessage{rpc::SomeIpRpcMessage::Deserialize(cRequestPayload)};
                        
                        // call function that contain what to do with received message
                        
                        InvokeEventHandler(_receivedMessage);
                    }
                }

                void SockeKEventServer::onSend()
                {
                    while (!mSendingQueue.Empty())
                    {
                        //std::cout << "--------------------------- onSend -------------------\n";
                        std::vector<uint8_t> _payload;
                        bool _dequeued{mSendingQueue.TryDequeue(_payload)};
                        if (_dequeued)
                        {       
                            // for printing
                            std::cout << "------------------------------------------------\n";
                            std::cout << ".....sent message..... \n";
                            rpc::SomeIpRpcMessage toSend  = rpc::SomeIpRpcMessage::Deserialize(_payload);
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

                void SockeKEventServer::Send(const std::vector<uint8_t> &payload)
                {
                    mSendingQueue.TryEnqueue(payload);
                }



                /**************************** override deconstructor  ************************/

                SockeKEventServer::~SockeKEventServer()
                {
                    mPoller->TryRemoveSender(&mUdpSocket);
                    mPoller->TryRemoveReceiver(&mUdpSocket);
                }
            }
        }
    }
}