#ifndef SOCKET_PUBSUB_H
#define SOCKET_PUBSUB_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/udp_client.h"
#include "../../helper/concurrent_queue.h"
#include "../../helper/network_layer.h"
#include "../someipSdMsg/someip_sd_message.h"
#include "event_provider.h"

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

                class SockeKPubSubServer : public pubsub::EventProvider
                {
                private:
                    /*********************** poller attributes  *******************/

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

                    
                    
                    /******************** function that parent need *****************/
    
                    virtual void Send(const SomeIpSdMessage &message) override;



                public:
                    /******************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    SockeKPubSubServer(
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint16_t eventgroupId, 
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port);



                    /**************** override deconstructor  *********************/

                    virtual ~SockeKPubSubServer() override;
                };
            }
        }
    }
}

#endif