#ifndef SOMEIP_SD_CLIENT
#define SOMEIP_SD_CLIENT

#include "../../core/optional.h"
#include "../../helper/ipv4_address.h"
#include "../../helper/ttl_timer.h"
#include "../someipSdMsg/entry/service_entry.h"
#include "./fsm/service_notseen_state.h"
#include "./fsm/service_seen_state.h"
#include "./fsm/client_initial_wait_state.h"
#include "./fsm/client_repetition_state.h"
#include "./fsm/service_ready_state.h"
#include "./fsm/stopped_state.h"
#include "./someip_sd_agent.h"

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
                /*
                    ********** inherited : private ****************************

                    helper::FiniteStateMachine<T> StateMachine;
                    std::future<void> Future;
                    helper::NetworkLayer<SomeIpSdMessage> *CommunicationLayer;

                    ********** inherited  : public ****************************

                    /// @brief Join to the timer's thread
                    void Join()
                    {
                        // If the future is valid, block unitl its result becomes
                        // avialable after the timer expiration.
                        if (Future.valid())
                        {
                            Future.get();
                        }
                    }

                    /// @brief Start the service discovery agent
                    void Start()
                    {
                        // Valid future means the timer is not expired yet.
                        if (Future.valid())
                        {
                            throw std::logic_error("The state has been already activated.");
                        }
                        else
                        {
                            StartAgent(GetState());
                        }
                    }

                    void Stop() {   StopAgent();    }
                */
                private:
                    /******************************* extra attributes ******************************/

                    const uint16_t mServiceId;



                    helper::TtlTimer mTtlTimer;
                    bool mValidState;

                    std::mutex mOfferingMutex;
                    std::unique_lock<std::mutex> mOfferingLock;
                    std::condition_variable mOfferingConditionVariable;
                    
                    std::mutex mStopOfferingMutex;
                    std::unique_lock<std::mutex> mStopOfferingLock;
                    std::condition_variable mStopOfferingConditionVariable;

                    fsm::ServiceNotseenState mServiceNotseenState;
                    fsm::ServiceSeenState mServiceSeenState;
                    fsm::ClientInitialWaitState mInitialWaitState;
                    fsm::ClientRepetitionState mRepetitionState;
                    fsm::StoppedState mStoppedState;
                    fsm::ServiceReadyState mServiceReadyState;

                    SomeIpSdMessage mFindServieMessage;

                    std::mutex mEndpointMutex;
                    std::unique_lock<std::mutex> mEndpointLock;
                    
                    core::Optional<std::string> mOfferedIpAddress;
                    core::Optional<uint16_t> mOfferedPort;



                    void sendFind();
                    bool hasOfferingEntry(const SomeIpSdMessage &message, uint32_t &ttl) const;
                    void receiveSdMessage(SomeIpSdMessage &&message);
                    bool tryExtractOfferedEndpoint(
                        const SomeIpSdMessage &message, std::string &ipAddress, uint16_t &port) const;
                    
                    
                    void onOfferChanged(uint32_t ttl);

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
                        helper::NetworkLayer<SomeIpSdMessage> *networkLayer,
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