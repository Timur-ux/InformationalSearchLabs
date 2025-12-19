#ifndef DETOKENIZE_HANDLER_HPP_
#define DETOKENIZE_HANDLER_HPP_
#include <codecvt>
#include <locale>
#include <userver/formats/json/value.hpp>
#pragma once

#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
namespace SERVICE_NAMESPACE {
using namespace userver;
class DetokenizeHandler final : public server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "detokenize-handler";

  using server::handlers::HttpHandlerJsonBase::HttpHandlerJsonBase;
	using HttpRequest = server::http::HttpRequest;
	using Value = formats::json::Value;
	using RequestContext = server::request::RequestContext;

	Value HandleRequestJsonThrow(const HttpRequest &request, const Value &requestJson,
                               RequestContext &context) const override;
};
} // namespace SERVICE_NAMESPACE
	
#endif // !DETOKENIZE_HANDLER_HPP_

