#include "./notsubscribed_state.h"

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

                    NotSubscribedState::NotSubscribedState() noexcept : helper::MachineState<helper::PubSubState>(helper::PubSubState::NotSubscribed)
                    {}


                    /**************************** fundemental functions *************************/

                    void NotSubscribedState::Subscribed()
                    {
                        Transit(helper::PubSubState::Subscribed);
                    }

                    void NotSubscribedState::Stopped()
                    {
                        Transit(helper::PubSubState::ServiceDown);
                    }



                    /*************** override virtual functions inherited from parent*************/

                    void NotSubscribedState::Activate(helper::PubSubState previousState)
                    {
                        // Nothing to do on activation
                    }

                    void NotSubscribedState::Deactivate(helper::PubSubState nextState)
                    {
                        // Nothing to do on deactivation
                    }
                }
            }
        }
    }
}