cmake_minimum_required(VERSION 3.10)
project(SecureChatServer VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(ProjectConfig.h.in ProjectConfig.h)

add_executable(Main main.cpp)
add_executable(Server server.cpp)
add_executable(Client client.cpp)

target_include_directories(Main PUBLIC "${PROJECT_BINARY_DIR}")

