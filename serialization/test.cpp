#include <iostream>
#include "serializer.hpp"
#include "deserializer.hpp"

enum class message_type_e : uint8_t {
    MT_REQUEST = 0x00,
    MT_REQUEST_NO_RETURN = 0x01,
    MT_NOTIFICATION = 0x02,
    MT_REQUEST_ACK = 0x40,
    MT_REQUEST_NO_RETURN_ACK = 0x41,
    MT_NOTIFICATION_ACK = 0x42,
    MT_RESPONSE = 0x80,
    MT_ERROR = 0x81,
    MT_RESPONSE_ACK = 0xC0,
    MT_ERROR_ACK = 0xC1,
    MT_UNKNOWN = 0xFF
};


enum class return_code_e : uint8_t {
    E_OK = 0x00,
    E_NOT_OK = 0x01,
    E_UNKNOWN_SERVICE = 0x02,
    E_UNKNOWN_METHOD = 0x03,
    E_NOT_READY = 0x04,
    E_NOT_REACHABLE = 0x05,
    E_TIMEOUT = 0x06,
    E_WRONG_PROTOCOL_VERSION = 0x07,
    E_WRONG_INTERFACE_VERSION = 0x08,
    E_MALFORMED_MESSAGE = 0x09,
    E_WRONG_MESSAGE_TYPE = 0x0A,
    E_UNKNOWN = 0xFF
};



class message: public serializable 
{
private:
    uint16_t service_;
    uint32_t method_;
    uint32_t length_;
    uint16_t client_;
    uint16_t session_;
    uint8_t protocol_version_;
    uint8_t interface_version_;
    message_type_e type_;
    return_code_e code_;

public:
    message( uint16_t service_,
             uint32_t method_,
             uint32_t length_,
             uint16_t client_,
             uint16_t session_,
             uint8_t protocol_version_,
             uint8_t interface_version_,
             message_type_e type_,
             return_code_e code_
    )
    {
        this->service_ = service_;
        this->method_ = method_;
        this->length_ = length_;
        this->client_ = client_;
        this->session_ = session_;
        this->protocol_version_ = protocol_version_;
        this->interface_version_ = interface_version_;
        this->type_ = type_;
        this->code_ = code_;
    }


    bool serialize(serializer *_to) const 
    {
        return (0 != _to
            && _to->serialize(service_)
            && _to->serialize(method_)
            && _to->serialize(length_)
            && _to->serialize(client_)
            && _to->serialize(session_)
            && _to->serialize(protocol_version_)
            && _to->serialize(interface_version_)
            && _to->serialize(static_cast<uint8_t>(type_))
            && _to->serialize(static_cast<uint8_t>(code_)));
    }

    bool deserialize(deserializer *_from) 
    {
        bool is_successful;
        uint8_t tmp_message_type, tmp_return_code;

        is_successful = (0 != _from
                && _from->deserialize(service_)
                && _from->deserialize(method_)
                && _from->deserialize(length_)
                && _from->deserialize(client_)
                && _from->deserialize(session_)
                && _from->deserialize(protocol_version_)
                && _from->deserialize(interface_version_)
                && _from->deserialize(tmp_message_type)
                && _from->deserialize(tmp_return_code));

        if (is_successful) 
        {
            type_ = static_cast< message_type_e >(tmp_message_type);
            code_ = static_cast< return_code_e >(tmp_return_code);
        }
        return is_successful;
    }



};


int main()
{
    message msg(1, 2, 3, 4, 5, 6, 7, message_type_e::MT_UNKNOWN, return_code_e::E_WRONG_MESSAGE_TYPE);

    serializer mySerializer(50);
    msg.serialize(&mySerializer);
    
    const uint8_t* data = mySerializer.get_data();

    for (size_t i = 0; i < mySerializer.get_size(); i++) {
        std::cout << static_cast<std::uint32_t>(data[i]) << std::endl;
    }
    
    //mySerializer.print();
    return 0;
}