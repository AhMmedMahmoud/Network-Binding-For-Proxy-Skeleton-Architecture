// headers
#include <iostream>
#include "../someip/someipRpcMsg/someip_rpc_message.h"

// namespaces
using namespace ara::com::someip::rpc;
using namespace ara::com::someip;

// constants
const uint16_t cServiceId = 10;
const uint16_t cInstanceId = 3;
const uint16_t cMethodId = 4;
const uint16_t cInitialSessionId = 1;
const uint8_t cMajorVersion = 10;
const uint8_t cMinorVersion = 4;
const uint16_t mEventgroupId = 5;
const uint8_t cProtocolVersion = 20;
const uint16_t cInterfaceVersion = 2;
const uint16_t cClientId = 1;


int main()
{
    /*******************************************************************
    *  create object represent SOMEIP message using empty constructor  *
    *                                                                  *
    *  SomeIpRpcMessage()                                              *
    *******************************************************************/

    SomeIpRpcMessage message1;
    message1.print();
    std::cout << "------------------------------------------\n\n\n";



    /**************************************************************************
    *  create object represent SOMEIP message using parametrized constructor  *
    *                                                                         *
    *  SomeIpRpcMessage( uint32_t messageId,                                  *
    *                    uint16_t clientId,                                   *
    *                    uint16_t sessionId,                                  *
    *                    uint8_t protocolVersion,                             *
    *                    uint8_t interfaceVersion,                            *
    *                    const std::vector<uint8_t> &rpcPayload               *
    *                  );                                                     *
    *                                                                         *
    *   for Request messages                                                  *
    **************************************************************************/

    auto _messageId = static_cast<uint32_t>(cServiceId << 16);
    _messageId |= cMethodId;

    std::vector<uint8_t> rpcPayload = {10,20,30,40};
    SomeIpRpcMessage request( _messageId,
                              cClientId,
                              cInitialSessionId,
                              cProtocolVersion,
                              cInterfaceVersion,
                              rpcPayload);
    request.print();
    std::cout << "------------------------------------------\n\n\n";



    /**************************************************************************
    *  create object represent SOMEIP message using parametrized constructor  *
    *                                                                         *
    *  SomeIpRpcMessage( uint32_t messageId,                                  *
    *                    uint16_t clientId,                                   *
    *                    uint16_t sessionId,                                  *
    *                    uint8_t protocolVersion,                             *
    *                    uint8_t interfaceVersion,                            *
    *                    SomeIpReturnCode returnCode,                         *
    *                    const std::vector<uint8_t> &rpcPayload               *
    *                  );                                                     *
    *                                                                         *
    *   for Response messages or Error messages                               *
    **************************************************************************/

    SomeIpRpcMessage response(
                        request.MessageId(),
                        request.ClientId(),
                        request.SessionId(),
                        cProtocolVersion,
                        cInterfaceVersion,
                        SomeIpReturnCode::eOK,
                        rpcPayload);
    response.print();
    std::cout << "------------------------------------------\n\n\n";



    /*******************************************************************
    *  serialization of SOMEIP message using memeber function Payload  * 
    *                                                                  *
    *  Payload()                                                       *
    *******************************************************************/

    std::cout << "-----------------------Serialization of SOMEIP message----------------------\n";
    std::vector<uint8_t> responsePayload =  response.Payload();
    uint8_t counter = 0;
    for (uint8_t byte : responsePayload) 
    {
      std::cout << static_cast<int>(byte) << "\n";
      counter++;
      switch(counter)
      {              
        case 4: std::cout << "...Message ID...\n"; break;
        case 8: std::cout << "...Lenght...\n"; break;
        case 12: std::cout << "...Client ID & Session ID...\n"; break;
        case 16: std::cout << "...Protocol Vesion & Interface Version & Message type & Return Code...\n"; break;
      }
    }
    std::cout << "...Pdu...\n";
    std::cout << "----------------------------------------------------------------------------\n\n\n";


    return 0;
}