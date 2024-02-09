#include "rpcs_responser.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                /********************************* useful in child ******************************/

                RpcsResponser::RpcsResponser(uint8_t protocolVersion,
                                             uint8_t interfaceVersion) noexcept : mProtocolVersion{protocolVersion},
                                                                                  mInterfaceVersion{interfaceVersion}
                {}



                /*************************** useful in  TryInvokeHandler ************************/

                SomeIpReturnCode RpcsResponser::validate(const SomeIpRpcMessage &request) const
                {
                    if (request.MessageType() != SomeIpMessageType::Request)
                    {
                        std::cout << "message type is not request\n";
                        return SomeIpReturnCode::eWrongMessageType;
                    }

                    if (request.ProtocolVersion() != mProtocolVersion)
                    {
                        std::cout << "ProtocolVersion is same\n";
                        return SomeIpReturnCode::eWrongProtocolVersion;
                    }

                    if (request.InterfaceVersion() != mInterfaceVersion)
                    {
                        std::cout << "InterfaceVersion is same\n";
                        return SomeIpReturnCode::eWrongInterfaceVersion;
                    }

                    auto _serviceId{static_cast<uint16_t>(request.MessageId() >> 16)};
                    if (mServices.find(_serviceId) == mServices.end())
                    {
                        std::cout << "eUnknownService\n";
                        return SomeIpReturnCode::eUnknownService;
                    }

                    return SomeIpReturnCode::eOK;
                }



                /*
                    - make response message 
                        - has same message id of received message
                        - has same client id of received message
                        - has same session id of received message
                        
                        - has passed return code
                        - has passed rpcPayload
                    - fill its serialization into passed vector that called payload
                */
                void RpcsResponser::getResponsePayload(
                    const SomeIpRpcMessage &request,
                    SomeIpReturnCode returnCode,
                    const std::vector<uint8_t> &rpcPayload,
                    std::vector<uint8_t> &payload) const
                {
                    SomeIpRpcMessage _errorMessage(
                        request.MessageId(),
                        request.ClientId(),
                        request.SessionId(),
                        mProtocolVersion,
                        mInterfaceVersion,
                        returnCode,
                        rpcPayload);

                    payload = std::move(_errorMessage.Payload());
                }

                /*
                    special case when rpcPayload is set to null 
                */
                void RpcsResponser::getResponsePayload(
                    const SomeIpRpcMessage &request,
                    SomeIpReturnCode returnCode,
                    std::vector<uint8_t> &payload) const
                {
                    const std::vector<uint8_t> cRpcPayload;
                    getResponsePayload(request, returnCode, cRpcPayload, payload);
                }



                /**************************** for my child ***************************************/

                /*
                    - valiadate request message message
                    - if invalid request message then fill responsePayload with error message and return
                    - else check if message id of request message is registed or not
                        - if not registed then fill responsePayload with error message and return
                        - else call the registed handler for this message id
                        - call the method
                        - fill responsePayload with response message with result of requested method
                */

                bool RpcsResponser::TryInvokeHandler(
                    const std::vector<uint8_t> &requestPayload,
                    std::vector<uint8_t> &responsePayload) const
                {
                    try
                    {
                        const SomeIpRpcMessage _request{SomeIpRpcMessage::Deserialize(requestPayload)};

                        SomeIpReturnCode _returnCode{validate(_request)};
                        if (_returnCode != SomeIpReturnCode::eOK)
                        {
                            getResponsePayload(
                                _request,
                                _returnCode,
                                responsePayload);
                         
                            return true;
                        }

                        auto _itr{mHandlers.find(_request.MessageId())};
                        if (_itr != mHandlers.end())
                        {
                            std::vector<uint8_t> _rpcResponsePdu;
                            bool _handled{_itr->second(_request.RpcPayload(), _rpcResponsePdu)};

                            if (_handled)
                            {
                                getResponsePayload( _request,
                                                    SomeIpReturnCode::eOK,
                                                    _rpcResponsePdu,
                                                    responsePayload
                                                  );  
                            }
                            else
                            {
                                getResponsePayload( _request,
                                                    SomeIpReturnCode::eNotOk,
                                                    _rpcResponsePdu,
                                                    responsePayload
                                                  );
                            }
                        }
                        else
                        {
                            getResponsePayload(
                                _request,
                                SomeIpReturnCode::eUnknownMethod,
                                responsePayload);
                        }

                        return true;
                    }
                    catch (std::out_of_range)
                    {                 
                        std::cout << "77777777777777777777777777777\n";
                        return false;
                    }
                }



                /**************************** fundemental functions *************************/

                void RpcsResponser::SetHandler(uint16_t serviceId, uint16_t methodId, HandlerType handler)
                {
                    auto _messageId{static_cast<uint32_t>(serviceId << 16)};
                    _messageId |= methodId;
                    mHandlers[_messageId] = handler;

                    mServices.insert(serviceId);

                    std::cout << "handler for method whose id is " 
                              << methodId 
                              << " and belong to service whose id is " 
                              << serviceId 
                              << " is registed into hash table" << "\n";
                }
            }
        }
    }
}