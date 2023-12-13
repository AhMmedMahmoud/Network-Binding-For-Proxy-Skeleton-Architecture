#ifndef TIMER_SET_STATE_H
#define TIMER_SET_STATE_H

#include <stdexcept>
#include "../../../helper/state.h"
#include <thread>
#include <chrono>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                /// @brief Finite state machine
                /// @note The namespace is not part of the official AUTOSAR standard.
                namespace fsm
                {
                    /// @brief abstract class represents timer set state
                    /// @tparam T Server's or client state enumeration type
                    /// @note The state is not copyable
                    template <typename T>
                    class TimerSetState : virtual public helper::State<T>
                    {
                        /****************** inherited private attributes **************************
                        const T mState;
                        AbstractStateMachine<T> *mFSM;

                        generic for any state
                        ---------------------
                        Transit    : uses the FSM at which it related to for making transit to new state and activate it 
                        Register   : set FSM at which it related to (used in FSM->initilize() )
                        GetState   : get value of enum that represent the state
                        particular for Initail Wait State
                        -----------------------------------
                        Activate   : call waitFor (some logic) transit to nextState or Stopped state and activate it  
                        Deactivate : reset internal variables mStopped, mInterrupted
                        WaitFor    : make delay for certain time   
                        ***************************************************************************/
                       
                    private:
                        /************************** extra attributes ******************************/
                        T mNextState;
                        const T mStoppedState;
                        bool mStopped;
                        bool mInterrupted;

                    protected:
                        /********************** for my child *************************************/

                        /// @brief Wait for certian period of time
                        /// @param duration Waiting duration
                        /// @returns True if waiting is interrupted; otherwise false if timeout occurs
                        bool WaitFor(std::chrono::milliseconds duration)
                        {
                            std::this_thread::sleep_for(duration);
                            bool _result = mStopped || mInterrupted;
                            return _result;
                        }

                        /// @brief Wait for certian period of time
                        /// @param duration Waiting duration in milliseconds
                        /// @returns True if waiting is interrupted; otherwise false if timeout occurs
                        bool WaitFor(int duration)
                        {
                            auto _milliseconds = std::chrono::milliseconds(duration);
                            bool _result = WaitFor(_milliseconds);
                            return _result;
                        }

                        /// @brief Interrupt the timer
                        /// @remark If the timer is interrupted, it should transit to the next state.
                        void Interrupt() noexcept
                        {
                            mInterrupted = true;
                        }

                        /// @brief Delegate which is invoked by timer's thread when the timer is expired
                        const std::function<void()> OnTimerExpired;



                        /**************************** virtual fuction *************************/

                        /// @brief Set the phase time on state activation
                        virtual void SetTimer() = 0;



                        /******************************** constructor **********************/


                        /// @brief Constructor
                        /// @param nextState Next state after initial wait phase expiration
                        /// @param stoppedState Default stopped state after put a stop to the service
                        /// @param onTimerExpired Delegate to be invoked by timer's thread when the timer is expired
                        TimerSetState(
                            T nextState,
                            T stoppedState,
                            std::function<void()> onTimerExpired) : mNextState{nextState},
                                                                    mStoppedState{stoppedState},
                                                                    OnTimerExpired{onTimerExpired},
                                                                    mStopped{false},
                                                                    mInterrupted{false}
                        {}



                        /****************** override functions inherited from parent ****************/

                        void Deactivate(T nextState) override
                        {
                            // Reset 'service interrupted' flag
                            mInterrupted = false;
                            // Reset 'service stopped' flag
                            mStopped = false;
                        }

                    public:
                        /****************** override functions inherited from parent ****************/

                        virtual void Activate(T previousState) override
                        {
                            if (!mStopped)
                            {
                                SetTimer();
                                // Transition to the next state or to the stopped state
                                if (mStopped)
                                {
                                    helper::State<T>::Transit(mStoppedState);
                                }
                                else
                                {
                                    helper::State<T>::Transit(mNextState);
                                }
                            }
                        }



                        /***************************** fundemental function *************************/

                        /// @brief Inform the state that the server's service is stopped
                        void ServiceStopped() noexcept
                        {
                            mStopped = true;
                        }

                        
                        
                        /********************************** setter ********************************/
                        
                        /// @brief Set next state
                        /// @param nextState New next state
                        void SetNextState(T nextState) noexcept
                        {
                            mNextState = nextState;
                        }



                        /*********** disable copy constructor and copy assigment operator *********/
                        
                        TimerSetState(const TimerSetState &) = delete;
                        TimerSetState &operator=(const TimerSetState &) = delete;



                        /************************ override deconstructor ***************************/

                        virtual ~TimerSetState() override
                        {
                            // Set a fake stop signal, otherwise the timer loop may never end (e.g., in the main phase).
                            ServiceStopped();
                        }
                    };
                }
            }
        }
    }
}

#endif