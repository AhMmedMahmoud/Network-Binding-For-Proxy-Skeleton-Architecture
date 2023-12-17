#ifndef FINITE_MACHINE_STATE_H
#define FINITE_MACHINE_STATE_H

#include <map>
#include <initializer_list>
#include "state.h"
#include <iostream>
#include <type_traits>


namespace ara
{
    namespace com
    {
        namespace helper
        {
            /// @brief Finite State Machine (FMS) controller
            /// @details FMS controller is responsible for transiting between added states to the machine
            /// @tparam T State enumeration type
            /// @note FSM controller is not copyable
            template <typename T>
            class FSM  : public AbstractStateMachine<T>
            {
            private:
                /******************************* attributes ******************************/
                
                T mCurrentState;                  // enum represent current state of FSM
                std::map<T, State<T> *> mStates;  // to store states inside FSM

            public:
                /*************************** fundemental functions **************************/

                /// @brief Initalize the FSM
                /// @param states Machine state list
                /// @param entrypoint Entrypoint state to initialize the FSM
                void Initialize(std::initializer_list<State<T> *> states, T entrypoint)
                {
                    for (auto state : states)
                    {
                        mStates.emplace(state->GetState(), state);
                        state->Register(this);
                    }

                    auto _initialState = mStates.at(entrypoint);
                    // At entrypoint the previous state and the next state are the same.
                    _initialState->Activate(entrypoint);
                    mCurrentState = entrypoint;
                }



                /*************** override virtual functions inherited from parent*************/

                void TransitThenActivate(T currentState, T nextState)  override
                {
                    /*
                    std::cout << "TransitThenActivate is called" << std::endl;
                    
                    
                    switch(static_cast<int>(currentState)){
                    case static_cast<int>(SdServerState::NotReady):  std::cout << "NotReady --> ";                   break;
                    case static_cast<int>(SdServerState::InitialWaitPhase):  std::cout << "InitialWaitPhase --> ";   break;
                    case static_cast<int>(SdServerState::RepetitionPhase):  std::cout << "RepetitionPhase --> ";     break;
                    case static_cast<int>(SdServerState::MainPhase):  std::cout << "MainPhase --> ";                 break;
                    default: std::cout << static_cast<int>(currentState) << " --> ";                                 break;
                    }
                    
                    switch(static_cast<int>(nextState))
                    {
                    case static_cast<int>(SdServerState::NotReady):  std::cout << " NotReady\n";                   break;
                    case static_cast<int>(SdServerState::InitialWaitPhase):  std::cout << " InitialWaitPhase\n";   break;
                    case static_cast<int>(SdServerState::RepetitionPhase):  std::cout << " RepetitionPhase\n";     break;
                    case static_cast<int>(SdServerState::MainPhase):  std::cout << " MainPhase\n";                 break;
                    default: std::cout << static_cast<int>(nextState) << "\n"; break;
                    }
                    */
                    
                    

                    // Only current state should be able to transit to another state
                    if (currentState == mCurrentState)
                    {
                        auto _nextMachineState = mStates.at(nextState);
                        mCurrentState = nextState;
                        /*
                        switch(static_cast<int>(nextState))
                        {
                        case static_cast<int>(SdServerState::NotReady):  std::cout << "----- state NotReady is activated -----\n";                   break;
                        case static_cast<int>(SdServerState::InitialWaitPhase):  std::cout << "----- state InitialWaitPhase is activated -----\n";   break;
                        case static_cast<int>(SdServerState::RepetitionPhase):  std::cout << "----- state RepetitionPhase is activated -----\n";     break;
                        case static_cast<int>(SdServerState::MainPhase):  std::cout << "----- state MainPhase is activated -----\n";                 break;
                        default: std::cout << "----- state " << static_cast<int>(nextState) << " is activated -----\n"; break;
                        }
                        */
                        _nextMachineState->Activate(currentState);
                    }
                }



                /************** tell compiler to generate default constructor*************/

                FSM() noexcept = default;



                /********************************** getters *********************************/

                /// @brief Get the FSM current state
                /// @returns Current state enumeration
                T GetState() const noexcept
                {
                    return mCurrentState;
                }

                /// @brief Get the current machine state object
                /// @returns Machine state object pointer
                State<T> *GetMachineState() const
                {
                    return mStates.at(mCurrentState);
                }



                /*********** disable copy constructor and copy assigment operator ********/

                FSM(const FSM &) = delete;
                FSM &operator=(const FSM &) = delete;



                /****************** tell compiler to generate default desctructor  ********************/

                ~FSM() noexcept = default;
            };
        }
    }
}
#endif