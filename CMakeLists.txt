cmake_minimum_required(VERSION 3.16)
project(omci_helper CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(helper
    src/main.cpp
    src/message_parser.cpp
    src/message_decoder.cpp
)

target_include_directories(helper PRIVATE include)
target_compile_options(helper PRIVATE -Wall -Wextra -O2)
