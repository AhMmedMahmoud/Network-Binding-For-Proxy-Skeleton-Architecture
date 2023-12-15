#ifndef STATE_H
#define STATE_H

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


            /*
                generic for any state
                ---------------------
                Transit    : uses the FSM at which it related to for making transit to new state and activate it 
                Register   : set FSM at which it related to (used in FSM->initilize() )
                GetState   : get value of enum that represent the state
                
                particular for xxxx State
                -----------------------------------
                Activate   : do certain logic then transit to xxxxx or xxxxx state and activate xxxxx  
                Deactivate : called inside Transit so no need to make it public
                some functions ..
            */

            /// @brief abstract class represents state
            /// @tparam T State enumeration type
            template <typename T>
            class State
            {
            private:
                /******************************** attributes *****************************/

                const T mState;                 // enum that store State as enum
                AbstractStateMachine<T> *mFSM;  // store FSM at which this state belong to
            
            protected:
                /********************* for my child but virtual ******************************/

                /// @brief Deactivating the current state before transiting to the next state
                /// @param nextState Next state coming after deactivation
                virtual void Deactivate(T nextState) = 0;
                
                
                
                /***************************** for my child ******************************/

                /// @brief Transit to the next state
                /// @param nextState Next state coming after the current state
                void Transit(T nextState)
                {
                    Deactivate(nextState);
                    if (mFSM)
                    {
                        mFSM->TransitThenActivate(mState, nextState);
                    }
                }



                /*********************** useful in constructor of my child ******************/

                /// @brief Constructor
                /// @param state Machine state enumeration
                State(T state) noexcept : mState{state},
                                          mFSM{nullptr}
                {}

            public:
                /********************************** pure virtual functions **************************/

                /// @brief Activate the state
                /// @param previousState Previous state before transiting to this state
                virtual void Activate(T previousState) = 0;



                /*************************** fundemental functions **************************/

                // this function is called inside Initialize function of FSM
                /// @brief Register the state to a finite state machine (FSM)
                /// @param finiteStateMachine Finite state machine that contains the state
                void Register(AbstractStateMachine<T> *finiteStateMachine) noexcept
                {
                    mFSM = finiteStateMachine;
                }



                /********************************** getters *********************************/

                /// @brief Get the machine state
                /// @returns Machine state enumeration
                T GetState() const noexcept
                {
                    return mState;
                }


               
                /*********** disable copy constructor and copy assigment operator ********/

                State(const State &) = delete;
                State &operator=(const State &) = delete;



                /*********** tell compiler to generate default desctructor  ****************/

                virtual ~State() = default;
            };
        }
    }
}

#endif