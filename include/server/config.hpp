#pragma once
#include <cstdint>
#include <cstddef>

namespace server
{
    struct Config
    {
        std::uint16_t port = 8080;
        std::size_t threads = 1;
    };
    Config Parse(int argc, char* argv[]);
} // namespace server

