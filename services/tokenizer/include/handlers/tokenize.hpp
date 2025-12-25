#ifndef TOKENIZE_HANDLER_HPP_
#define TOKENIZE_HANDLER_HPP_
#include "components/Tokenizer.hpp"
#include <codecvt>
#include <userver/components/component_config.hpp>
#include <userver/components/component_context.hpp>
#include <userver/formats/json/value.hpp>
#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace SERVICE_NAMESPACE {
using namespace userver;
class TokenizeHandler final : public server::handlers::HttpHandlerJsonBase {
	Tokenizer & tokenizer_;
public:
  static constexpr std::string_view kName = "tokenize-handler";

	using HttpRequest = server::http::HttpRequest;
	using Value = formats::json::Value;
	using RequestContext = server::request::RequestContext;

	TokenizeHandler(const components::ComponentConfig &config, const components::ComponentContext & context);

	Value HandleRequestJsonThrow(const HttpRequest &request, const Value &requestJson,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE
	
#endif // !TOKENIZE_HANDLER_HPP_
