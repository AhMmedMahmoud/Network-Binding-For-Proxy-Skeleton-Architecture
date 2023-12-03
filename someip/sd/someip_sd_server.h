#ifndef SOMEIP_SD_SERVER
#define SOMEIP_SD_SERVER

#include "../../helper/concurrent_queue.h"
#include "../../helper/ipv4_address.h"
#include "../someipSdMsg/entry/service_entry.h"
#include "../someipSdMsg/option/ipv4_endpoint_option.h"
#include "./fsm/notready_state.h"
#include "./fsm/initial_wait_state.h"
#include "./fsm/repetition_state.h"
#include "./fsm/main_state.h"
#include "./someip_sd_agent.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace sd
            {
                /// @brief SOME/IP service discovery server
                class SomeIpSdServer : public SomeIpSdAgent<helper::SdServerState>
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
                    const uint16_t mInstanceId;
                    const uint8_t mMajorVersion;
                    const uint32_t mMinorVersion;



                    helper::ConcurrentQueue<SomeIpSdMessage> mMessageBuffer;

                    SomeIpSdMessage mOfferServiceMessage;
                    SomeIpSdMessage mStopOfferMessage;
                    
                    
                    fsm::NotReadyState mNotReadyState;
                    fsm::InitialWaitState<helper::SdServerState> mInitialWaitState;
                    fsm::RepetitionState<helper::SdServerState> mRepetitionState;
                    fsm::MainState mMainState;


                    bool hasFindingEntry(const SomeIpSdMessage &message) const;

                    void sendOffer();
                    void receiveFind(SomeIpSdMessage &&message);
                    void onServiceStopped();

                protected:
                    /******************** function that parent need *****************/

                    void StartAgent(helper::SdServerState state) override;
                    void StopAgent() override;

                public:
                    /******************************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param networkLayer Network communication abstraction layer
                    /// @param serviceId Service ID
                    /// @param instanceId Service instance ID
                    /// @param majorVersion Service major version
                    /// @param minorVersion Service minor version
                    /// @param ipAddress Service unicast endpoint IP Address
                    /// @param port Service unicast endpoint TCP port number
                    /// @param initialDelayMin Minimum initial delay
                    /// @param initialDelayMax Maximum initial delay
                    /// @param repetitionBaseDelay Repetition phase delay
                    /// @param cycleOfferDelay Cycle offer delay in the main phase
                    /// @param repetitionMax Maximum message count in the repetition phase
                    SomeIpSdServer(
                        helper::NetworkLayer<SomeIpSdMessage> *networkLayer,
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint32_t minorVersion,
                        helper::Ipv4Address ipAddress,
                        uint16_t port,
                        int initialDelayMin,
                        int initialDelayMax,
                        int repetitionBaseDelay = 30,
                        int cycleOfferDelay = 1000,
                        uint32_t repetitionMax = 3);



                    /********************************** disable empty constructor  ********************/

                    SomeIpSdServer() = delete;



                    /****************** override desctructor  ********************/

                    ~SomeIpSdServer() override;
                };
            }
        }
    }
}

#endif