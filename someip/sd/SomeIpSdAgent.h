#ifndef SOMEIP_SD_AGENT_H
#define SOMEIP_SD_AGENT_H

#include <future>
#include "../../helper/fsm.h"
#include "../../helper/abstract_network_layer.h"
#include "../someipSdMsg/someip_sd_message.h"
#include <iostream>

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                /// @brief SOME/IP service discovery agent (i.e., a server or a client)
                /// @tparam T Agent state enumeration type
                template <typename T>
                class SomeIpSdAgent
                {
                protected:
                    /***************************** for my child ******************************/

                    /// @brief Agent's FSM
                    helper::FSM<T> StateMachine;
                    /// @brief Agent running state future object
                    std::future<void> Future;
                    /// @brief Network communication abstraction layer
                    helper::AbstractNetworkLayer<SomeIpSdMessage> *CommunicationLayer;



                   /****************** functions that my child will implement *****************/

                    /// @brief Start the service discovery agent
                    /// @param state Current FSM state before start
                    virtual void StartAgent(T state) = 0;

                    /// @brief Stop the service discovery agent
                    /// @param state Current FSM state before stop
                    virtual void StopAgent() = 0;

                public:
                    /******************************* constructor  ******************************/

                    /// @brief Constructor
                    /// @param networkLayer Network communication abstraction layer
                    SomeIpSdAgent(helper::AbstractNetworkLayer<SomeIpSdMessage> *networkLayer) : CommunicationLayer{networkLayer}
                    {}



                    /******************************* fundemental functions  *********************/
                    
                    /// @brief Join to the timer's thread
                    void Join()
                    {
                        // If the future is valid, block unitl its result becomes avialable after the timer expiration.
                        if (Future.valid())
                        {
                            Future.get();
                        }
                    }

                    /// @brief Start the service discovery agent
                    void Start()
                    {
                        std::cout << "Start is called\n";

                        // Valid future means the timer is not expired yet.
                        /*
                            first time you call it Future isnot valid 
                            So, startAgent will execute to start FSM by making thread
                            and Future is used with this thread
                        */
                        if (Future.valid())
                        {
                            throw std::logic_error("The state has been already activated.");
                        }
                        else
                        {
                            std::cout << "StartAgent is called\n";
                            StartAgent(GetState());
                        }
                    }

                    /// @brief Stop the service discovery agent
                    /// @note It is safe to recall the function if the agent has been already stopped.
                    void Stop()
                    {
                        StopAgent();
                    }



                    /********************************** getters *********************************/

                    /// @brief Get the current server state
                    /// @returns Server machine state
                    T GetState() const noexcept
                    {
                        return StateMachine.GetState();
                    }



                    /****************** tell compiler to generate default desctructor  ********************/

                    virtual ~SomeIpSdAgent() noexcept = default;
                };
            }
        }
    }
}

#endif