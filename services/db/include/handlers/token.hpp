#ifndef TOKEN_HANDLER_HPP_
#define TOKEN_HANDLER_HPP_
#include "components/DataBase.hpp"
#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace SERVICE_NAMESPACE {
using namespace userver;
class TokenHandler final : public server::handlers::HttpHandlerBase {
	DataBase & db_;
public:
  static constexpr std::string_view kName = "token-handler";

	using HttpRequest = server::http::HttpRequest;
	using RequestContext = server::request::RequestContext;

	TokenHandler(const components::ComponentConfig &config,
                                 const components::ComponentContext &context);
	std::string HandleRequest(HttpRequest &request,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE
	
#endif // !TOKEN_HANDLER_HPP_
