#pragma once
#include "server/config.hpp"

namespace http{class Router;}

namespace net{
    class Server
    {
    public:
        Server(server::Config config, const http::Router& router);
        void Run();
        ~Server();

    private:
        void ServeClient(int client_fd);
        server::Config config_;
        const http::Router& router_;
    };
}
