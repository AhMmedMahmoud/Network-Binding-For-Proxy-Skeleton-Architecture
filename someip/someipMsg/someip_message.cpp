#include "./someip_message.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            /****** it helps in constructors that will be inherited to mychild *******/

            SomeIpMessage::SomeIpMessage(uint32_t messageId,
                                         uint16_t clientId,
                                         uint16_t sessionId,
                                         uint8_t protocolVersion,
                                         uint8_t interfaceVersion,
                                         SomeIpMessageType messageType,
                                         SomeIpReturnCode returnCode) noexcept : mMessageId{messageId},
                                                                                 mClientId{clientId},
                                                                                 mSessionId{sessionId},
                                                                                 mProtocolVersion{protocolVersion},
                                                                                 mInterfaceVersion{interfaceVersion},
                                                                                 mMessageType{messageType},
                                                                                 mReturnCode{returnCode}

            {}



            /***************************** for my child ******************************/

            SomeIpMessage::SomeIpMessage(uint32_t messageId,
                                         uint16_t clientId,
                                         uint8_t protocolVersion,
                                         uint8_t interfaceVersion,
                                         SomeIpMessageType messageType,
                                         uint16_t sessionId) : SomeIpMessage(messageId,
                                                                             clientId,
                                                                             sessionId,
                                                                             protocolVersion,
                                                                             interfaceVersion,
                                                                             messageType,
                                                                             SomeIpReturnCode::eOK)
            {
                if ((messageType != SomeIpMessageType::Request) &&
                    (messageType != SomeIpMessageType::Notification))
                {
                    // E2E is not supported yet.
                    throw std::invalid_argument("Invalid message type.");
                }
            }

            SomeIpMessage::SomeIpMessage(uint32_t messageId,
                                         uint16_t clientId,
                                         uint8_t protocolVersion,
                                         uint8_t interfaceVersion,
                                         SomeIpMessageType messageType,
                                         SomeIpReturnCode returnCode,
                                         uint16_t sessionId) : SomeIpMessage(messageId,
                                                                             clientId,
                                                                             sessionId,
                                                                             protocolVersion,
                                                                             interfaceVersion,
                                                                             messageType,
                                                                             returnCode)
            {
                if ((messageType == SomeIpMessageType::Request) ||
                    (messageType == SomeIpMessageType::Notification))
                {
                    // E2E is not supported yet.
                    throw std::invalid_argument("Invalid message type.");
                }
                else if ((messageType == SomeIpMessageType::Error) &&
                         (returnCode == SomeIpReturnCode::eOK))
                {
                    // Error message cannot have OK return code.
                    throw std::invalid_argument("Invalid return code.");
                }
            }



            /***************** move constructor and move assigment operator *****************/

            SomeIpMessage::SomeIpMessage(SomeIpMessage &&other) noexcept : mMessageId{other.mMessageId},
                                                                           mClientId{other.mClientId},
                                                                           mSessionId{other.mSessionId},
                                                                           mProtocolVersion{other.mProtocolVersion},
                                                                           mInterfaceVersion{other.mInterfaceVersion},
                                                                           mMessageType{other.mMessageType},
                                                                           mReturnCode{other.mReturnCode}
            {
            }

            SomeIpMessage &SomeIpMessage::operator=(SomeIpMessage &&other)
            {
                mMessageId = other.mMessageId;
                mClientId = other.mClientId;
                mSessionId = other.mSessionId;
                mProtocolVersion = other.mProtocolVersion;
                mInterfaceVersion = other.mInterfaceVersion;
                mMessageType = other.mMessageType;
                mReturnCode = other.mReturnCode;

                return *this;
            }



            /********************************** getters *********************************/

            uint32_t SomeIpMessage::MessageId() const noexcept
            {
                return mMessageId;
            }

            uint16_t SomeIpMessage::ClientId() const noexcept
            {
                return mClientId;
            }

            uint16_t SomeIpMessage::SessionId() const noexcept
            {
                return mSessionId;
            }

            uint8_t SomeIpMessage::ProtocolVersion() const noexcept
            {
                return mProtocolVersion;
            }

            uint8_t SomeIpMessage::InterfaceVersion() const noexcept
            {
                return mInterfaceVersion;
            }

            SomeIpMessageType SomeIpMessage::MessageType() const noexcept
            {
                return mMessageType;
            }

            SomeIpReturnCode SomeIpMessage::ReturnCode() const noexcept
            {
                return mReturnCode;
            }



            /********************************** non-pure virtual functions ***********************/
            
            void SomeIpMessage::SetSessionId(uint16_t sessionId)
            {
                mSessionId = sessionId;
            }

            bool SomeIpMessage::IncrementSessionId() noexcept
            {
                if (mSessionId == std::numeric_limits<uint16_t>::max())
                {
                    mSessionId = 1;
                    return true;
                }
                else
                {
                    mSessionId++;
                    return false;
                }
            }

            std::vector<uint8_t> SomeIpMessage::Payload() const
            {
                std::vector<uint8_t> _result;
                /*
                _result is vector of uint8_t
                
                uint32_t messageId
                uint32_t Lenght
                uint16_t clientId
                uint16_t sessionId

                uint8_t protocolVersion
                uint8_t interfaceVersion
                uint8_t messageType
                uint8_t returnCode
                */ 
                helper::Inject(_result, MessageId());
                helper::Inject(_result, Length());
                helper::Inject(_result, ClientId());
                helper::Inject(_result, SessionId());

                _result.push_back(ProtocolVersion());
                _result.push_back(InterfaceVersion());
                _result.push_back(static_cast<uint8_t>(MessageType()));
                _result.push_back(static_cast<uint8_t>(ReturnCode()));

                return _result;
            }

            void SomeIpMessage::print() const noexcept
            {
                if(MessageId() == 0xFFFF8100)
                {
                    std::cout << "message id: " << MessageId() << std::endl;
                }
                else
                {
                    std::cout << "sevice id: " << (MessageId() >> 16) << std::endl;

                    if( ((MessageId() << 16) >> 16) & 1)
                        std::cout << "event id: "<< ((MessageId() << 16) >> 16) << std::endl;
                    else
                        std::cout << "method id: "<< ((MessageId() << 16) >> 16) << std::endl;
                }
                
                std::cout << "lenght: " << Length() << std::endl;
                
                if(MessageId() == 0xFFFF8100)
                {
                    std::cout << "request id: " << (((uint32_t)ClientId()<<16) | ((uint32_t)SessionId())) << std::endl;
                }
                else
                {
                    std::cout << "client id: " << ClientId() << std::endl;
                    std::cout << "session id: " << SessionId() << std::endl;
                }
                std::cout << "protocol version: " << static_cast<int>(ProtocolVersion()) << std::endl;
                std::cout << "interface version: " << static_cast<int>(InterfaceVersion()) << std::endl;

                switch (MessageType())
                {
                    case SomeIpMessageType::Request:
                    std::cout << "message type: " << "Request" << std::endl;
                    break;
                    
                    case SomeIpMessageType::RequestNoReturn:
                    std::cout << "message type: " << "RequestNoReturn" << std::endl;                   
                    break;

                    case SomeIpMessageType::Response:
                    std::cout << "message type: " << "Response" << std::endl;                 
                    break;

                    case SomeIpMessageType::Error:
                    std::cout << "message type: " << "Error" << std::endl;                
                    break;

                    case SomeIpMessageType::Notification:
                    std::cout << "message type: " << "Notification" << std::endl;             
                    break;

                    default:
                    std::cout << "message type: " << static_cast<int>(MessageType()) << std::endl;                 
                    break;
                }

                std::cout << "return code: " << static_cast<int>(ReturnCode()) << std::endl;
            }


            /********************************** static functions ***********************/

            void SomeIpMessage::Deserialize(SomeIpMessage *message, const std::vector<uint8_t> &payload)
            {
                // Set the offset at the beginning of the payload
                std::size_t _offset = 0;

                message->mMessageId = helper::ExtractInteger(payload, _offset);

                // Apply the message length field offset
                _offset += 4;

                message->mClientId = helper::ExtractShort(payload, _offset);
                message->mSessionId = helper::ExtractShort(payload, _offset);
                message->mProtocolVersion = payload[_offset++];
                message->mInterfaceVersion = payload[_offset++];
                message->mMessageType = static_cast<SomeIpMessageType>(payload[_offset++]);
                message->mReturnCode = static_cast<SomeIpReturnCode>(payload[_offset]);
            }
        }
    }
}