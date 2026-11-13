#pragma once
#include <string>
#include <unordered_map>
namespace http {
struct Request {
    std::string method;
    std::string target;
    std::string path
    std::string query; 
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool keep_alive = true;
};

}
