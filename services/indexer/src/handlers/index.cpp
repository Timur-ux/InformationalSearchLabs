#include "handlers/index.hpp"
#include "exceptions/PageNotFound.hpp"
#include "schemas/doIndex.hpp"
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include <userver/formats/bson.hpp>
#include <userver/logging/log.hpp>

#include "exceptions/KeyNotFound.hpp"

namespace SERVICE_NAMESPACE {
formats::json::Value
IndexHandler::HandleRequestJsonThrow(const HttpRequest &request,
                                     const Value &requestJson,
                                     RequestContext &context) const {
  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kTextPlain);

  auto requestBody = requestJson.As<doIndex::DoIndexRequestBody>();
  if (!requestBody.pageId.has_value())
    throw exception::KeyNotFoundException("pageId");

  indexPage(*requestBody.pageId);

  doIndex::DoIndexResponseBody responseBody{"success"};
  return formats::json::ValueBuilder{responseBody}.ExtractValue();
}

void IndexHandler::indexPage(std::uint32_t pageId) const {
	auto transaction = pool_->GetCollection("ParsedDocuments");
	using formats::bson::MakeDoc;
	auto doc = transaction.FindOne(MakeDoc("_id", MakeDoc("$eq", pageId)));
	if(!doc.has_value()) 
		throw exception::PageNotFoundException(pageId);

	auto content = (*doc)["raw"].As<std::string>();
	// do tokenize stuff ...
	//
	// do inserting in db stuff ...
	LOG_DEBUG() << "Page with id: " << pageId << " have raw content length: " << content.size();
}

} // namespace SERVICE_NAMESPACE
