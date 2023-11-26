#ifndef SERVICE_DOWN_STATE_H
#define SERVICE_DOWN_STATE_H

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
                    /// @brief Machine state in which the service is down 
                    class ServiceDownState : public helper::MachineState<helper::PubSubState>
                    {
                    protected:
                        /*************** override virtual functions inherited from parent*************/

                        void Deactivate(helper::PubSubState nextState) override;
                    public:
                        void Activate(helper::PubSubState previousState) override;

                        // void Transit(T nextState)


                        /**************************** fundemental functions *************************/

                        /// @brief Notify that the server has been started
                        void Started();



                        /*********************** constructor **************************************/

                        ServiceDownState() noexcept;

                        
                        
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