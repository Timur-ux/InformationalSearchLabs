#include "handlers/tokenize.hpp"
#include "components/Tokenizer.hpp"
#include "schemas/tokenize.hpp"
#include <cctype>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/serialize_container.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>

namespace SERVICE_NAMESPACE {
TokenizeHandler::TokenizeHandler(const components::ComponentConfig &config,
                                 const components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      tokenizer_(context.FindComponent<TokenizerComponent>().GetTokenizer()) {}
formats::json::Value
TokenizeHandler::HandleRequestJsonThrow(const HttpRequest &request,
                                        const Value &requestJson,
                                        RequestContext &context) const {
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt_;
  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kApplicationJson);

  auto requestBody =
      cvt_.from_bytes(requestJson.As<tokenize::TokenizeRequestBody>());
  for (wchar_t &c : requestBody)
    c = tolower(c);

  auto tokens = tokenizer_.tokenize(requestBody);

  tokenize::TokenizeResponseBody responseBody{std::begin(tokens),
                                              std::end(tokens)};

  return formats::json::ValueBuilder{responseBody}.ExtractValue();
}
} // namespace SERVICE_NAMESPACE
