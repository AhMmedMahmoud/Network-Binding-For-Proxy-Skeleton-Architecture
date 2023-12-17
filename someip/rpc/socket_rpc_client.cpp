
#include "socket_rpc_client.h"
#include <iostream>
#include <algorithm>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                const size_t SocketRpcClient::cBufferSize;
                
                /**************************** constructor  ****************************************/

                SocketRpcClient::SocketRpcClient(
                    AsyncBsdSocketLib::Poller *poller,
                    std::string ipAddress,
                    uint16_t port,
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion) : RpcsRequester(protocolVersion, interfaceVersion),
                                                mPoller{poller},
                                                mClient{AsyncBsdSocketLib::TcpClient(ipAddress, port)}
                {
                    bool _successful{mClient.TrySetup()};
                    if (!_successful)
                    {
                        throw std::runtime_error("TCP client socket setup failed.");
                    }

                    _successful = mClient.TryConnect();
                    if (!_successful)
                    {
                        throw std::runtime_error("TCP client socket connection failed.");
                    }

                    auto _sender{std::bind(&SocketRpcClient::onSend, this)};
                    _successful = mPoller->TryAddSender(&mClient, _sender);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding TCP client socket sender failed.");
                    }

                    auto _receiver{std::bind(&SocketRpcClient::onReceive, this)};
                    _successful = mPoller->TryAddReceiver(&mClient, _receiver);
                    if (!_successful)
                    {
                        throw std::runtime_error("Adding TCP client socket receiver failed.");
                    }
                }



                /************************** poller functions  ************************/

                void SocketRpcClient::onSend()
                {
                    while (!mSendingQueue.Empty())
                    {
                        try
                        {  
                            std::cout << "end end end\n";
                            std::vector<uint8_t> _payload;
                            bool _dequeued{mSendingQueue.TryDequeue(_payload)};
                            if (_dequeued)
                            {
                                std::array<uint8_t, cBufferSize> _buffer;
                                std::copy_n(
                                    std::make_move_iterator(_payload.begin()),
                                    _payload.size(),
                                    _buffer.begin());

                                mClient.Send(_buffer);
                            }
                            else
                            {
                                std::cout << "cannot dequeue the message\n";
                            }
                        }catch (const std::bad_function_call& ex)
                        {
                            std::cerr << "gggggggggggg" << ex.what() << std::endl;
                            // Handle the exception as needed
                        }
                        catch(const std::exception& e)
                        {
                            std::cerr << "ahmed ahmed ahmed\n" << '\n';
                        }
                        std::cout << "end end end\n";
                    }
                }

                void SocketRpcClient::onReceive()
                {
                    // define array to receive serialized SOMEIP message
                    std::array<uint8_t, cBufferSize> _buffer;
                    
                    // receive serialized SOMEIP message in form of array not vector
                    ssize_t _receivedSize{mClient.Receive(_buffer)};
                    if (_receivedSize > 0)
                    {
                        // convert serialized SOMEIP message from array into vector
                        const std::vector<uint8_t> cPayload(
                            std::make_move_iterator(_buffer.begin()),
                            std::make_move_iterator(_buffer.begin() + _receivedSize));

                        // call function that contain what to do with received message
                        InvokeHandler(cPayload);
                    }
                }

                
                
                /******************** function that parent need *****************/

                void SocketRpcClient::Send(const std::vector<uint8_t> &payload)
                {
                    //mSendingQueue.TryEnqueue(payload);

                    try
                    {
                        std::cout << "before putting data in mSendingQueue\n";
                        bool result = mSendingQueue.TryEnqueue(payload);
                        if(!result)
                        {
                            std::cout << "failed to enqueue\n";
                        }
                        std::cout << "after putting data in mSendingQueue\n";
                    }
                    catch (const std::bad_function_call& ex)
                    {
                        std::cerr << "ssssssssss" << ex.what() << std::endl;
                        // Handle the exception as needed
                    }
                    catch (const std::exception& e) {
                        std::cout << "333333333333333\n";
                    }
                }



                /**************************** override deconstructor  ************************/

                SocketRpcClient::~SocketRpcClient()
                {
                    mPoller->TryRemoveReceiver(&mClient);
                    mPoller->TryRemoveSender(&mClient);
                }
            }
        }
    }
}