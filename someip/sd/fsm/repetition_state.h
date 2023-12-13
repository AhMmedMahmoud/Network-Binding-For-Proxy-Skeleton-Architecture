#ifndef REPETITION_STATE_H
#define REPETITION_STATE_H

#include <thread>
#include <cmath>
#include <chrono>
#include "timer_set_state.h"

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
                    /// @brief Server's or client's service repetition state
                    /// @tparam T Server's or client state enumeration type
                    /// @note The state is not copyable
                    template <typename T>
                    class RepetitionState : public TimerSetState<T>
                    {
                    protected:
                        /************************ extra attributes ************************/

                        const int RepetitionsMax;        // Maximum iteration in repetition phase
                        const int RepetitionsBaseDelay;  // Repetition iteration delay in milliseconds



                        /************************* function that parent needs ************/

                        virtual void SetTimer() override
                        {
                            for (uint32_t i = 0; i < RepetitionsMax; ++i)
                            {
                                int _doubledDelay = std::pow(2, i) * RepetitionsBaseDelay;
                                auto _delay = std::chrono::milliseconds(_doubledDelay);
                                
                                bool _interrupted = this->WaitFor(_delay);
                                if (_interrupted)
                                {
                                    break;
                                }
                                else
                                {
                                    // Invoke the on timer expiration callback
                                    this->OnTimerExpired();
                                }
                            }
                        }

                    public:
                        /****************************** constructor ********************************/

                        /// @brief Constructor
                        /// @param currentState Current state at repetition phase
                        /// @param nextState Next state after repetition phase expiration
                        /// @param stoppedState Stopped state after put a stop to the service
                        /// @param onTimerExpired Delegate to be invoked by timer's thread when the timer is expired
                        /// @param repetitionsMax Maximum iteration in repetition phase
                        /// @param repetitionsBaseDelay Repetition iteration delay in milliseconds
                        RepetitionState(
                            T currentState,
                            T nextState,
                            T stoppedState,
                            std::function<void()> onTimerExpired,
                            uint32_t repetitionsMax,
                            int repetitionsBaseDelay) : helper::State<T>(currentState),
                                                        TimerSetState<T>(nextState, stoppedState, onTimerExpired),
                                                        RepetitionsMax{static_cast<int>(repetitionsMax)},
                                                        RepetitionsBaseDelay{repetitionsBaseDelay}
                        {
                            if (repetitionsBaseDelay < 0)
                            {
                                throw std::invalid_argument("Invalid repetition base delay.");
                            }
                        }



                        /****** disable emptyConstructor-copyConstructor-copyAssigmentOperator *****/

                        RepetitionState() = delete;
                        RepetitionState(const RepetitionState &) = delete;
                        RepetitionState &operator=(const RepetitionState &) = delete;
                    };
                }
            }
        }
    }
}

#endif