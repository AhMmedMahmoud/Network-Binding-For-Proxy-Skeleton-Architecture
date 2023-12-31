#ifndef OPTION_DESERIALIZER_H
#define OPTION_DESERIALIZER_H

#include "./ipv4_endpoint_option.h"
#include "./loadbalancing_option.h"

namespace ara
{
    namespace com
    {
        namespace option
        {
            /// @brief A helper class for deserialzing an option payload
            class OptionDeserializer
            {
            public:
                /// @brief Deserialize an option payload
                /// @param payload Serialized option payload byte array
                /// @param offset Deserializing offset in the payload
                /// @returns Deserialized option
                /// @throws std::out_of_range Throws when the option type for deserializing is not supported
                static std::unique_ptr<Option> Deserialize(
                    const std::vector<uint8_t> &payload, std::size_t &offset)
                {
                    // Apply the option length field offset
                    offset += 2;

                    auto _type = static_cast<OptionType>(payload.at(offset++));
                    auto _discardable = static_cast<bool>(payload.at(offset++));

                    switch (_type)
                    {
                        case OptionType::IPv4Endpoint:
                        case OptionType::IPv4Multicast:
                        case OptionType::IPv4SdEndpoint:
                            return Ipv4EndpointOption::Deserialize(payload, offset, _type, _discardable);

                        case OptionType::LoadBalancing:
                            return LoadBalancingOption::Deserialize(payload, offset, _discardable);

                        default:
                            throw std::out_of_range("Option type is not supported for deserializing.");
                    }
                }
            };
        }
    }
}

#endif