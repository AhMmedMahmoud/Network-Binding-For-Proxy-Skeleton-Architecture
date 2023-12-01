#ifndef NOTSUBSCRIBED_STATE_H
#define NOTSUBSCRIBED_STATE_H

#include "../../../helper/machine_state.h"

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
                    class NotSubscribedState : public helper::MachineState<helper::PubSubState>
                    {
                    protected:
                        /*************** override virtual functions inherited from parent*************/

                        void Deactivate(helper::PubSubState nextState) override;
                    public:
                        void Activate(helper::PubSubState previousState) override;

                        // void Transit(T nextState)



                       /**************************** fundemental functions *************************/

                        /// @brief Notify that a new client has subscribed to the server
                        void Subscribed();

                        /// @brief Notify that the server has been stopped
                        void Stopped();



                        /*********************** constructor **************************************/

                        NotSubscribedState() noexcept;



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