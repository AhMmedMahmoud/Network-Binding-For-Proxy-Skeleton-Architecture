#ifndef SOMEIP_PUBSUB_SERVER
#define SOMEIP_PUBSUB_SERVER

#include "../someipSdMsg/someip_sd_message.h"
#include "../someipSdMsg/entry/eventgroup_entry.h"
#include "../someipSdMsg/option/ipv4_endpoint_option.h"
#include "../../helper/fsm.h"
#include "fsm/service_down_state.h"
#include "fsm/notsubscribed_state.h"
#include "fsm/subscribed_state.h"


namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace pubsub
            {
                using HandleType = std::function<void(sd::SomeIpSdMessage)>;

                /// @brief SOME/IP event porivder
                class EventProvider
                {
                private:
                    /******************************* attributes ******************************/

                    uint16_t mServiceId;
                    uint16_t mInstanceId;
                    uint8_t mMajorVersion;
                    uint16_t mEventgroupId;
                    //helper::Ipv4Address mEndpointIp;
                    //uint16_t mEndpointPort;


                    helper::FSM<helper::PubSubState> mStateMachine;
                    fsm::ServiceDownState mServiceDownState;
                    fsm::NotSubscribedState mNotSubscribedState;
                    fsm::SubscribedState mSubscribedState;



                    /****************** useful in InvokeEventHandler method *****************/
                    
                    void processEntry(const entry::EventgroupEntry *entry);

                    void printCurrentState();
                    
                protected:
                    /****************** function that child will implement it *****************/

                    virtual void Send(const sd::SomeIpSdMessage &message) = 0;
                    
                    /******************** function take any someip/sd message *****************/

                    void InvokeEventHandler(sd::SomeIpSdMessage &&message);

                public:
                    /******************************* constructor ******************************/

                    /// @param networkLayer Network communication abstraction layer
                    /// @param serviceId Service ID
                    /// @param instanceId Service instance ID
                    /// @param majorVersion Service major version
                    /// @param eventgroupId Service event-group ID
                    /// @param ipAddress Multicast IP address that clients should listen to for receiving events
                    /// @param port Multicast port number that clients should listen to for receiving events
                    EventProvider( uint16_t serviceId,
                                    uint16_t instanceId,
                                    uint8_t majorVersion,
                                    uint16_t eventgroupId);



                    /******************************* fundemental functions *********************/

                    /// @brief Start the server
                    void Start();

                    /// @brief Get the current server state
                    /// @returns Server machine state
                    helper::PubSubState GetState() const noexcept;

                    /// @brief Stop the server
                    void Stop();

                    void update();
                    
                    
                    /******************************* destructor ******************************/

                    virtual ~EventProvider();
                };
            }
        }
    }
}

#endif