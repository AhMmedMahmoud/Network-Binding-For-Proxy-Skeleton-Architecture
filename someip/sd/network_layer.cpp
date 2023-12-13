#include <algorithm>
#include "network_layer.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                const size_t NetworkLayer::cBufferSize{256};
                const std::string NetworkLayer::cAnyIpAddress("0.0.0.0");



                /******************************* constructors  ******************************/

                NetworkLayer::NetworkLayer(
                    AsyncBsdSocketLib::Poller *poller,
                    std::string nicIpAddress,
                    std::string multicastGroup,
                    uint16_t port) : cNicIpAddress{nicIpAddress},
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

                    auto _receiver{std::bind(&NetworkLayer::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mUdpSocket, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    auto _sender{std::bind(&NetworkLayer::onSend, this)};
                    _successful = mPoller->TryAddSender(&mUdpSocket, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }
                }



                /**************************** poller functions  **********************************/

                void NetworkLayer::onReceive()
                {
                    std::cout << "------------ onReceive------------------\n";

                    std::array<uint8_t, cBufferSize> _buffer;
                    std::string _ipAddress;
                    uint16_t _port;
                    ssize_t _receivedSize{mUdpSocket.Receive(_buffer, _ipAddress, _port)};

                    if (_receivedSize > 0 && _port == cPort && _ipAddress == cNicIpAddress)
                    {
                        const std::vector<uint8_t> cRequestPayload(
                            std::make_move_iterator(_buffer.begin()),
                            std::make_move_iterator(_buffer.begin() + _receivedSize));

                        FireReceiverCallbacks(cRequestPayload);
                    }
                }

                void NetworkLayer::onSend()
                {
                    while (!mSendingQueue.Empty())
                    {
                        std::cout << "------------ onSend------------------\n";

                        std::vector<uint8_t> _payload;
                        bool _dequeued{mSendingQueue.TryDequeue(_payload)};
                        if (_dequeued)
                        {
                            std::array<uint8_t, cBufferSize> _buffer;
                            std::copy_n(
                                std::make_move_iterator(_payload.begin()),
                                _payload.size(),
                                _buffer.begin());

                            mUdpSocket.Send(_buffer, cMulticastGroup, cPort);
                        }
                    }
                }



                /*************** override virtual functions inherited from parent*************/

                void NetworkLayer::Send(const SomeIpSdMessage &message)
                {
                    std::vector<uint8_t> _payload{message.Payload()};
                    mSendingQueue.TryEnqueue(std::move(_payload));
                }



                /**************************** override deconstructor  ************************/

                NetworkLayer::~NetworkLayer()
                {
                    mPoller->TryRemoveSender(&mUdpSocket);
                    mPoller->TryRemoveReceiver(&mUdpSocket);
                }
            }
        }
    }
}