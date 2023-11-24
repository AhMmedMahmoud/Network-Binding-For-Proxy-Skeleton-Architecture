cmake_minimum_required(VERSION 3.14)

project(demo)

set(CMAKE_CXX_STANDARD 14)


add_library(
   araCom
   #### someip message ####
   someip/someip_message.h
   someip/someip_message.cpp

   #### someip_rpc_message ####
   someip/rpc/someip_rpc_message.cpp
   someip/rpc/someip_rpc_message.h

   #### sender and receiver for someip_rpc_message ####
   someip/rpc/rpc_server.h
   someip/rpc/rpc_server.cpp
   someip/rpc/socket_rpc_server.h
   someip/rpc//socket_rpc_server.cpp


   someip/rpc/rpc_client.h
   someip/rpc/rpc_client.cpp
   someip/rpc/socket_rpc_client.h
   someip/rpc/socket_rpc_client.cpp

   #### helper ####
   helper/abstract_state_machine.h
   helper/concurrent_queue.h
   helper/finite_state_machine.h
   helper/ipv4_address.h
   helper/ipv4_address.cpp
   helper/machine_state.h
   helper/network_layer.h
   helper/payload_helper.h
   helper/payload_helper.cpp
   helper/ttl_timer.h
   helper/ttl_timer.cpp

   ##### sockets #####
   asyncbsdsocket/include/communicator.h
   asyncbsdsocket/include/fifo_receiver.h
   asyncbsdsocket/include/fifo_sender.h
   asyncbsdsocket/include/network_socket.h
   asyncbsdsocket/include/poller.h
   asyncbsdsocket/include/tcp_client.h
   asyncbsdsocket/include/tcp_listener.h
   asyncbsdsocket/include/udp_client.h

   asyncbsdsocket/src/communicator.cpp
   asyncbsdsocket/src/fifo_receiver.cpp
   asyncbsdsocket/src/fifo_sender.cpp
   asyncbsdsocket/src/network_socket.cpp
   asyncbsdsocket/src/poller.cpp
   asyncbsdsocket/src/tcp_client.cpp
   asyncbsdsocket/src/tcp_listener.cpp
   asyncbsdsocket/src/udp_client.cpp
)

add_executable(server samples/server.cpp)
target_link_libraries(server araCom)


add_executable(client samples/client.cpp)
target_link_libraries(client araCom)