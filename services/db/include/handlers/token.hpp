#ifndef TOKEN_HANDLER_HPP_
#define TOKEN_HANDLER_HPP_
#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace SERVICE_NAMESPACE {
using namespace userver;
class TokenHandler final : public server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "token-handler";

  using server::handlers::HttpHandlerBase::HttpHandlerBase;
	using HttpRequest = server::http::HttpRequest;
	using RequestContext = server::request::RequestContext;

	std::string HandleRequest(HttpRequest &request,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE
	
#endif // !TOKEN_HANDLER_HPP_
