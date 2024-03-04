#ifndef PROVIDER_EVENT_H
#define PROVIDER_EVENT_H

#include "../../../sockets/include/poller.h"
#include "../../../sockets/include/udp_client.h"
#include "../../../helper/concurrent_queue.h"
#include "../../../helper/ipv4_address.h"
#include "../../someipSdMsg/someip_sd_message.h"
#include "../../someipSdMsg/entry/service_entry.h"
#include "../../someipSdMsg/entry/eventgroup_entry.h"
#include "../../someipSdMsg/option/ipv4_endpoint_option.h"
//#include "../../rpc/socket_rpc_server.h"
#include "../../events/socket_event_server.h"
#include <algorithm>
#include <iostream>


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
                class EventServiceProvider
                {
                public:   
                    using Handler = std::function<bool(const std::vector<uint8_t> &)>;
                 
                    SockeKEventServer *eventServer;
                    
                
                private:
                    /******************* attributes ********************************/

                    uint16_t mServiceId;
                    uint16_t mInstanceId;
                    uint8_t mMajorVersion;
                    uint8_t mMinorVersion;
                    uint16_t mEventgroupId;
                    //helper::Ipv4Address mEndpointIp;
                    uint16_t mEndpointRpcsPort;
                    uint16_t mEndpointEventPort;

                    const uint8_t mProtocolVersion;
                    const uint8_t mInterfaceVersion;

                    /*********************** poller attributes  ********************/

                    static const size_t cBufferSize{256};
                    const std::string cAnyIpAddress = "0.0.0.0";
                    helper::ConcurrentQueue<std::vector<uint8_t>> mSendingQueue;
                    AsyncBsdSocketLib::Poller *const mPoller;
                    AsyncBsdSocketLib::UdpClient mUdpSocket;

                    const std::string cNicIpAddress;
                    const std::string cMulticastGroup;
                    const uint16_t cPort;
                                 
                    /********************** poller functions  *********************/

                    void onReceive();
                    void onSend();
                    void Send(const SomeIpSdMessage &message);   

                public:
                    /******************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    EventServiceProvider(
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint8_t minorVersion,
                        uint16_t eventgroupId, 
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port,
                        uint16_t endpointEventPort,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);


                    /************************ fundemental functions *********************/

                    void offerService();

                    void stopService();
                    
                    void init(Handler h = nullptr);

                    AsyncBsdSocketLib::Poller* getPoller();

                    /******************************  deconstructor  *********************/

                    ~EventServiceProvider();   
                };
            }
        }
    }
}

#endif