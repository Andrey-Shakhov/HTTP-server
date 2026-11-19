#include "http/router.hpp"

namespace http {

Response Router::Handle(const Request& request) const {
    if (request.path != "/health") {
        return Response::Json(
                404,
                R"({"error":"Not Found"})"
        );
    }

    if (request.method != "GET") {
        Response response = Response::Json(
                405,
                R"({"error":"Method Not Allowed"})"
        );

        response.headers.emplace("Allow", "GET");

        return response;
    }

    return Response::Json(
            200,
            R"({"status":"ok"})"
    );
}

}