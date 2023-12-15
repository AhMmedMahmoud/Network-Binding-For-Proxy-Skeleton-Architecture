#ifndef SUBSCRIBED_STATE_H
#define SUBSCRIBED_STATE_H

#include <cstdint>
#include "../../../helper/state.h"

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
                    /// @brief Machine state in which the service is up and it has at least a subscriber
                    class SubscribedState : public helper::State<helper::PubSubState>
                    {
                    private:
                        int32_t mSubscriptionCounter;

                    protected:
                        /*************** override virtual functions inherited from parent*************/

                        void Deactivate(helper::PubSubState nextState) override
                        {
                            // Nothing to do on deactivation
                        }

                        void Activate(helper::PubSubState previousState) override
                        {
                           mSubscriptionCounter = 1;
                        }

                    public:
                        /*********************** constructor **************************************/

                        SubscribedState() noexcept: helper::State<helper::PubSubState>(helper::PubSubState::Subscribed)
                        {}



                        /**************************** fundemental functions *************************/

                        /// @brief Notify that a new client has subscribed to the server
                        void Subscribed() noexcept
                        {
                            ++mSubscriptionCounter;
                        }

                        /// @brief Notify that a new client has unsubscribed from the server
                        void Unsubscribed()
                        {
                            --mSubscriptionCounter;

                            if (mSubscriptionCounter <= 0)
                            {
                                Transit(helper::PubSubState::NotSubscribed);
                            }
                        }

                        /// @brief Notify that the server has been stopped
                        void Stopped()
                        {
                            Transit(helper::PubSubState::ServiceDown);
                        }    


                        /***************** disable copy constructor and assigment operator *********/

                        SubscribedState(const SubscribedState &) = delete;
                        SubscribedState &operator=(const SubscribedState &) = delete;
                    };
                }
            }
        }
    }
}

#endif