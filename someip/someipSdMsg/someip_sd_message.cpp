#include "./someip_sd_message.h"
#include "entry/entry_deserializer.h"
#include "option/option_deserializer.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                /******************************* constructor  ******************************/

                SomeIpSdMessage::SomeIpSdMessage() : SomeIpMessage(
                                                         cMessageId,
                                                         cClientId,
                                                         cProtocolVersion,
                                                         cInterfaceVersion,
                                                         cMessageType),
                                                         mRebooted{true}
                {}



                /***************** move constructor and move assigment operator *************/

                SomeIpSdMessage::SomeIpSdMessage(SomeIpSdMessage &&other) : SomeIpMessage{std::move(other)},
                                                                            mRebooted{other.mRebooted},
                                                                            mEntries{std::move(other.mEntries)}
                {}

                SomeIpSdMessage &SomeIpSdMessage::operator=(SomeIpSdMessage &&other)
                {
                    SomeIpMessage::operator=(std::move(other));
                    mRebooted = other.mRebooted;
                    mEntries = std::move(other.mEntries);

                    return *this;
                }


                /********************************** getters *********************************/

                uint32_t SomeIpSdMessage::getEntriesLength() const noexcept
                {
                    return (mEntries.size() * 16);
                }

                uint32_t SomeIpSdMessage::getOptionsLength() const noexcept
                {
                    uint32_t _result = 0;

                    for (auto &entry : mEntries)
                    {
                        for (auto &firstOption : entry->FirstOptions())
                        {
                            _result += 3 + firstOption->Length();
                        }

                        for (auto &secondOption : entry->SecondOptions())
                        {
                            _result += 3 + secondOption->Length();
                        }
                    }

                    return _result;
                }

                const std::vector<std::unique_ptr<entry::Entry>> &SomeIpSdMessage::Entries() const noexcept
                {
                    return mEntries;
                }

                


                /**************************** fundemental functions *************************/

                void SomeIpSdMessage::AddEntry(std::unique_ptr<entry::Entry> entry)
                {
                    mEntries.push_back(std::move(entry));
                }



                /*************** override virtual functions inherited from parent*************/

                void SomeIpSdMessage::SetSessionId(uint16_t sessionId) 
                {
                    if (sessionId == 0)
                    {
                        throw std::invalid_argument("Session ID cannot be zero for the SD message.");
                    }

                    SomeIpMessage::SetSessionId(sessionId);
                }

                bool SomeIpSdMessage::IncrementSessionId() noexcept
                {
                    bool _wrapped = SomeIpMessage::IncrementSessionId();
                    // If the message is rebooted, check for wrapping
                    mRebooted &= !_wrapped;

                    return _wrapped;
                }

                uint32_t SomeIpSdMessage::Length() const noexcept
                {
                    return (20 + getEntriesLength() + getOptionsLength());
                }

                std::vector<uint8_t> SomeIpSdMessage::Payload() const
                {
                    // General SOME/IP header payload insertion
                    std::vector<uint8_t> _result = SomeIpMessage::Payload();

                    if (mRebooted)
                    {
                        // Unicast Support flag is on.
                        helper::Inject(_result, cRebootedFlag);
                    }
                    else
                    {
                        // Unicast Support flag is on.
                        helper::Inject(_result, cNotRebootedFlag);
                    }

                    uint8_t _lastOptionIndex = 0;
                    std::vector<uint8_t> _entriesPayload;
                    std::vector<uint8_t> _optionsPayload;
                    for (auto &entry : mEntries)
                    {
                        auto _entryPayload = entry->Payload(_lastOptionIndex);
                        helper::Concat(
                            _entriesPayload, std::move(_entryPayload));

                        for (auto &firstOption : entry->FirstOptions())
                        {
                            auto _firstOptionPayload = firstOption->Payload();
                            helper::Concat(
                                _optionsPayload, std::move(_firstOptionPayload));
                            ++_lastOptionIndex;
                        }

                        for (auto &secondOption : entry->SecondOptions())
                        {
                            auto _secondOptionPayload = secondOption->Payload();
                            helper::Concat(
                                _optionsPayload, std::move(_secondOptionPayload));
                            ++_lastOptionIndex;
                        }
                    }

                    // Entries length and payloads insertion
                    uint32_t _entriesLength = getEntriesLength();
                    helper::Inject(_result, _entriesLength);
                    helper::Concat(_result, std::move(_entriesPayload));

                    // Options length and payloads insertion
                    uint32_t _optionsLength = getOptionsLength();
                    helper::Inject(_result, _optionsLength);
                    helper::Concat(_result, std::move(_optionsPayload));

                    return _result;
                }

                void SomeIpSdMessage::print() const noexcept
                {
                    std::cout << "------------SOMEIP/SD message------------\n";
                    SomeIpMessage::print();

                   for (auto &entry : Entries())
                    {
                        if (entry->Type() == entry::EntryType::Acknowledging 
                        || entry->Type() == entry::EntryType::Subscribing)
                        {
                            auto _eventgroupEntry = dynamic_cast<entry::EventgroupEntry *>(entry.get());
                            _eventgroupEntry->print();
                        }
                        else if(entry->Type() == entry::EntryType::Offering 
                        || entry->Type() == entry::EntryType::Finding)
                        {
                            auto _serviceEntry = dynamic_cast<entry::ServiceEntry *>(entry.get());
                            _serviceEntry->print();
                        }
                    }
                }


                /********************************** static functions **************************/

                SomeIpSdMessage SomeIpSdMessage::Deserialize(
                    const std::vector<uint8_t> &payload)
                {
                    SomeIpSdMessage _result;
                    SomeIpMessage::Deserialize(&_result, payload);

                    std::size_t _entryOffset = 16;
                    uint32_t _rebootFlag = helper::ExtractInteger(payload, _entryOffset);
                    if (_rebootFlag == cRebootedFlag)
                    {
                        _result.mRebooted = true;
                    }
                    else if (_rebootFlag == cNotRebootedFlag)
                    {
                        _result.mRebooted = false;
                    }
                    else
                    {
                        throw std::out_of_range("The serialized reboot flag is out of range.");
                    }

                    uint32_t _entriesLength = helper::ExtractInteger(payload, _entryOffset);
                    uint32_t _entryOffsetMax = _entryOffset + _entriesLength;

                    std::size_t _optionOffset = _entryOffset + _entriesLength + 4;

                    while (_entryOffset < _entryOffsetMax)
                    {
                        uint8_t _numberOfFirstOptions;
                        uint8_t _numberOfSecondOptions;

                        auto _entry{ entry::EntryDeserializer::Deserialize(payload, _entryOffset, _numberOfFirstOptions, _numberOfSecondOptions)};

                        for (int i = 0; i < _numberOfFirstOptions; i++)
                        {
                            auto _option{ option::OptionDeserializer::Deserialize(payload, _optionOffset) };
                            _entry->AddFirstOption(std::move(_option));
                        }

                        for (int i = 0; i < _numberOfSecondOptions; i++)
                        {
                            auto _option{ option::OptionDeserializer::Deserialize(payload, _optionOffset) };
                            _entry->AddSecondOption(std::move(_option));
                        }

                        _result.AddEntry(std::move(_entry));
                    }

                    return _result;
                }
            }
        }
    }
}