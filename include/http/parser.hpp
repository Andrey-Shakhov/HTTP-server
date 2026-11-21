#pragma once
#include "request.hpp"
#include <cstddef>
#include <string_view>
namespace http {

enum class ParseStatus {
    Incomplete,
    Complete,
    BadRequest
};

struct ParseResult {
    ParseStatus status = ParseStatus::Incomplete;
    Request request;
    std::size_t bytes_consumed = 0;
};

class Parser {
public:
    static constexpr std::size_t kMaxHeaderBytes = 16 * 1024;
    static constexpr std::size_t kMaxBodyBytes = 1024 * 1024;
    [[nodiscard]] static ParseResult Parse(std::string_view buffer);
};
}