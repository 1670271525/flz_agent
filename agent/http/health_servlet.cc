#include "agent/http/health_servlet.h"

#include "agent/util/json_helper.h"

namespace agent {

HealthServlet::HealthServlet()
    : flz::http::Servlet("HealthServlet") {
}

int32_t HealthServlet::handle(flz::http::HttpRequest::ptr request,
                              flz::http::HttpResponse::ptr response,
                              flz::http::HttpSession::ptr session) {
    (void)request;
    (void)session;
    Json::Value root;
    root["code"] = 0;
    root["msg"] = "ok";
    response->setStatus(flz::http::HttpStatus::OK);
    response->setHeader("Content-Type", "application/json; charset=utf-8");
    response->setBody(JsonHelper::toString(root));
    return 0;
}

} // namespace agent
