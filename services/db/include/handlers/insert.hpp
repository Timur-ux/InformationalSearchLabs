#ifndef INSERT_HANDLER_HPP_
#define INSERT_HANDLER_HPP_
#include "components/DataBase.hpp"
#pragma once
#include <userver/formats/json/value.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

namespace SERVICE_NAMESPACE {
using namespace userver;
class InsertHandler final : public server::handlers::HttpHandlerJsonBase {
	DataBase & db_;
public:
  static constexpr std::string_view kName = "insert-handler";

	InsertHandler(const components::ComponentConfig &config,
                                 const components::ComponentContext &context);

  Value HandleRequestJsonThrow(const HttpRequest &request,
                               const Value &request_json,
                               RequestContext &context) const override;
};
} // namespace dbService
#endif // !INSERT_HANDLER_HPP_
