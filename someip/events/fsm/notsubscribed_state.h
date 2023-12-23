#ifndef NOTSUBSCRIBED_STATE_H
#define NOTSUBSCRIBED_STATE_H

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
                    /*
                    PubSubState is enum class represenet state of Publish-subscribe server
                    */

                    /// @brief Machine state in which the service is up but has no subscriber 
                    class NotSubscribedState : public helper::State<helper::PubSubState>
                    {
                    protected:
                        /*************** override virtual functions inherited from parent*************/

                        void Deactivate(helper::PubSubState nextState) override
                        {
                            // Nothing to do on deactivation
                        }

                        void Activate(helper::PubSubState previousState) override
                        {
                            // Nothing to do on activation
                        }

                    public:
                        /*********************** constructor **************************************/

                        NotSubscribedState() noexcept  : helper::State<helper::PubSubState>(helper::PubSubState::NotSubscribed)
                        {}



                       /**************************** fundemental functions *************************/

                        /// @brief Notify that a new client has subscribed to the server
                        void Subscribed()
                        {
                            Transit(helper::PubSubState::Subscribed);
                        }

                        /// @brief Notify that the server has been stopped
                        void Stopped()
                        {
                            Transit(helper::PubSubState::ServiceDown);
                        }



                        /***************** disable copy constructor and assigment operator *********/

                        NotSubscribedState(const NotSubscribedState &) = delete;
                        NotSubscribedState &operator=(const NotSubscribedState &) = delete;
                    };
                }
            }
        }
    }
}

#endif