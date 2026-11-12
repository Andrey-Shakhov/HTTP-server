#pragma once
#include <string>
#include <unordered_map>
namespace http {
using Headers = std::unordered_map<std::string, std::string>;
struct Request {
    std::string method;
    std::string target;
    Headers headers;
    std::string body;
    bool keep_alive = true;
};
struct Response {
    int status = 200;
    std::string body;
    Headers headers;
};
std::string serialize_response(const Response& response, bool keep_alive);
}
