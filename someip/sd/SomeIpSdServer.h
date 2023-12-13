#ifndef SOMEIP_SD_SERVER
#define SOMEIP_SD_SERVER

#include "../../helper/concurrent_queue.h"
#include "../../helper/ipv4_address.h"

#include "../someipSdMsg/entry/service_entry.h"
#include "../someipSdMsg/option/ipv4_endpoint_option.h"

#include "fsm/notready_state.h"
#include "fsm/initial_wait_state.h"
#include "fsm/repetition_state.h"
#include "fsm/main_state.h"

#include "SomeIpSdAgent.h"

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
                
                protected:
                    /*************** function that parent need *****************/

                    /*
                    - first fuction should be called after creating instance of this class
                    - it start executing the FSM of Sd Server
                        - it should take SdServerState::NotReady
                        - then it transits from NotReady state to InitialWaitPhase state;
                    */
                    void StartAgent(helper::SdServerState state) override;
                    void StopAgent() override;


                private:
                    /********************** extra attributes *******************/
                    
                    //// service instance infp
                    const uint16_t mServiceId;
                    const uint16_t mInstanceId;
                    const uint8_t mMajorVersion;
                    const uint32_t mMinorVersion;



                    /********************* variables needed for logic **********/

                    //// buffer to put messages that we want to send                    
                    helper::ConcurrentQueue<SomeIpSdMessage> mMessageBuffer;

                    ////// messages to send
                    SomeIpSdMessage mOfferServiceMessage;
                    SomeIpSdMessage mStopOfferMessage;
                    
                    ////// fsm
                    fsm::NotReadyState mNotReadyState;
                    fsm::InitialWaitState<helper::SdServerState> mInitialWaitState;
                    fsm::RepetitionState<helper::SdServerState> mRepetitionState;
                    fsm::MainState mMainState;


                    
                    /********** function take any someip/sd message **************/

                    void InvokeHandler(SomeIpSdMessage &&message);
                    // useful in InvokeHandler 
                    bool hasFindingEntry(const SomeIpSdMessage &message) const;



                    /*********** useful in notReady state ************************/

                    void onServiceStopped();


                    /****************** useful in constructor *********************/

                    void sendOffer();

                public:
                    /***************** constructor  *******************************/

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
                        helper::AbstractNetworkLayer<SomeIpSdMessage> *networkLayer,

                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint32_t minorVersion,

                        helper::Ipv4Address ipAddress,
                        uint16_t port,

                        int initialDelayMin,
                        int initialDelayMax,
                        int repetitionBaseDelay = 2000,
                        int cycleOfferDelay = 2000,
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