#ifndef DOCUMENT_HANDLER_HPP_
#define DOCUMENT_HANDLER_HPP_
#include "components/DataBase.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#pragma once

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace SERVICE_NAMESPACE {
using namespace userver;
class DocumentHandler final : public server::handlers::HttpHandlerBase {
	DataBase & db_;
public:
  static constexpr std::string_view kName = "document-handler";

	using HttpRequest = server::http::HttpRequest;
	using RequestContext = server::request::RequestContext;

	DocumentHandler(const components::ComponentConfig & config, const components::ComponentContext & context);

	std::string HandleRequest(HttpRequest &request,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE

#endif // !DOCUMENT_HANDLER_HPP_
