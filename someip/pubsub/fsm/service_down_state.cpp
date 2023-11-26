#include "./service_down_state.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace pubsub
            {
                namespace fsm
                {
                    /*********************** constructor **************************************/

                    ServiceDownState::ServiceDownState() noexcept : helper::MachineState<helper::PubSubState>(helper::PubSubState::ServiceDown)
                    {}



                    /**************************** fundemental functions *************************/

                    void ServiceDownState::Started()
                    {
                        Transit(helper::PubSubState::NotSubscribed);
                    }



                    /*************** override virtual functions inherited from parent*************/
                    
                    void ServiceDownState::Activate(helper::PubSubState previousState)
                    {
                        // Nothing to do on activation
                    }

                    void ServiceDownState::Deactivate(helper::PubSubState nextState)
                    {
                        // Nothing to do on deactivation
                    }
                }
            }
        }
    }
}