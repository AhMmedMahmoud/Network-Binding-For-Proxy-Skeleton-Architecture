#ifndef SOMEIP_SD_CLIENT
#define SOMEIP_SD_CLIENT

#include "../../core/optional.h"
#include "../../helper/ipv4_address.h"
#include "../../helper/ttl_timer.h"
#include "../someipSdMsg/entry/service_entry.h"

#include "fsm/fsmClient/service_notseen_state.h"
#include "fsm/fsmClient/service_seen_state.h"
#include "fsm/fsmClient/client_initial_wait_state.h"
#include "fsm/fsmClient/client_repetition_state.h"
#include "fsm/fsmClient/service_ready_state.h"
#include "fsm/fsmClient/stopped_state.h"
#include "SomeIpSdAgent.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                /// @brief SOME/IP service discovery client
                class SomeIpSdClient : public SomeIpSdAgent<helper::SdClientState>
                {
                    /********************* inherited ****************************
                    private attributes
                    ------------------
                    helper::FSM<T> StateMachine;
                    std::future<void> Future;
                    helper::NetworkLayer<SomeIpSdMessage> *CommunicationLayer;
                    
                    public functions
                    ----------------
                    Start :  Start the service discovery agent
                    Stop  :  stop the service discovery agent
                    Join  :  Join to the timer's thread
                    ************************************************************/
                private:
                    /******************************* extra attributes ******************************/

                    const uint16_t mServiceId;



                    helper::TtlTimer mTtlTimer;
                    bool mValidState;

                    ////// mutex for offering
                    std::mutex mOfferingMutex;
                    std::unique_lock<std::mutex> mOfferingLock;
                    std::condition_variable mOfferingConditionVariable;
                    
                    ////// mutex for stop offering
                    std::mutex mStopOfferingMutex;
                    std::unique_lock<std::mutex> mStopOfferingLock;
                    std::condition_variable mStopOfferingConditionVariable;

                    ////// mutex for Endpoint
                    std::mutex mEndpointMutex;
                    std::unique_lock<std::mutex> mEndpointLock;

                    ////// fsm
                    fsm::ServiceNotseenState mServiceNotseenState;
                    fsm::ServiceSeenState mServiceSeenState;
                    fsm::ClientInitialWaitState mInitialWaitState;
                    fsm::ClientRepetitionState mRepetitionState;
                    fsm::StoppedState mStoppedState;
                    fsm::ServiceReadyState mServiceReadyState;

                    ////// message to send
                    SomeIpSdMessage mFindServieMessage;

                    ////// getting ip address and port number                    
                    core::Optional<std::string> mOfferedIpAddress;
                    core::Optional<uint16_t> mOfferedPort;



                    /**************************** useful in constructor *********************/

                    void sendFind();
                    
                    

                    /******************** function take any someip/sd message *****************/

                    void InvokeHandler(SomeIpSdMessage &&message);
                    
                    

                    /**************************** useful in InvokeHandler *********************/
                    
                    bool hasOfferingEntry(const SomeIpSdMessage &message, uint32_t &ttl) const;

                    bool tryExtractOfferedEndpoint(
                        const SomeIpSdMessage &message,
                        std::string &ipAddress,
                        uint16_t &port) const;
                    
                    
                protected:
                    /******************** function that parent need *****************/

                    void StartAgent(helper::SdClientState state) override;
                    void StopAgent() override;

                public:
                    /******************************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param networkLayer Network communication abstraction layer
                    /// @param serviceId Server's service ID
                    /// @param initialDelayMin Minimum initial delay
                    /// @param initialDelayMax Maximum initial delay
                    /// @param repetitionBaseDelay Repetition phase delay
                    /// @param repetitionMax Maximum message count in the repetition phase
                    SomeIpSdClient(
                        helper::AbstractNetworkLayer<SomeIpSdMessage> *networkLayer,
                        uint16_t serviceId,
                        int initialDelayMin,
                        int initialDelayMax,
                        int repetitionBaseDelay = 30,
                        uint32_t repetitionMax = 3);



                    /************************* fundemental functions  ********************************/

                    /// @brief Try to wait unitl the server offers the service
                    /// @param duration Waiting timeout in milliseconds
                    /// @returns True, if the service is offered before the timeout; otherwise false
                    /// @note Zero duration means wait until the service is offered.
                    bool TryWaitUntiServiceOffered(int duration);

                    /// @brief Try to wait unitl the server stops offering the service
                    /// @param duration Waiting timeout in milliseconds
                    /// @returns True, if the service offering is stopped before the timeout; otherwise false
                    /// @note Zero duration means wait until the service offering stops.
                    bool TryWaitUntiServiceOfferStopped(int duration);

                    /// @brief Try to the offered unicast endpoint from the SD server
                    /// @param[out] ipAddress Offered unicast IPv4 address
                    /// @param[out] port Offered TCP port number
                    /// @return True if the SD server has already offered the endpoint; otherwise false
                    /// @remark The arguments won't be touched while returning 'false'.
                    /// @note The endpoint WON'T be invalidated after receiving the stop offer.
                    bool TryGetOfferedEndpoint(std::string &ipAddress, uint16_t &port);



                    /********************************** disable empty constructor  ********************/

                    SomeIpSdClient() = delete;



                    /****************** override desctructor  ********************/

                    ~SomeIpSdClient() override;
                };
            }
        }
    }
}

#endif