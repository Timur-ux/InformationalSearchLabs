#ifndef INSERT_HANDLER_HPP_
#define INSERT_HANDLER_HPP_
#pragma once
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

namespace SERVICE_NAMESPACE {
using namespace userver;
class InsertHandler final : public server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "insert-handler";

  using server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase;

  Value HandleRequestJsonThrow(const HttpRequest &request,
                               const Value &request_json,
                               RequestContext &context) const override;
};
} // namespace dbService
#endif // !INSERT_HANDLER_HPP_
