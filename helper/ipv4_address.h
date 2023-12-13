#ifndef IPV4_ADDRESS_H
#define IPV4_ADDRESS_H

#include <array>
#include <vector>
#include <stdint.h>
#include <sstream>

namespace ara
{
    namespace com
    {
        namespace helper
        {
            /// @brief IPv4 address wrapper helper
            struct Ipv4Address
            {
                /// @brief IPv4 address octets
                std::array<uint8_t, 4> Octets;

                Ipv4Address() = delete;

                /// @brief Constructor
                /// @param octet0 IPv4 first octet
                /// @param octet1 IPv4 second octet
                /// @param octet2 IPv4 third octet
                /// @param octet3 IPv4 forth octet
                Ipv4Address( uint8_t octet0,
                             uint8_t octet1, uint8_t octet2, uint8_t octet3) noexcept: Octets{octet0, octet1, octet2, octet3}
                {}

                /// @brief Constructor
                /// @param ipAddress IPv4 address string
                Ipv4Address(std::string ipAddress)
                {
                    char _dot;
                    int _octetInt0, _octetInt1, _octetInt2, _octetInt3;
                    std::istringstream ipAddressStream(ipAddress.c_str());

                    ipAddressStream >>
                        _octetInt0 >> _dot >>
                        _octetInt1 >> _dot >>
                        _octetInt2 >> _dot >>
                        _octetInt3 >> _dot;

                    const auto cOctet0{static_cast<uint8_t>(_octetInt0)};
                    const auto cOctet1{static_cast<uint8_t>(_octetInt1)};
                    const auto cOctet2{static_cast<uint8_t>(_octetInt2)};
                    const auto cOctet3{static_cast<uint8_t>(_octetInt3)};

                    Octets = std::array<uint8_t, 4>{cOctet0, cOctet1, cOctet2, cOctet3};
                }

                /// @brief Convert the IP address to string
                /// @return IP address in string format
                std::string ToString() const
                {
                    const std::string cResult{
                        std::to_string(Octets[0]) + "." +
                        std::to_string(Octets[1]) + "." +
                        std::to_string(Octets[2]) + "." +
                        std::to_string(Octets[3])};

                    return cResult;
                }

                ~Ipv4Address() noexcept = default;

                /// @brief Inject an IP address into a byte vector
                /// @param vector Byte vector
                /// @param ipAddress IP address to be injected
                static void Inject(std::vector<uint8_t> &vector, Ipv4Address ipAddress)
                {
                    vector.insert( vector.end(),
                                   ipAddress.Octets.begin(),
                                   ipAddress.Octets.end()
                                 );
                }

                /// @brief Extract an IPv4 address from a byte vector
                /// @param vector Byte vector
                /// @param offset Extract offset at the vector
                /// @returns Extracted IPv4 address
                static Ipv4Address Extract(const std::vector<uint8_t> &vector, std::size_t &offset)
                {
                    uint8_t _octet0 = vector.at(offset++);
                    uint8_t _octet1 = vector.at(offset++);
                    uint8_t _octet2 = vector.at(offset++);
                    uint8_t _octet3 = vector.at(offset++);
                    Ipv4Address _result(_octet0, _octet1, _octet2, _octet3);

                    return _result;
                }
            };

            /// @brief Ipv4Address equality operator override
            /// @param address1 First address to compare
            /// @param address2 Second address to compare
            /// @returns True if both addresses are equal; otherwise false
            inline bool operator==(Ipv4Address address1, Ipv4Address address2)
            {
                bool _result =
                    (address1.Octets[0] == address2.Octets[0]) &&
                    (address1.Octets[1] == address2.Octets[1]) &&
                    (address1.Octets[2] == address2.Octets[2]) &&
                    (address1.Octets[3] == address2.Octets[3]);

                return _result;
            }

            /// @brief Ipv4Address inequality operator override
            /// @param address1 First address to compare
            /// @param address2 Second address to compare
            /// @returns True if both addresses are not equal; otherwise false
            inline bool operator!=(Ipv4Address address1, Ipv4Address address2)
            {
                bool _result = !(address1 == address2);

                return _result;
            }
        }
    }
}

#endif