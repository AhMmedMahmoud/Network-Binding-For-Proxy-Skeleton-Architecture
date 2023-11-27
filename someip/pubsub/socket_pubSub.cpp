#include <algorithm>
#include "./socket_pubSub.h"
#include <iostream>
#include "../../entry/eventgroup_entry.h"


namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                const size_t SockeKPubSub::cBufferSize{256};
                const std::string SockeKPubSub::cAnyIpAddress("0.0.0.0");



                /******************************* constructors  ******************************/

                SockeKPubSub::SockeKPubSub(
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

                    auto _receiver{std::bind(&SockeKPubSub::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mUdpSocket, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket receiver failed.");
                    }

                    auto _sender{std::bind(&SockeKPubSub::onSend, this)};
                    _successful = mPoller->TryAddSender(&mUdpSocket, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding UDP socket sender failed.");
                    }
                }



                /**************************** backend functions  **********************************/

                void SockeKPubSub::onReceive()
                {
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

                void SockeKPubSub::onSend()
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



                /*************** override virtual functions inherited from parent*************/

                void SockeKPubSub::Send(const SomeIpSdMessage &message)
                {
                    std::vector<uint8_t> _payload{message.Payload()};
                    mSendingQueue.TryEnqueue(std::move(_payload));
                }



                /**************************** override deconstructor  ************************/

                SockeKPubSub::~SockeKPubSub()
                {
                    mPoller->TryRemoveSender(&mUdpSocket);
                    mPoller->TryRemoveReceiver(&mUdpSocket);
                }
            }
        }
    }
}