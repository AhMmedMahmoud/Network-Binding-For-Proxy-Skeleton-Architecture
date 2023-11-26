#include "./rpc_client.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                /*********************** useful for constructor of my child *****************/

                RpcClient::RpcClient(
                    uint8_t protocolVersion,
                    uint8_t interfaceVersion) noexcept : mProtocolVersion{protocolVersion},
                                                         mInterfaceVersion{interfaceVersion}
                {}



                /**************************** useful for my child *************************/

                /*
                - take serialized SOMEIP received that represents response message
                - check if messageID for this message is one of my messages ids
                    - invoke registed handler that takes this message and processes it
                */
                void RpcClient::InvokeHandler(const std::vector<uint8_t> &payload) const
                {
                    try
                    {
                        const SomeIpRpcMessage _message{SomeIpRpcMessage::Deserialize(payload)};

                        auto _itr{mHandlers.find(_message.MessageId())};
                        if (_itr != mHandlers.end())
                        {
                            _itr->second(_message);
                        }
                    }
                    catch (std::out_of_range)
                    {
                        // Ignore the corrupted RPC server response
                    }
                }



                /**************************** fundemental functions *************************/

                void RpcClient::SetHandler(
                    uint16_t serviceId, uint16_t methodId, HandlerType handler)
                {
                    auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                    _messageId |= methodId;
                    mHandlers[_messageId] = handler;
                }

                void RpcClient::Send(
                    uint16_t serviceId,
                    uint16_t methodId,
                    uint16_t clientId,
                    const std::vector<uint8_t> &rpcPayload)
                {
                    const uint16_t cInitialSessionId{1};

                    auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                    _messageId |= methodId;

                    auto _itr{mSessionIds.find(_messageId)};
                    uint16_t _sessionId{(_itr != mSessionIds.end()) ? _itr->second : cInitialSessionId};

                    SomeIpRpcMessage _request(
                        _messageId,
                        clientId,
                        _sessionId,
                        mProtocolVersion,
                        mInterfaceVersion,
                        rpcPayload);


                    /*
                    std::cout << "\n------------------------------------------------\n";
                    std::cout << ".....requesting message..... \n";
                    std::cout << "sevice id: " << (_request.MessageId() >> 16) << std::endl;
                    std::cout << "method id: "<< ((_request.MessageId() << 16) >> 16) << std::endl;
                    std::cout << "lenght: " << _request.Length() << std::endl;
                    std::cout << "client id: " << _request.ClientId() << std::endl;
                    std::cout << "session id: " << _request.SessionId() << std::endl;
                    std::cout << "protocol version: " << 12 << std::endl;
                    std::cout << "interface version: " << 1 << std::endl;
                    std::cout << "message type: " << static_cast<int>(_request.MessageType()) << std::endl;
                    std::cout << "return code: " << static_cast<int>(_request.ReturnCode()) << std::endl;
                    std::cout << "------------------------------------------------\n\n";
                    */


                    Send(_request.Payload());

                    // Increment the session ID for that specific message ID for the next send
                    _request.IncrementSessionId();
                    mSessionIds[_messageId] = _request.SessionId();
                }
            }
        }
    }
}