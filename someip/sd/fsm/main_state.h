#ifndef MAIN_STATE_H
#define MAIN_STATE_H

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
                    /// @brief Server's main state
                    /// @note The state is not copyable
                    class MainState : public TimerSetState<helper::SdServerState>
                    {
                    private:
                        /******************** extra attributes **************************/

                        const std::chrono::milliseconds mCyclicOfferDelay;


                    protected:
                        /************************* function that parent needs ************/

                        void SetTimer() override
                        {
                            while (true)
                            {
                                bool _interrupted = this->WaitFor(mCyclicOfferDelay);

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
                        /*********************** constructor *****************************/

                        /// @brief Constructor
                        /// @param cyclicOfferDelay Interval in milliseconds to offer the service
                        /// @param onTimerExpired Delegate to be invoked by timer's thread when the timer is expired
                        MainState( std::function<void()> onTimerExpired, int cyclicOfferDelay) :
                                        helper::State<helper::SdServerState>
                                        (
                                            helper::SdServerState::MainPhase
                                        ),
                                        TimerSetState<helper::SdServerState>
                                        (
                                            helper::SdServerState::NotReady,
                                            helper::SdServerState::NotReady,
                                            onTimerExpired
                                        ),
                                        mCyclicOfferDelay{cyclicOfferDelay}
                        {
                            if (cyclicOfferDelay < 0)
                            {
                                throw std::invalid_argument("Invalid cyclic offer delay.");
                            }
                        }



                        /****** disable emptyConstructor-copyConstructor-copyAssigmentOperator *****/

                        MainState() = delete;
                        MainState(const MainState &) = delete;
                        MainState &operator=(const MainState &) = delete;
                    };
                }
            }
        }
    }
}

#endif