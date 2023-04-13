cmake_minimum_required(VERSION 3.22)
project(tcp_chat)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/master.zip
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/download_deps/gtest
)
FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG v0.6.0
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/download_deps/glog
)

FetchContent_MakeAvailable(googletest glog)

add_executable(Client "code/src/client.cpp" "test/src/client_test.cpp" inc/client.h inc/common.h)
add_executable(Server "code/src/server.cpp" "test/src/server_test.cpp" inc/server.h inc/common.h)

include_directories(glog::glog)
target_link_libraries(Client gtest_main glog::glog)
target_link_libraries(Server gtest_main glog::glog)
