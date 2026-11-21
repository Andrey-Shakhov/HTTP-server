#include "net/reactor.hpp"
#include "http/parser.hpp"
#include "http/response.hpp"
#include "http/router.hpp"
#include <cerrno>
#include <csignal>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
namespace net {

volatile std::sig_atomic_t stop = 0;

void signal_handler(int signal) { stop = signal; }

void InstallSignalHandler() {
  struct sigaction sa{};
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGINT, &sa, nullptr) == -1 ||
      sigaction(SIGTERM, &sa, nullptr) == -1) {
    perror("sigaction");
    exit(1);
  }
}

bool SendAll(int fd, const std::string &data) {
  std::size_t sent = 0;
  while (sent < data.size()) {
    const ssize_t count = send(fd, data.data() + sent, data.size() - sent, 0);
    if (count > 0)
      sent += static_cast<size_t>(count);
    else if (count < 0 && errno == EINTR)
      continue;
    else {
      return false;
    }
  }
  return true;
}

Server::Server(server::Config config, const http::Router &router)
    : config_(config), router_(router) {}
void Server::Run() {
  InstallSignalHandler();
  int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
  int yes = 1;
  if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
      -1) {
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
  std::cout << "Listening on port: " << config_.port << '\n';
  while (!stop) {
    int client_fd = accept(listener_fd, nullptr, nullptr);
    if (client_fd == -1) {
      if (errno == EINTR) {
        continue;
      }
      perror("accept");
      break;
    }
    ServeClient(client_fd);
    close(client_fd);
  }
  if (stop == SIGINT) {
    std::cout << "\nSIGINT received\n";
  } else if (stop == SIGTERM) {
    std::cout << "\nSIGTERM received\n";
  }
  close(listener_fd);
}
void Server::ServeClient(int client_fd) {
  std::string buffer;
  char chunk[4096];
  while (true) {
    while (!buffer.empty()) {
      const http::ParseResult parsed = http::Parser::Parse(buffer);
      if (parsed.status == http::ParseStatus::Incomplete) {
        break;
      }
      if (parsed.status == http::ParseStatus::BadRequest) {
        http::Response response =
            http::Response::Json(400, R"({"error":"Bad Request"})");
        SendAll(client_fd, response.Serialize(false));
        return;
      }
      const bool keep_alive = parsed.request.keep_alive && !stop;
      std::cout << parsed.request.method + parsed.request.path << '\n';
      http::Response response = router_.Handle(parsed.request);
      if (!SendAll(client_fd, response.Serialize(keep_alive))) {
        return;
      }
      buffer.erase(0, parsed.bytes_consumed);
      if (!keep_alive)
        return;
    }
    if (stop)
      return;
    const ssize_t received = recv(client_fd, chunk, sizeof(chunk), 0);
    if (received > 0) {
      buffer.append(chunk, static_cast<size_t>(received));
    } else if (received == 0) {
      return;
    } else if (!stop && received == -1 && errno == EINTR) {
      continue;
    } else {
      return;
    }
  }
}
} // namespace net
