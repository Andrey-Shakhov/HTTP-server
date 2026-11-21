#include <iostream>
#include "net/reactor.hpp"
#include "server/config.hpp"

int main(int argc, char* argv []){
    try{
        server::Config config = server::Parse(argc, argv);
        http::Router router;
        net::Server server(config, router);
        server.Run();
    }
    catch (const std::exception& e){
        std::cerr << "server error: " << e.what() <<'\n';
        return 1;
    }

    return 0;
}
