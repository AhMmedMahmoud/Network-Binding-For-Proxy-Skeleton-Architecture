#include "./option.h"

namespace ara
{
    namespace com
    {
        namespace option
        {
            /********************************** getters *********************************/

            OptionType Option::Type() const noexcept
            {
                return mType;
            }

            bool Option::Discardable() const noexcept
            {
                return mDiscardable;
            }



            /***************************** for my child ******************************/

            std::vector<uint8_t> Option::BasePayload() const
            {
                std::vector<uint8_t> _result;
                /*
                _result is vector of uint8_t
                
                uint16_t Length

                uint8_t mType
                uint8_t mDiscardable
                */ 
                helper::Inject(_result, Length());

                _result.push_back(static_cast<uint8_t>(mType));
                _result.push_back(static_cast<uint8_t>(mDiscardable));

                return _result;
            }
        }
    }
}