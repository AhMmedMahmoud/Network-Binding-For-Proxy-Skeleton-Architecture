#include <algorithm>
#include "./socket_rpc_server.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                const size_t SocketRpcServer::cBufferSize;
                
                /**************************** constructor  ****************************************/

                SocketRpcServer::SocketRpcServer(
                    AsyncBsdSocketLib::Poller *poller,
                    std::string ipAddress,
                    uint16_t port,
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion) : RpcServer(protocolVersion, interfaceVersion),
                                                mPoller{poller},
                                                mServer{AsyncBsdSocketLib::TcpListener(ipAddress, port)}
                {
                    bool _successful{mServer.TrySetup()};
                    if (!_successful)
                    {
                        throw std::runtime_error("TCP server socket setup failed.");
                    }

                    auto _listener{std::bind(&SocketRpcServer::onAccept, this)};
                    _successful = mPoller->TryAddListener(&mServer, _listener);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding TCP server socket listener failed.");
                    }
                }



                /**************************** backend functions  **********************************/

                void SocketRpcServer::onAccept()
                {
                    bool _successful{mServer.TryAccept()};

                    if (!_successful)
                    {
                        throw std::runtime_error("Accepting RPC client TCP connection failed.");
                    }

                    _successful = mServer.TryMakeConnectionNonblock();
                    if (!_successful)
                    {
                        throw std::runtime_error("Making non-blocking TCP connection failed.");
                    }

                    auto _receiver{std::bind(&SocketRpcServer::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mServer, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding TCP server socket receiver failed.");
                    }

                    auto _sender{std::bind(&SocketRpcServer::onSend, this)};
                    _successful = mPoller->TryAddSender(&mServer, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error(
                            "Adding TCP server socket sender failed.");
                    }
                }

                void SocketRpcServer::onReceive()
                {
                    // define array to receive serialized SOMEIP message
                    std::array<uint8_t, cBufferSize> _buffer;

                    // receive serialized SOMEIP message in form of array not vector
                    ssize_t _receivedSize{mServer.Receive(_buffer)};
                    if (_receivedSize > 0)
                    {
                        // convert serialized SOMEIP message from array into vector
                        const std::vector<uint8_t> cRequestPayload(std::make_move_iterator(_buffer.begin()), std::make_move_iterator(_buffer.begin() + _receivedSize));
                        
                        /*
                        SomeIpRpcMessage received  = SomeIpRpcMessage::Deserialize(cRequestPayload);
                        std::cout << "\n------------------------------------------------\n";
                        std::cout << ".....received message..... \n";
                        std::cout << "sevice id: " << (received.MessageId() >> 16) << std::endl;
                        std::cout << "method id: "<< ((received.MessageId() << 16) >> 16) << std::endl;
                        std::cout << "lenght: " << received.Length() << std::endl;
                        std::cout << "client id: " << received.ClientId() << std::endl;
                        std::cout << "session id: " << received.SessionId() << std::endl;
                        std::cout << "protocol version: " << 12 << std::endl;
                        std::cout << "interface version: " << 1 << std::endl;
                        std::cout << "message type: " << static_cast<int>(received.MessageType()) << std::endl;
                        std::cout << "return code: " << static_cast<int>(received.ReturnCode()) << std::endl;
                        std::cout << "--------------------------------------------------\n";
                        */

                        // define vector that will be filled with result of method that i provide
                        std::vector<uint8_t> _responsePayload;
                        
                        
                        // Try to invoke corresponding request handler at a message reception
                        bool _handled{TryInvokeHandler(cRequestPayload, _responsePayload)};
                        if (_handled)
                        {
                            /*
                            SomeIpRpcMessage toSend  = SomeIpRpcMessage::Deserialize(_responsePayload);
                            std::cout << "\n------------------------------------------------\n";
                            std::cout << ".....sending message..... \n";
                            std::cout << "sevice id: " << (toSend.MessageId() >> 16) << std::endl;
                            std::cout << "method id: "<< ((toSend.MessageId() << 16) >> 16) << std::endl;
                            std::cout << "lenght: " << toSend.Length() << std::endl;
                            std::cout << "client id: " << toSend.ClientId() << std::endl;
                            std::cout << "session id: " << toSend.SessionId() << std::endl;
                            std::cout << "protocol version: " << 12 << std::endl;
                            std::cout << "interface version: " << 1 << std::endl;
                            std::cout << "message type: " << static_cast<int>(toSend.MessageType()) << std::endl;
                            std::cout << "return code: " << static_cast<int>(toSend.ReturnCode()) << std::endl;
                            std::cout << "--------------------------------------------------\n";
                            */

                            // put vector that holds the result of method that i provide
                            mSendingQueue.TryEnqueue(std::move(_responsePayload));
                        }
                    }
                }

                void SocketRpcServer::onSend()
                {
                    while (!mSendingQueue.Empty())
                    {
                        std::vector<uint8_t> _payload;
                        bool _dequeued{mSendingQueue.TryDequeue(_payload)};
                        if (_dequeued)
                        {
                            std::array<uint8_t, cBufferSize> _buffer;
                            std::copy_n(
                                std::make_move_iterator(_payload.begin()),
                                _payload.size(),
                                _buffer.begin());

                            mServer.Send(_buffer);
                        }
                    }
                }



                /**************************** override deconstructor inherited from parent *********/

                SocketRpcServer::~SocketRpcServer()
                {
                    mPoller->TryRemoveSender(&mServer);
                    mPoller->TryRemoveReceiver(&mServer);
                    mPoller->TryRemoveListener(&mServer);
                }
            }
        }
    }
}