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

include(CTest)
include(FetchContent)

if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(storage_tests
        tests/storage_test.cpp
        src/storage/storage.cpp
    )

    target_include_directories(storage_tests PRIVATE include)

    target_link_libraries(storage_tests PRIVATE
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(storage_tests)
endif()
