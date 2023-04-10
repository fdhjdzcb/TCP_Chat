cmake_minimum_required(VERSION 3.22)
project(tcp_chat)

set(CMAKE_CXX_STANDARD 20)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "-lncursesw")

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/master.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(Client "code/src/client.cpp" "test/src/client_test.cpp" code/inc/client.h code/inc/server.h)
add_executable(Server "code/src/server.cpp" "test/src/server_test.cpp")

target_link_libraries(Client gtest_main)
target_link_libraries(Server gtest_main)
target_link_libraries(Client ${CURSES_LIBRARY})