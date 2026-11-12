#include <cstddef>
#include <string_view>
#include <variant>
namespace http {
struct NeedMore {};
struct BadRequest {};
struct ParsedRequest {
    Request request;
    std::size_t consumed;
};
using ParseResult = std::variant<NeedMore, BadRequest, ParsedRequest>;
ParseResult parse_request(std::string_view input);
}
