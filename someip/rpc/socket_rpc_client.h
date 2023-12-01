#ifndef SOCKET_RPC_CLIENT_H
#define SOCKET_RPC_CLIENT_H

#include "../../sockets/include/poller.h"
#include "../../sockets/include/tcp_client.h"
#include "../../helper/concurrent_queue.h"
#include "./rpcs_requester.h"

namespace ara
{
    namespace com
    {
        namespace someip
        {
            namespace rpc
            {
                /// @brief TCP socket-based RPC client
                class SocketRpcClient : public RpcsRequester
                {
                private:
                    /*********************** poller attributes  *******************/

                    static const size_t cBufferSize{256};
                    helper::ConcurrentQueue<std::vector<uint8_t>> mSendingQueue;
                    AsyncBsdSocketLib::Poller *const mPoller;
                    AsyncBsdSocketLib::TcpClient mClient;



                    /********************** poller functions  *********************/

                    void onSend();
                    void onReceive();



                    /******************** function that parent need *****************/

                    virtual void Send(const std::vector<uint8_t> &payload) override;



                public:
                    /******************* constructor  *******************************/

                    /// @brief Constructor
                    /// @param poller BSD sockets poller
                    /// @param ipAddress RPC server IP address
                    /// @param port RPC server listening TCP port number
                    /// @param protocolVersion SOME/IP protocol header version
                    /// @param interfaceVersion Service interface version
                    /// @throws std::runtime_error Throws when the TCP client socket configuration failed
                    SocketRpcClient(
                        AsyncBsdSocketLib::Poller *poller,
                        std::string ipAddress,
                        uint16_t port,
                        uint8_t protocolVersion,
                        uint8_t interfaceVersion = 1);



                    /**************** override deconstructor  *********************/

                    virtual ~SocketRpcClient() override;
                };
            }
        }
    }
}

#endif