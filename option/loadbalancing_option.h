#ifndef LOADBALANCING_OPTION_H
#define LOADBALANCING_OPTION_H

#include <memory>
#include "./option.h"

namespace ara
{
    namespace com
    {
        namespace option
        {
            /// @brief Service load balancing option
            class LoadBalancingOption : public Option
            {
            private:
                /******************************* extra attributes *************************/

                uint16_t mPriority;
                uint16_t mWeight;

            public:
                /* instances of this class cannot be created without providing specific arguments to a constructor. */

                LoadBalancingOption() = delete;



                /******************************* constructor ******************************/

                /// @brief Constructor
                /// @param discardable Indicates whether the option can be discarded or not
                /// @param priority Service instance priority
                /// @param weight Servince instance random selection weight
                constexpr LoadBalancingOption(
                    bool discardable,
                    uint16_t priority,
                    uint16_t weight) noexcept : Option(OptionType::LoadBalancing, discardable),
                                                mPriority{priority},
                                                mWeight{weight}
                {}




                /*************** override virtual functions inherited from parent*************/

                virtual uint16_t Length() const noexcept override;

                virtual std::vector<uint8_t> Payload() const override;



                /************************************* getters *******************************/

                /// @brief Get priority
                /// @returns Service instance priority
                uint16_t Priority() const noexcept;

                /// @brief Get weight
                /// @returns Servince instance random selection weight
                uint16_t Weight() const noexcept;



                /********************************** static functions **************************/

                /// @brief Deserialize an option payload
                /// @param payload Serialized option payload byte array
                /// @param offset Deserializing offset in the payload
                /// @param discardable Indicates whether the option can be discarded or not
                /// @returns Deserialized option
                static std::unique_ptr<LoadBalancingOption> Deserialize(
                    const std::vector<uint8_t> &payload,
                    std::size_t &offset,
                    bool discardable);
            };
        }
    }
}

#endif