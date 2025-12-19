#include "handlers/detokenize.hpp"
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include "schemas/detokenize.hpp"
#include "Tokenizer.hpp"

namespace SERVICE_NAMESPACE {
formats::json::Value
DetokenizeHandler::HandleRequestJsonThrow(const HttpRequest &request,
                                        const Value &requestJson,
                                        RequestContext &context) const {
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt_;
	request.GetHttpResponse().SetContentType(userver::http::content_type::kApplicationJson);
	auto requestBody = requestJson.As<detokenize::DetokenizeRequestBody>();

	auto text = Tokenizer::instance().detokenize(IR::Vector<std::uint32_t>{requestBody.data(), requestBody.size()});

	detokenize::DetokenizeResponseBody responseBody{cvt_.to_bytes(text)};

	return formats::json::ValueBuilder{responseBody}.ExtractValue();
}
} // namespace SERVICE_NAMESPACE
