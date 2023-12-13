#include "./rpcs_requester.h"
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

                RpcsRequester::RpcsRequester(
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
                void RpcsRequester::InvokeHandler(const std::vector<uint8_t> &payload) const
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

                void RpcsRequester::SetHandler(
                    uint16_t serviceId, uint16_t methodId, HandlerType handler)
                {
                    auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                    _messageId |= methodId;
                    mHandlers[_messageId] = handler;
                }

                void RpcsRequester::Request(
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

                    // create message that represents a request
                    SomeIpRpcMessage _request(_messageId, clientId, _sessionId, mProtocolVersion, mInterfaceVersion, rpcPayload);

                    // for printing
                    std::cout << "\n------------------------------------------------\n";
                    std::cout << ".....sent message..... \n";
                    _request.print();
                    std::cout << "--------------------------------------------------\n";

                    Send(_request.Payload());

                    // Increment the session ID for that specific message ID for the next send
                    _request.IncrementSessionId();
                    mSessionIds[_messageId] = _request.SessionId();
                }
            }
        }
    }
}