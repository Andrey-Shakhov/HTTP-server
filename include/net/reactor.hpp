#pragma once
#include "http/router.hpp"
#include "server/config.hpp"

namespace net {
class Server {
public:
  Server(server::Config config, const http::Router &router);
  void Run();
  ~Server() = default;

private:
  void ServeClient(int client_fd);
  server::Config config_;
  const http::Router &router_;
};
} // namespace net
