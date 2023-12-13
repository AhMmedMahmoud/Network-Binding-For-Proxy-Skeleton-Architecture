#ifndef NOTREADY_STATE_H
#define NOTREADY_STATE_H

#include "../../../helper/state.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                namespace fsm
                {
                    /// @brief Machine state when the server's service is down or stopped
                    /// @note The state is not copyable
                    class NotReadyState : public helper::State<helper::SdServerState>
                    {
                        /****************** inherited private attributes **************************
                        const T mState;
                        AbstractStateMachine<T> *mFSM;
                        std::function<void(T, T)> mTransitionCallback;

                        void Register(AbstractStateMachine<T> *finiteStateMachine) noexcept
                        {  mFSM = finiteStateMachine;  }

                        void Transit(T nextState)
                        {
                            Deactivate(nextState);
                            if (mFSM) {   mFSM->Transit(mState, nextState);   }
                        }   

                        pure vitural functions : activate and deactivate     
                        getter                 : GetState
                        ***************************************************************************/

                    private:
                        /************************** extra attributes ********************/

                        // handle that will invoke when service is stopped
                        // (sending someip/sd message has stop offering entry)
                        const std::function<void()> mOnServiceStopped;


                    public:
                        /********************** constructor ******************************/

                        /// @brief Constructor
                        /// @param onServiceStopped Callback to be invoked when the service is stopped
                        NotReadyState(std::function<void()> onServiceStopped): helper::State<helper::SdServerState>(helper::SdServerState::NotReady),
                                                                               mOnServiceStopped(onServiceStopped)
                        {}

                        

                        /************************* fundemental functions ***************************/

                        void Activate(helper::SdServerState previousState) override
                        {
                            // If the state activate not as the entrypoint, it means the service has been stopped.
                            // At entrypoint the current state and the previous state are the same.
                            if (previousState != helper::SdServerState::NotReady)
                            {
                                mOnServiceStopped();
                            }
                        }

                        /// @brief Inform the state that the server's service is up
                        void ServiceActivated()
                        {
                            std::cout << "Service Activated is called\n";
                            Transit(helper::SdServerState::InitialWaitPhase);
                        }





                        /****** disable emptyConstructor-copyConstructor-copyAssigmentOperator *****/
                        
                        NotReadyState() = delete;
                        NotReadyState(const NotReadyState &) = delete;
                        NotReadyState &operator=(const NotReadyState &) = delete;

                    protected:
                        /****************************** for my child *******************************/

                        void Deactivate(helper::SdServerState nextState) override
                        {
                            // Nothing to do on deactivation.
                        }
                    };
                }
            }
        }
    }
}

#endif