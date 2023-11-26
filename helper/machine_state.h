#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <functional>
#include "./abstract_state_machine.h"

namespace ara
{
    namespace com
    {
        namespace helper
        {
            /// @brief Service discovery server machine state
            enum class SdServerState
            {
                NotReady,         ///< Server's service is down
                InitialWaitPhase, ///< Server's service is in initial waiting phase
                RepetitionPhase,  ///< Server's service is in repetition phase
                MainPhase         ///< Server's service is in main phase
            };

            /// @brief Service discovery client machine state
            enum class SdClientState
            {
                ServiceNotSeen,   ///< Service is not requested and not seen
                ServiceSeen,      ///< Service is not requsted, but seen
                ServiceReady,     ///< Service is ready
                Stopped,          ///< Service is stopped
                InitialWaitPhase, ///< Client service is in initial waiting phase
                RepetitionPhase   ///< Client service is in repetition phase
            };

            /// @brief Publish-subscribe server machine state
            enum class PubSubState
            {
                ServiceDown,    ///< Service server is down
                NotSubscribed,  ///< Service server is up, but there is no subscriber
                Subscribed      ///< Service server is up, and there is at least a subscriber
            };

            /// @brief Machine state abstract class
            /// @tparam T State enumeration type
            /// @note A machine state is not copyable
            template <typename T>
            class MachineState
            {
            private:
                /********************************** getters *********************************/

                const T mState;
                AbstractStateMachine<T> *mFiniteStateMachine;
                std::function<void(T, T)> mTransitionCallback;

            protected:
                /************************ for my child but virtual *************************/

                /// @brief Deactivating the current state before transiting to the next state
                /// @param nextState Next state coming after deactivation
                virtual void Deactivate(T nextState) = 0;



                /***************************** for my child ******************************/

                /// @brief Transit to the next state
                /// @param nextState Next state coming after the current state
                void Transit(T nextState)
                {
                    Deactivate(nextState);
                    if (mFiniteStateMachine)
                    {
                        mFiniteStateMachine->Transit(mState, nextState);
                    }
                }



                /********************************** constructor *********************************/

                /// @brief Constructor
                /// @param state Machine state enumeration
                MachineState(T state) noexcept : mState{state},
                                                 mFiniteStateMachine{nullptr}
                {}

            public:
                /*********** disable copy constructor and copy assigment operator ********/

                MachineState(const MachineState &) = delete;
                MachineState &operator=(const MachineState &) = delete;



                /********************************** getters *********************************/

                /// @brief Get the machine state
                /// @returns Machine state enumeration
                T GetState() const noexcept
                {
                    return mState;
                }



                /*************************** fundemental functions **************************/

                /// @brief Register the state to a finite state machine (FSM)
                /// @param finiteStateMachine Finite state machine that contains the state
                void Register(AbstractStateMachine<T> *finiteStateMachine) noexcept
                {
                    mFiniteStateMachine = finiteStateMachine;
                }



                /********************************** pure virtual functions **************************/

                /// @brief Activate the state
                /// @param previousState Previous state before transiting to this state
                virtual void Activate(T previousState) = 0;



                /****************** tell compiler to generate default desctructor  ********************/

                virtual ~MachineState() = default;
            };
        }
    }
}

#endif