#ifndef SOCKET_FIELD_SERVER_H
#define SOCKET_FIELD_SERVER_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/udp_client.h"
#include "../../helper/concurrent_queue.h"
#include "field_provider.h"

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
                using HandleType = std::function<void(rpc::SomeIpRpcMessage)>;

                class SockeKFieldServer : public pubsub::FieldServer
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
    
                    virtual void Send(const std::vector<uint8_t> &payload);


                public:
                    /******************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param nicIpAddress Network interface controller IPv4 address
                    /// @param multicastGroup Multicast group IPv4 address
                    /// @param port Multicast UDP port number
                    /// @throws std::runtime_error Throws when the UDP socket configuration failed
                    SockeKFieldServer(
                        uint16_t serviceId,
                        uint16_t instanceId,
                        uint8_t majorVersion,
                        uint16_t eventgroupId, 
                        AsyncBsdSocketLib::Poller *poller,
                        std::string nicIpAddress,
                        std::string multicastGroup,
                        uint16_t port,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);



                    /**************** override deconstructor  *********************/

                    virtual ~SockeKFieldServer() override;
                };
            }
        }
    }
}

#endif