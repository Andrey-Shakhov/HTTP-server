#include "http/parser.hpp"
#include <algorithm>
#include <charconv>
#include <string>
#include <system_error>

namespace http {
namespace {

std::string_view Trim(std::string_view text) {
    const auto begin = text.find_first_not_of(" \t");

    if (begin == std::string_view::npos) {
        return {};
    }

    const auto end = text.find_last_not_of(" \t");
    return text.substr(begin, end - begin + 1);
}

std::string Lowercase(std::string_view text) {
    std::string result(text);

    for (char& character : result) {
        if (character >= 'A' && character <= 'Z') {
            character += 'a' - 'A';
        }
    }

    return result;
}

bool IsToken(std::string_view text) {
    constexpr std::string_view allowed =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789!#$%&'*+-.^_`|~";

    return !text.empty() && text.find_first_not_of(allowed) == std::string_view::npos;
}

bool IsTarget(std::string_view text) {
    return text.starts_with('/') && std::all_of(text.begin(), text.end(), [](unsigned char character) {
               return character > 32 && character < 127 && character != '#';
           });
}

bool IsHeaderValue(std::string_view text) {
    return std::all_of(text.begin(), text.end(), [](unsigned char character) {
        return character == '\t' || (character >= 32 && character != 127);
    });
}

bool ContainsClose(std::string_view value) {
    while (!value.empty()) {
        const auto comma = value.find(',');

        if (Lowercase(Trim(value.substr(0, comma))) == "close") {
            return true;
        }

        if (comma == std::string_view::npos) {
            break;
        }

        value.remove_prefix(comma + 1);
    }

    return false;
}

ParseResult BadRequest() {
    ParseResult result;
    result.status = ParseStatus::BadRequest;
    return result;
}

}

ParseResult Parser::Parse(std::string_view buffer) {
    const auto headers_end = buffer.find("\r\n\r\n");

    if (headers_end == std::string_view::npos) {
        if (buffer.size() >= kMaxHeaderBytes) {
            return BadRequest();
        }

        return {};
    }

    const auto body_start = headers_end + 4;

    if (body_start > kMaxHeaderBytes) {
        return BadRequest();
    }

    const auto line_end = buffer.find("\r\n");
    const auto line = buffer.substr(0, line_end);
    const auto first_space = line.find(' ');

    if (first_space == std::string_view::npos) {
        return BadRequest();
    }

    const auto second_space = line.find(' ', first_space + 1);

    if (second_space == std::string_view::npos) {
        return BadRequest();
    }

    const auto method = line.substr(0, first_space);
    const auto target = line.substr(first_space + 1, second_space - first_space - 1);

    if (!IsToken(method) || !IsTarget(target) || line.substr(second_space + 1) != "HTTP/1.1") {
        return BadRequest();
    }

    ParseResult result;
    auto& request = result.request;

    request.method = method;
    request.target = target;

    const auto question = target.find('?');
    request.path = target.substr(0, question);

    if (question != std::string_view::npos) {
        request.query = target.substr(question + 1);
    }

    auto position = line_end + 2;

    while (position < headers_end) {
        const auto end = buffer.find("\r\n", position);
        const auto header = buffer.substr(position, end - position);
        const auto colon = header.find(':');

        if (colon == std::string_view::npos || !IsToken(header.substr(0, colon))) {
            return BadRequest();
        }

        const auto name = Lowercase(header.substr(0, colon));
        const auto value = Trim(header.substr(colon + 1));

        if (!IsHeaderValue(value)) {
            return BadRequest();
        }

        auto [iterator, inserted] =
            request.headers.emplace(name, std::string(value));

        if (!inserted) {
            if (name == "host" || name == "content-length") {
                return BadRequest();
            }

            iterator->second += ", ";
            iterator->second.append(value);
        }

        position = end + 2;
    }

    const auto host = request.headers.find("host");

    if (host == request.headers.end() || host->second.empty() || request.headers.contains("transfer-encoding") || request.headers.contains("expect")) {
        return BadRequest();
    }

    std::size_t content_length = 0;

    if (auto header = request.headers.find("content-length");
        header != request.headers.end()) {
        const auto& value = header->second;
        const auto end = value.data() + value.size();
        const auto conversion =
            std::from_chars(value.data(), end, content_length);

        if (conversion.ec != std::errc{} || conversion.ptr != end || content_length > kMaxBodyBytes) {
            return BadRequest();
        }
    }

    if (buffer.size() - body_start < content_length) {
        return {};
    }

    if (auto header = request.headers.find("connection");
        header != request.headers.end()) {
        request.keep_alive = !ContainsClose(header->second);
    }

    request.body = buffer.substr(body_start, content_length);
    result.status = ParseStatus::Complete;
    result.bytes_consumed = body_start + content_length;

    return result;
}

}