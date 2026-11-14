#pragma once
#include <string>
#include <unordered_map>
namespace http {

struct Response {
    int status = 200;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    [[nodiscard]] static Response Json(int status, std::string body);
    [[nodiscard]] std::string Serialize(bool keep_alive) const;
};

}