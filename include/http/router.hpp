#pragma once
#include "request.hpp"
#include "response.hpp"
namespace http {

class Router {
public:
    [[nodiscard]] Response Handle(const Request& request) const;

};
}