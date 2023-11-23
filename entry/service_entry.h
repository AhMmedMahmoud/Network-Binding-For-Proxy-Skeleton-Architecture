#ifndef SERVICE_ENTRY_H
#define SERVICE_ENTRY_H

#include <stdint.h>
#include "./entry.h"

namespace ara
{
    namespace com
    {
        namespace entry
        {
            /// @brief Entry to find and offer a service
            class ServiceEntry : public Entry
            {
            private:
                /********************************* constants ******************************/

                static const uint32_t cInfiniteTtl = 0xffffff;



                /******************************* extra attributes *************************/

                uint32_t mMinorVersion;



               /************************ it helps in static functions *********************/

                ServiceEntry(EntryType type,
                             uint16_t serviceId,
                             uint16_t instanceId,
                             uint32_t ttl,
                             uint8_t majorVersion,
                             uint32_t minorVersion) noexcept;

            protected:
                virtual bool ValidateOption(
                    const option::Option *option) const noexcept override;

            public:
                /********************************* constants ******************************/

                /// @brief Any service minor version
                static const uint32_t cAnyMinorVersion = 0xffffffff;




                /* instances of this class cannot be created without providing specific arguments to a constructor. */
                ServiceEntry() = delete;



                /***************** move constructor and move assigment operator *************/

                ServiceEntry(ServiceEntry &&other);
                ServiceEntry &operator=(ServiceEntry &&other);



                /********************************** getters *********************************/

                /// @brief Get minor version
                /// @returns Service minor version
                uint32_t MinorVersion() const noexcept;




                /*************** override virtual functions inherited from parent*************/

                virtual std::vector<uint8_t> Payload(uint8_t &optionIndex) const override;




                /********************************** static functions **************************/

                /// @brief Find a service entry factory
                /// @param serviceId Service in interest ID
                /// @param ttl Entry time to live
                /// @param instanceId Service in interest instance ID
                /// @param majorVersion Service in interest major version
                /// @param minorVersion Service in interest minor version
                /// @returns Find service entry
                static std::unique_ptr<ServiceEntry> CreateFindServiceEntry(
                    uint16_t serviceId,
                    uint32_t ttl = cInfiniteTtl,
                    uint16_t instanceId = cAnyInstanceId,
                    uint8_t majorVersion = cAnyMajorVersion,
                    uint32_t minorVersion = cAnyMinorVersion);

                /// @brief Offer a service entry factory
                /// @param serviceId Service in interest ID
                /// @param instanceId Service in interest instance ID
                /// @param majorVersion Service in interest major version
                /// @param minorVersion Service in interest minor version
                /// @param ttl Service offering lifetime
                /// @returns Offer service entry
                static std::unique_ptr<ServiceEntry> CreateOfferServiceEntry(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint32_t minorVersion,
                    uint32_t ttl = cInfiniteTtl);

                /// @brief Stop offer a service entry factory
                /// @param serviceId Service in interest ID
                /// @param instanceId Service in interest instance ID
                /// @param majorVersion Service in interest major version
                /// @param minorVersion Service in interest minor version
                /// @returns Stop service offering entry
                static std::unique_ptr<ServiceEntry> CreateStopOfferEntry(
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint8_t majorVersion,
                    uint32_t minorVersion) noexcept;

                /// @brief Deserialize a entry payload
                /// @param payload Serialized entry payload byte array
                /// @param offset Deserializing offset in the payload
                /// @param type Entry type
                /// @param serviceId Service in interest ID
                /// @param instanceId Service in interest instance ID
                /// @param ttl Entry time to live
                /// @param majorVersion Service in interest major version
                /// @returns Deserialized entry
                /// @throws std::out_of_range Throws when the entry type is not a service entry
                static std::unique_ptr<ServiceEntry> Deserialize(
                    const std::vector<uint8_t> &payload,
                    std::size_t &offset,
                    EntryType type,
                    uint16_t serviceId,
                    uint16_t instanceId,
                    uint32_t ttl,
                    uint8_t majorVersion);
            };
        }
    }
}

#endif