cmake_minimum_required(VERSION 3.22)
project(tcp_chat)

set(CMAKE_CXX_STANDARD 20)

find_package(GTest REQUIRED)
find_package(glog REQUIRED)

add_library(ServerLib "code/server.cpp" "inc/serverFuncs.cpp" "inc/server.h" "inc/common.h")
add_executable(Server "code/server.cpp")
add_executable(ServerTests "test/server_test.cpp")

add_library(ClientLib "code/client.cpp" "inc/clientFuncs.cpp" "inc/client.h" "inc/common.h")
add_executable(Client "code/client.cpp")
add_executable(ClientTests "test/client_test.cpp")

target_link_libraries(Server ServerLib GTest::Main glog::glog gmock)
target_link_libraries(Client ClientLib GTest::Main glog::glog gmock)
target_link_libraries(ServerTests ServerLib GTest::Main glog::glog gmock)
target_link_libraries(ClientTests ClientLib GTest::Main glog::glog gmock)
