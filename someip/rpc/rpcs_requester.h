#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include <functional>
#include <map>
#include <stdint.h>
#include <vector>
#include "../someipRpcMsg/someip_rpc_message.h"
#include <future>
#include "../../helper/concurrent_queue.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                /// @brief SOME/IP RPC response handler type
                using HandlerType = std::function<void(const SomeIpRpcMessage &)>;

                /// @brief SOME/IP RPC abstract client
                class RpcsRequester
                {
                //public:

                private:
                    /**************************  attributes ******************************/

                    const uint8_t mProtocolVersion;
                    const uint8_t mInterfaceVersion;


                    /************ hash table to store session id for each message id*********/
                    std::map<uint32_t, uint16_t> mSessionIds;


                    /************ hash table to store handler for each message id*********/
                    std::map<uint32_t, HandlerType> mHandlers;


                    
                    // queue at which we get received messages that contained acknowledging entry
                    helper::ConcurrentQueue<SomeIpRpcMessage> mMessageBuffer;

                    // mutex object
                    std::mutex mMutex;
                    // trier to take mutex
                    std::unique_lock<std::mutex> mLock;
                    std::condition_variable mConditionVariable;
                    // flag
                    bool mValidNotify;
                    

                protected:
                    /*********************** useful for constructor of my child *****************/

                    /// @brief Constructor
                    /// @param protocolVersion SOME/IP protocol header version
                    /// @param interfaceVersion Service interface version
                    RpcsRequester(uint8_t protocolVersion, uint8_t interfaceVersion) noexcept;


                    /**************************** useful for my child *************************/

                    /// @brief Invoke corresponding response handler at a message reception
                    /// @param payload Serialized SOME/IP response payload byte vector
                    void InvokeHandler(const std::vector<uint8_t> &payload);



                    /****************** function that child will implement it *****************/

                    /// @brief Send a SOME/IP request to the RPC server
                    /// @param payload Serialized SOME/IP request payload byte vector
                    virtual void Send(const std::vector<uint8_t> &payload) = 0;




                public:
                   /**************************** fundemental functions *************************/

                    /// @brief Set a RPC response handler
                    /// @param serviceId Service ID that contains the requested method
                    /// @param methodId Requested method ID for invocation
                    /// @param handler Handler to be invoked at the response arrival
                    void SetHandler(uint16_t serviceId, uint16_t methodId, HandlerType handler);

                    /// @brief Send a request to the RPC server
                    /// @param serviceId Service ID that contains the requested method
                    /// @param methodId Requested method ID for invocation
                    /// @param clientId Client ID that sends the request
                    /// @param rpcPayload Serialized RPC request payload byte vector
                    void Request(
                        uint16_t serviceId,
                        uint16_t methodId,
                        uint16_t clientId,
                        const std::vector<uint8_t> &rpcPayload);

                    
                    std::future<bool> RequestWithoutHandler(
                    uint16_t serviceId,
                    uint16_t methodId,
                    uint16_t clientId,
                    const std::vector<uint8_t> &rpcPayload, std::vector<uint8_t> &data);
                    

                    /****************** tell compiler to generate default desctructor  ********************/

                    virtual ~RpcsRequester() noexcept = default;
                };
            }
        }
    }
}

#endif