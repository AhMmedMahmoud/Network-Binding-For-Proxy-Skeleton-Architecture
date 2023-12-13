#ifndef SOMEIP_PUBSUB_CLIENT
#define SOMEIP_PUBSUB_CLIENT

#include <condition_variable>
#include "../../helper/concurrent_queue.h"
#include "../someipSdMsg/someip_sd_message.h"
#include "../someipSdMsg/entry/eventgroup_entry.h"
#include <functional>


#include "../someipRpcMsg/someip_rpc_message.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace pubsub
            {
                using HandlerType = std::function<void(sd::SomeIpSdMessage)>;

                /// @brief SOME/IP event requester
                class EventRequester
                {
                private:
                    /******************************* attributes ******************************/
                    const uint8_t mProtocolVersion;
                    const uint8_t mInterfaceVersion;
                    const uint8_t mCounter;


                    /******************************* useful variables ************************/

                    // queue at which we get received messages that contained acknowledging entry
                    helper::ConcurrentQueue<sd::SomeIpSdMessage> mMessageBuffer;

                    // mutex object
                    std::mutex mSubscriptionMutex;
                    // trier to take mutex
                    std::unique_lock<std::mutex> mSubscriptionLock;
                    std::condition_variable mSubscriptionConditionVariable;
                    // flag
                    bool mValidNotify;



                protected:
                    /*********************** useful for constructor of my child *****************/
                    
                    /// @param networkLayer Network communication abstraction layer
                    /// @param counter Counter to make the client distinguishable among other subscribers
                    EventRequester(uint8_t counter, uint8_t protocolVersion, uint8_t interfaceVersion = 1);
                    

                    
                    /****************** function that child will implement it *****************/

                    virtual void Send(const sd::SomeIpSdMessage &message) = 0;



                    /******************** function take any someip/sd message *****************/

                    void InvokeEventHandler(sd::SomeIpSdMessage &&message);

                    //void InvokeEventHandler(rpc::SomeIpRpcMessage &&message);


                public:
                    /******************************* fundemental functions *********************/

                    /// @brief Subscribe to an event-group
                    /// @param serviceId Service in interest ID
                    /// @param instanceId Service in interest instance ID
                    /// @param majorVersion Service in interest major version
                    /// @param eventgroupId Event-group in interest ID
                    void RequestSubscribe(uint16_t serviceId, uint16_t instanceId, uint8_t majorVersion, uint16_t eventgroupId);

                    /// @brief Unsubscribe from a subscribed event-group
                    void RequestUnsubscribe(uint16_t serviceId, uint16_t instanceId, uint8_t majorVersion, uint16_t eventgroupId);
                    
                    /// @brief Try to wait unitl the server processes a subscription request
                    /// @param duration Waiting timeout in milliseconds
                    /// @param message The first processed subscription message in the buffer
                    /// @returns True, if the service offering is stopped before the timeout; otherwise false
                    bool TryGetProcessedSubscription(int duration, sd::SomeIpSdMessage &message);



                    /************************ disable empty constructor **********************/

                    EventRequester() = delete;
    


                    /******************************* destructor ******************************/

                    virtual ~EventRequester();
                };
            }
        }
    }
}

#endif