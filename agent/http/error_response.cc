#include "agent/http/error_response.h"

#include "agent/util/json_helper.h"

namespace agent {

void ErrorResponse::writeJson(flz::http::HttpResponse::ptr rsp, int code, const std::string& msg) {
    Json::Value root = JsonHelper::makeError(code, msg);
    rsp->setStatus(static_cast<flz::http::HttpStatus>(code));
    rsp->setHeader("Content-Type", "application/json; charset=utf-8");
    rsp->setBody(JsonHelper::toString(root));
}

void ErrorResponse::writeSse(SseWriter& writer, int code, const std::string& msg) {
    Json::Value root = JsonHelper::makeError(code, msg);
    writer.sendEvent("error", JsonHelper::toString(root));
}

} // namespace agent
