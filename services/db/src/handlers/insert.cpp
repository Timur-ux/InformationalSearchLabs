#include "handlers/insert.hpp"
#include "schemas/insert.hpp"
#include <format>
#include <string>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>

namespace dbService {
std::string InsertHandler::HandleRequest(server::http::HttpRequest &request, server::request::RequestContext &context) const {
	request.GetHttpResponse().SetContentType(http::content_type::kApplicationJson);

	auto requestJson = formats::json::FromString(request.RequestBody());

	auto requestBody = requestJson.As<insert::InsertRequestBody>();

	std::string message = std::format("id: {}; tokens: [", requestBody.id.value_or(-1));
	auto tokens = requestBody.tokens.value_or(std::vector<int>{});
	for(auto & token : tokens) 
		message += std::format("{}, ", token);
	message += "]";

	insert::InsertResponseBody responseBody{message};

	auto responseJson = formats::json::ValueBuilder{responseBody}.ExtractValue();

	return formats::json::ToString(responseJson);
}
} // namespace dbService
