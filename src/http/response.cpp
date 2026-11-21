#include "http/response.hpp"

#include <stdexcept>
#include <string_view>
#include <utility>

namespace http {
namespace {

std::string_view ReasonPhrase(int status) {
    switch (status) {
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 204:
            return "No Content";
        case 400:
            return "Bad Request";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 409:
            return "Conflict";
        case 500:
            return "Internal Server Error";
        default:
            throw std::invalid_argument("Unsupported HTTP response status");
    }
}

bool EqualsIgnoreCase(std::string_view left, std::string_view right) {
    if (left.size() != right.size()) {
        return false;
    }

    for (std::size_t index = 0; index < left.size(); ++index) {
        char character = left[index];

        if (character >= 'A' && character <= 'Z') {
            character += 'a' - 'A';
        }

        if (character != right[index]) {
            return false;
        }
    }

    return true;
}

void ValidateHeader(std::string_view name, std::string_view value) {
    if (name.empty()) {
        throw std::invalid_argument("Empty HTTP header name");
    }

    constexpr std::string_view symbols = "!#$%&'*+-.^_`|~";

    for (unsigned char character : name) {
        const bool is_letter = (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z');

        const bool is_digit = character >= '0' && character <= '9';

        if (!is_letter && !is_digit && symbols.find(character) == std::string_view::npos) {
            throw std::invalid_argument("Invalid HTTP header name");
        }
    }

    for (unsigned char character : value) {
        if ((character < 32 && character != '\t') || character == 127) {
            throw std::invalid_argument("Invalid HTTP header value");
        }
    }
}

} 

Response Response::Json(int status, std::string body) {
    Response response;

    response.status = status;
    response.body = std::move(body);
    response.headers.emplace("Content-Type", "application/json");
    return response;
}

std::string Response::Serialize(bool keep_alive) const {
    std::string result = "HTTP/1.1 ";
    result += std::to_string(status);
    result += ' ';
    result += ReasonPhrase(status);
    result += "\r\n";

    for (const auto& [name, value] : headers) {
        ValidateHeader(name, value);

        if (EqualsIgnoreCase(name, "content-length") ||
            EqualsIgnoreCase(name, "connection") ||
            EqualsIgnoreCase(name, "transfer-encoding")) {
            continue;
        }

        result += name;
        result += ": ";
        result += value;
        result += "\r\n";
    }

    if (status != 204) {
        result += "Content-Length: ";
        result += std::to_string(body.size());
        result += "\r\n";
    }

    result += "Connection: ";
    result += keep_alive ? "keep-alive" : "close";
    result += "\r\n\r\n";

    if (status != 204) {
        result += body;
    }

    return result;
}

}