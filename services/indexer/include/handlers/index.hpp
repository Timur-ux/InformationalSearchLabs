#ifndef TOKEN_HANDLER_HPP_
#define TOKEN_HANDLER_HPP_
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#pragma once

#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace SERVICE_NAMESPACE {
using namespace userver;
class IndexHandler final : public server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "index-handler";

  using server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase;
  using Value = formats::json::Value;
  using HttpRequest = server::http::HttpRequest;
  using RequestContext = server::request::RequestContext;

  Value HandleRequestJsonThrow(const HttpRequest &request, const Value &requestJson,
                      RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE

#endif // !TOKEN_HANDLER_HPP_
