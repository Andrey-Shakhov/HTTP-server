#include "net/reactor.hpp"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <iostream>
namespace net
{
    bool SendAll(int fd, const std::string& data){
        std::size_t sent=0;
        while (sent < data.size()){
            const ssize_t count = send(fd, data.data()+sent, data.size()-sent,0);
            if (count>0) sent+=static_cast<size_t>(count);
            else if(count <0 && errno==EINTR) continue;
            else {return false;}
        }
        return true;
    }

    Server::Server(server::Config config, const http::Router& router):config_(config), router_(router){}
    void Server::Run(){
        int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
        int yes=1;
        if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
          perror("setsockopt");
          exit(1);
        }
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(config_.port);
        if (bind(listener_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
          perror("bind");
          exit(1);
        }
        if (listen(listener_fd, SOMAXCONN) == -1) {
          perror("listen");
          exit(1);
        }
        std::cout<<"Listening on port: "<<config_.port<<'\n';
        while (1){
            int client_fd = accept(listener_fd, nullptr, nullptr);
            if (client_fd==-1){
              perror("accept");
              exit(1);
            }
            ServeClient(client_fd);
        }
    }
    void Server::ServeClient(int client_fd){
        //TODO parse(request)->handle->send(response)
    }

} // namespace net


