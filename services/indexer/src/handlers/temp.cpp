#include "handlers/temp.hpp"
#include "exceptions/temp.hpp"
#include "schemas/temp.hpp"
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>

namespace SERVICE_NAMESPACE {
	std::string TempHandler::HandleRequest(HttpRequest & request, RequestContext & context) const {
		request.GetHttpResponse().SetContentType(userver::http::content_type::kTextPlain);

		auto arg = request.GetArg("name");
		if(arg.empty()) 
			throw exception::TempException();
		
		temp::TempResponseBody responseBody;
		responseBody = std::format("Hello {}!", arg.empty() ? responseBody : arg);
		
		auto responseJson = formats::json::ValueBuilder{responseBody}.ExtractValue();
		return formats::json::ToString(responseJson);
	}
} // namespace SERVICE_NAMESPACE
