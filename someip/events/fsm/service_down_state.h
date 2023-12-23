#ifndef SERVICE_DOWN_STATE_H
#define SERVICE_DOWN_STATE_H

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
                    /// @brief Machine state in which the service is down 
                    class ServiceDownState : public helper::State<helper::PubSubState>
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

                        ServiceDownState() noexcept: helper::State<helper::PubSubState>(helper::PubSubState::ServiceDown)
                        {}



                        /**************************** fundemental functions *************************/

                        /// @brief Notify that the server has been started
                        void Started()
                        {
                            Transit(helper::PubSubState::NotSubscribed);
                        }


                        
                        /***************** disable copy constructor and assigment operator *********/

                        ServiceDownState(const ServiceDownState &) = delete;
                        ServiceDownState &operator=(const ServiceDownState &) = delete;
                    };
                }
            }
        }
    }
}

#endif