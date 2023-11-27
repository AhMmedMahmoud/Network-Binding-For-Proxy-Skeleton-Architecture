cmake_minimum_required(VERSION 3.14)

project(demo)

set(CMAKE_CXX_STANDARD 14)

######################### source files ##################################
add_library(
   araCom

   ##### sockets #####
   asyncbsdsocket/include/communicator.h
   asyncbsdsocket/include/network_socket.h
   asyncbsdsocket/include/poller.h
   asyncbsdsocket/include/tcp_client.h
   asyncbsdsocket/include/tcp_listener.h
   asyncbsdsocket/include/udp_client.h
   asyncbsdsocket/src/communicator.cpp
   asyncbsdsocket/src/network_socket.cpp
   asyncbsdsocket/src/poller.cpp
   asyncbsdsocket/src/tcp_client.cpp
   asyncbsdsocket/src/tcp_listener.cpp
   asyncbsdsocket/src/udp_client.cpp

   #### entry ####
   entry/entry_deserializer.h
   entry/entry.cpp
   entry/entry.h
   entry/eventgroup_entry.cpp
   entry/eventgroup_entry.h
   entry/service_entry.cpp
   entry/service_entry.h

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

   #### option ####
   option/ipv4_endpoint_option.cpp
   option/ipv4_endpoint_option.h
   option/loadbalancing_option.cpp
   option/loadbalancing_option.h
   option/option_deserializer.h
   option/option.cpp
   option/option.h

   #### someip message ####
   someip/someip_message.h
   someip/someip_message.cpp

   #### someip_rpc_message ####
   someip/rpc/someip_rpc_message.cpp
   someip/rpc/someip_rpc_message.h

   #### receiver for someip_rpc_message ####
   someip/rpc/rpc_server.h
   someip/rpc/rpc_server.cpp
   someip/rpc/socket_rpc_server.h
   someip/rpc//socket_rpc_server.cpp

   #### sender for someip_rpc_message ####
   someip/rpc/rpc_client.h
   someip/rpc/rpc_client.cpp
   someip/rpc/socket_rpc_client.h
   someip/rpc/socket_rpc_client.cpp

   #### someip_sd message ####
   someip/sd/sd_network_layer.cpp
   someip/sd/sd_network_layer.h
   someip/sd/someip_sd_agent.h
   someip/sd/someip_sd_message.cpp
   someip/sd/someip_sd_message.h
   someip/sd/someip_sd_server.cpp
   someip/sd/someip_sd_server.h

   someip/sd/fsm/timer_set_state.h
   someip/sd/fsm/client_service_state.h
   someip/sd/fsm/notready_state.h
   someip/sd/fsm/notready_state.cpp
   someip/sd/fsm/initial_wait_state.h
   someip/sd/fsm/client_initial_wait_state.h
   someip/sd/fsm/client_initial_wait_state.cpp
   someip/sd/fsm/repetition_state.h
   someip/sd/fsm/client_repetition_state.h
   someip/sd/fsm/client_repetition_state.cpp
   someip/sd/fsm/main_state.h
   someip/sd/fsm/main_state.cpp
   someip/sd/fsm/stopped_state.h
   someip/sd/fsm/stopped_state.cpp
   someip/sd/fsm/service_notseen_state.h
   someip/sd/fsm/service_notseen_state.cpp
   someip/sd/fsm/service_ready_state.h
   someip/sd/fsm/service_ready_state.cpp
   someip/sd/fsm/service_seen_state.h
   someip/sd/fsm/service_seen_state.cpp

   #### provider and subscripter for events ####
   someip/pubsub/fsm/notsubscribed_state.cpp
   someip/pubsub/fsm/notsubscribed_state.h
   someip/pubsub/fsm/service_down_state.cpp
   someip/pubsub/fsm/service_down_state.h
   someip/pubsub/fsm/subscribed_state.cpp
   someip/pubsub/fsm/subscribed_state.h

   someip/pubsub/socket_pubSub.cpp
   someip/pubsub/socket_pubSub.h
   someip/pubsub/someip_pubsub_server.cpp
   someip/pubsub/someip_pubsub_server.h
   someip/pubsub/someip_pubsub_client.cpp
   someip/pubsub/someip_pubsub_client.h
)

######################### samples ##################################
#### execuatables for rpc ####
add_executable(server_rpc samples/demo_rpc_server.cpp)
target_link_libraries(server_rpc araCom)

add_executable(client_rpc samples/demo_rpc_client.cpp)
target_link_libraries(client_rpc araCom)

#### executables for pubSub ####
add_executable(client_pubSub samples/demo_pubSub_client.cpp)
target_link_libraries(client_pubSub araCom)

add_executable(server_pubSub samples/demo_pubSub_server.cpp)
target_link_libraries(server_pubSub araCom)