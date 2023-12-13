#ifndef INITIAL_WAIT_STATE_H
#define INITIAL_WAIT_STATE_H

#include "timer_set_state.h"
#include <random>
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
                    /// @brief Server's or client's service initial wait state
                    /// @tparam T Server's or client state enumeration type
                    /// @note The state is not copyable
                    template <typename T>
                    class InitialWaitState : public TimerSetState<T>
                    {
                    protected:
                        /************************ attributes *****************************/

                        const int InitialDelayMin;   // Initial delay lower bound in milliseconds
                        const int InitialDelayMax;   // Initial delay higher bound in milliseconds



                        /************************* function that parent needs ************/

                        virtual void SetTimer() override
                        {
                            // Generate a random initial delay
                            std::default_random_engine _generator;
                            std::uniform_int_distribution<int> _distribution(InitialDelayMin, InitialDelayMax);
                            int _randomDely = _distribution(_generator);

                            // Sleep for the initali random delay and
                            // then transit to the next state
                            auto _delay = std::chrono::milliseconds(_randomDely);
                            bool _interrupted = this->WaitFor(_delay);
                            if (!_interrupted)
                            {
                                std::cout << "Timer is expired\n";

                                // Invoke the on timer expiration callback
                                this->OnTimerExpired();
                            }
                        }

                    public:
                        /****************************** constructor ********************************/

                        /// @brief Constructor
                        /// @param currentState Current state at initial wait phase
                        /// @param nextState Next state after initial wait phase expiration
                        /// @param stoppedState Stopped state after put a stop to the service
                        /// @param onTimerExpired Delegate to be invoked by timer's thread when the timer is expired
                        /// @param initialDelayMin Minimum initial delay in milliseconds
                        /// @param initialDelayMax Maximum initial delay in milliseconds
                        InitialWaitState( T currentState,
                                          T nextState,
                                          T stoppedState,
                                          std::function<void()> onTimerExpired,
                                          int initialDelayMin,
                                          int initialDelayMax) : helper::State<T>(currentState),
                                                                 TimerSetState<T>(nextState, stoppedState, onTimerExpired),
                                                                 InitialDelayMin{initialDelayMin},
                                                                 InitialDelayMax{initialDelayMax}
                        {
                            if( (initialDelayMin < 0) || (initialDelayMax < 0) || (initialDelayMin > initialDelayMax))
                            {
                                throw std::invalid_argument("Invalid initial delay minimum and/or maximum.");
                            }
                        }



                        /****** disable emptyConstructor-copyConstructor-copyAssigmentOperator *****/

                        InitialWaitState() = delete;
                        InitialWaitState(const InitialWaitState &) = delete;
                        InitialWaitState &operator=(const InitialWaitState &) = delete;
                    };
                }
            }
        }
    }
}

#endif