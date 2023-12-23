#include "entry.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace entry
        {
            /************************ useful for constructor of child ******************/

            Entry::Entry(EntryType type,
                         uint16_t serviceId,
                         uint16_t instanceId,
                         uint32_t ttl,
                         uint8_t majorVersion) noexcept : mType{type},
                                                          mServiceId{serviceId},
                                                          mInstanceId{instanceId},
                                                          mTTL{ttl},
                                                          mMajorVersion{majorVersion}
            {}



            /***************** move constructor and move assigment operator ************/

            Entry::Entry(Entry &&other) : mType{other.mType},
                                          mFirstOptions{std::move(other.mFirstOptions)},
                                          mSecondOptions{std::move(other.mSecondOptions)},
                                          mServiceId{other.mServiceId},
                                          mInstanceId{other.mInstanceId},
                                          mTTL{other.mTTL},
                                          mMajorVersion{other.mMajorVersion}
            {
            }

            Entry &Entry::operator=(Entry &&other)
            {
                mType = other.mType;
                mFirstOptions = std::move(other.mFirstOptions);
                mSecondOptions = std::move(other.mSecondOptions);
                mServiceId = other.mServiceId;
                mInstanceId = other.mInstanceId;
                mTTL = other.mTTL;
                mMajorVersion = other.mMajorVersion;

                return *this;
            }

            

            /********************************** getters *********************************/

            EntryType Entry::Type() const noexcept
            {
                return mType;
            }

            uint16_t Entry::ServiceId() const noexcept
            {
                return mServiceId;
            }

            uint16_t Entry::InstanceId() const noexcept
            {
                return mInstanceId;
            }

            uint8_t Entry::MajorVersion() const noexcept
            {
                return mMajorVersion;
            }

            uint32_t Entry::TTL() const noexcept
            {
                return mTTL;
            }

            const std::vector<std::unique_ptr<option::Option>> &Entry::FirstOptions() const noexcept
            {
                return mFirstOptions;
            }      

            const std::vector<std::unique_ptr<option::Option>> &Entry::SecondOptions() const noexcept
            {
                return mSecondOptions;
            }



            /****************************** fundemental functions ***********************/

            void Entry::AddFirstOption(std::unique_ptr<option::Option> firstOption)
            {
                bool _valid = ValidateOption(firstOption.get());

                if (_valid)
                {
                    mFirstOptions.push_back(std::move(firstOption));
                }
                else
                {
                    throw std::invalid_argument("The option cannot be added.");
                }
            }

            void Entry::AddSecondOption(std::unique_ptr<option::Option> secondOption)
            {
                bool _valid = ValidateOption(secondOption.get());

                if (_valid)
                {
                    mSecondOptions.push_back(std::move(secondOption));
                }
                else
                {
                    throw std::invalid_argument("The option cannot be added.");
                }
            }



            /***************************** for my child but virtual **********************/
            
            bool Entry::ValidateOption(const option::Option *option) const noexcept
            {
                bool _result;

                switch (option->Type())
                {
                    case option::OptionType::Configuration:
                    {
                        bool _containsConfiguration =
                            ContainsCertainOption(option::OptionType::Configuration);

                        // Each entry can only have at maximum one configuration option.
                        _result = !_containsConfiguration;

                        break;
                    }
                    case option::OptionType::LoadBalancing:
                    {
                        bool _containsLoadBalancing =
                            ContainsCertainOption(option::OptionType::LoadBalancing);

                        // Each entry can only have at maximum one load balancing option.
                        _result = !_containsLoadBalancing;

                        break;
                    }
                    case option::OptionType::IPv4SdEndpoint:
                    case option::OptionType::IPv6SdEndpoint:
                    {
                        // Service discovery endpoints are not allowed in entries.
                        _result = false;

                        break;
                    }
                    default:
                    {
                        // Other options cannot be validated in the base entry class.
                        _result = true;

                        break;
                    }
                }

                return _result;
            }

            bool Entry::ContainsCertainOption(option::OptionType optionType) const noexcept
            {
                for (auto &firstOption : mFirstOptions)
                {
                    if (firstOption->Type() == optionType)
                    {
                        return true;
                    }
                }

                for (auto &secondOption : mSecondOptions)
                {
                    if (secondOption->Type() == optionType)
                    {
                        return true;
                    }
                }

                return false;
            }

            std::vector<uint8_t> Entry::BasePayload(uint8_t &optionIndex) const
            {
                // define vector
                std::vector<uint8_t> _result;


                // size of vector of options
                uint8_t _firstOptionsSize = FirstOptions().size();
                uint8_t _secondOptionsSize = SecondOptions().size();


                /******** first4Byte ********/

                // firstByte 
                _result.push_back(static_cast<uint8_t>(Type()));
                // secondByte 
                _result.push_back(optionIndex);
                // thirdByte 
                _result.push_back(optionIndex + _firstOptionsSize);
                // fourthByte 
                _result.push_back((_firstOptionsSize << 4) | (_secondOptionsSize));

                
                optionIndex += (_firstOptionsSize + _secondOptionsSize);


                /******** second4Byte ********/

                helper::Inject(_result, ServiceId());
                helper::Inject(_result, InstanceId());


                /******** third4Byte ********/

                uint32_t _third4Byte = MajorVersion();
                _third4Byte <<= 24;
                _third4Byte |= TTL();
                helper::Inject(_result, _third4Byte);


                return _result;
            }

            
            
            /****************************** non-pure virtual functions **************************/

            void Entry::print() const noexcept
            {
                switch(Type())
                {
                    case EntryType::Finding:
                    std::cout << "Entry:Type : " << "Finding" << std::endl;
                    break;

                    case EntryType::Offering:
                    std::cout << "Entry:Type : " << "Offering" << std::endl;
                    break;

                    case EntryType::Subscribing:
                    std::cout << "Entry:Type : " << "Subscribing" << std::endl;
                    break;

                    case EntryType::Acknowledging:
                    std::cout << "Entry:Type : " << "Acknowledging" << std::endl;
                    break;
                }
                std::cout << "Entry ServiceId: " << ServiceId() << std::endl;
                std::cout << "Entry InstanceId: " << InstanceId() << std::endl;
                std::cout << "Entry MajorVersion: " << static_cast<int>(MajorVersion()) << std::endl;
                std::cout << "Entry TTL: " << TTL() << std::endl;
            }

        }
    }
}