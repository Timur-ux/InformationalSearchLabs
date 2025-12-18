#include "handlers/index.hpp"
#include "exceptions/PageNotFound.hpp"
#include "schemas/doIndex.hpp"
#include "schemas/insert.hpp"
#include "schemas/tokenize.hpp"
#include <cstdlib>
#include <format>
#include <stdexcept>
#include <userver/formats/bson.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include <userver/http/http_version.hpp>
#include <userver/logging/log.hpp>

#include "exceptions/KeyNotFound.hpp"

namespace SERVICE_NAMESPACE {
IndexHandler::IndexHandler(const components::ComponentConfig &config,
                           const components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      pool_(context.FindComponent<components::Mongo>("mongo-index").GetPool()),
      httpClient_(
          context.FindComponent<components::HttpClient>("index-http-client").GetHttpClient()) {
  tokenizeServiceAddr_ = getenv("TOKENIZE_SERVICE_ADDR");
  if (!tokenizeServiceAddr_) {
    LOG_CRITICAL() << "Env variable [TOKENIZE_SERVICE_ADDR] is not set";
		throw std::runtime_error("Env variable [TOKENIZE_SERVICE_ADDR] is not set");
	}

  dbServiceAddr_ = getenv("DB_SERVICE_ADDR");
  if (!dbServiceAddr_) {
    LOG_CRITICAL() << "Env variable [DB_SERVICE_ADDR] is not set";
		throw std::runtime_error("Env variable [DB_SERVICE_ADDR] is not set");
	}
}

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
  if (!doc.has_value())
    throw exception::PageNotFoundException(pageId);

  auto content = (*doc)["raw"].As<std::string>();
  auto tokens = tokenize(content);
  insertTokensToDB(pageId, tokens);
  LOG_DEBUG() << "Page with id: " << pageId
              << " have raw content length: " << content.size()
              << " and tokens amount: " << tokens.size();
}

std::vector<std::uint32_t> IndexHandler::tokenize(std::string data) const {
  tokenize::TokenizeRequestBody requestBody{std::move(data)};
  auto requestJson = formats::json::ValueBuilder{requestBody}.ExtractValue();
  auto requestData = formats::json::ToString(requestJson);

  auto request =
      httpClient_.CreateRequest()
          .post()
          .url(std::format("{}/{}", tokenizeServiceAddr_, "tokenize"))
          .data(std::move(requestData))
          .retry(1)
					.timeout(1000);

  auto res = request.perform();
  if (res->IsOk()) 
    return formats::json::FromString(std::move(*res).body())
        .As<std::vector<std::uint32_t>>();
	

  LOG_ERROR() << "Request on endpoint " << request.GetUrl()
              << " return bad response: " << res->status_code() << "\t"
              << res->body_view();
  return {};
}

void IndexHandler::insertTokensToDB(std::uint32_t pageId,
                                    std::vector<std::uint32_t> tokens) const {
  insert::InsertRequestBody requestBody{pageId, std::move(tokens)};
  auto requestJson = formats::json::ValueBuilder{requestBody}.ExtractValue();
  auto requestData = formats::json::ToString(requestJson);

  auto request = httpClient_.CreateRequest()
                     .post()
                     .url(std::format("{}/{}", dbServiceAddr_, "insert"))
                     .data(std::move(requestData))
                     .retry(1)
										 .timeout(5000);

  auto res = request.perform();
  if (res->IsError())
    LOG_ERROR() << "Request on endpoint " << request.GetUrl()
                << " return bad response: " << res->status_code()
                << "\t" << res->body_view();
}

} // namespace SERVICE_NAMESPACE
