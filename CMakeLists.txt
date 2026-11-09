cmake_minimum_required(VERSION 3.16)

project(HTTP-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(http_server
    src/main.cpp
    src/http/parser.cpp
    src/http/response.cpp
    src/http/router.cpp
    src/net/reactor.cpp
    src/server/config.cpp
)

target_include_directories(http_server PRIVATE include)
