#include "./subscribed_state.h"

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

                    SubscribedState::SubscribedState() noexcept : helper::MachineState<helper::PubSubState>(helper::PubSubState::Subscribed)
                    {}



                    /*************** override virtual functions inherited from parent*************/

                    void SubscribedState::Activate(helper::PubSubState previousState)
                    {
                        mSubscriptionCounter = 1;
                    }

                    void SubscribedState::Deactivate(helper::PubSubState nextState)
                    {
                        // Nothing to do on deactivation
                    }



                    /**************************** fundemental functions *************************/

                    void SubscribedState::Subscribed() noexcept
                    {
                        ++mSubscriptionCounter;
                    }

                    void SubscribedState::Unsubscribed()
                    {
                        --mSubscriptionCounter;

                        if (mSubscriptionCounter <= 0)
                        {
                            Transit(helper::PubSubState::NotSubscribed);
                        }
                    }

                    void SubscribedState::Stopped()
                    {
                        Transit(helper::PubSubState::ServiceDown);
                    }  
                }
            }
        }
    }
}