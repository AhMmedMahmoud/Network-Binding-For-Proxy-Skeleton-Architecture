#ifndef PROVIDER_H
#define PROVIDER_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/udp_client.h"

#include "../../helper/concurrent_queue.h"
#include "../../helper/ipv4_address.h"
#include "../../helper/fsm.h"

#include "../someipSdMsg/someip_sd_message.h"
#include "../someipSdMsg/entry/service_entry.h"
#include "../someipSdMsg/entry/eventgroup_entry.h"
#include "../someipSdMsg/option/ipv4_endpoint_option.h"

#include "fsm/service_down_state.h"
#include "fsm/notsubscribed_state.h"
#include "fsm/subscribed_state.h"

#include "../rpc/socket_rpc_server.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            /// @brief SOME/IP service discovery namespace
            /// @note The namespace is not part of the ARA standard.
            namespace sd
            {
                using HandleType = std::function<void(sd::SomeIpSdMessage)>;
                using HandlerTypeFunc = std::function<bool(const std::vector<uint8_t> &, std::vector<uint8_t> &)>;

                class Provider
                {
                private:
                    rpc::SocketRpcServer *rpcServer;

                    /******************* attributes ********************************/

                    uint16_t mServiceId;
                    uint16_t mInstanceId;
                    uint8_t mMajorVersion;
                    uint8_t mMinorVersion;
                    uint16_t mEventgroupId;
                    //helper::Ipv4Address mEndpointIp;
                    uint16_t mEndpointPort;

                    const uint8_t mProtocolVersion;
                    const uint8_t mInterfaceVersion;

                    const uint16_t cSumationOverVectorMethodId = 1000;
                    const uint16_t cMultiplicationOverVectorMethodID = 2000;

                    /********** Finite State Machine for Event Provider  ***********/

                    helper::FSM<helper::PubSubState> mStateMachine;
                    pubsub::fsm::ServiceDownState mServiceDownState;
                    pubsub::fsm::NotSubscribedState mNotSubscribedState;
                    pubsub::fsm::SubscribedState mSubscribedState;

                    /*********************** poller attributes  ********************/

                    static const size_t cBufferSize{256};
                    helper::ConcurrentQueue<std::vector<uint8_t>> mSendingQueue;
                    AsyncBsdSocketLib::Poller *const mPoller;
                    AsyncBsdSocketLib::UdpClient mUdpSocket;

                    const std::string cNicIpAddress;
                    const std::string cMulticastGroup;
                    const uint16_t cPort;
                    static const std::string cAnyIpAddress;
                    

                    
                    /********************** poller functions  *********************/

                    void onReceive();
                    void onSend();
                    void Send(const SomeIpSdMessage &message);   

                    
                    
                    /******************** internal functions *****************/
                        
                    void InvokeEventHandler(sd::SomeIpSdMessage &&message);

                    void processEntry(const entry::EventgroupEntry *entry);

                    void printCurrentState();

                    

                public:
                    /******************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    Provider(
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint8_t minorVersion,
                        uint16_t eventgroupId, 
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port,
                        uint16_t endpointPort,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);



                    /************************ fundemental functions *********************/

                    /// @brief Start the server
                    void Start();

                    /// @brief Get the current server state
                    /// @returns Server machine state
                    helper::PubSubState GetState() const noexcept;

                    /// @brief Stop the server
                    void Stop();

                    void update();

                    void offerService(helper::Ipv4Address ipAddress);

                    void init();

                    /**************** override deconstructor  *********************/

                    ~Provider();    //override;
                };
            }
        }
    }
}

#endif