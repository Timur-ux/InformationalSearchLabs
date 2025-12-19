#include "handlers/quote.hpp"
#include "Map.hpp"
#include "Requester.hpp"
#include "handlers/boolean.hpp"
#include "schemas/pageData.hpp"
#include "schemas/search.hpp"
#include <cstdint>
#include <cstdlib>
#include <userver/formats/bson.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/serialize_container.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include <userver/http/http_version.hpp>
#include <userver/logging/log.hpp>
#include <vector>

#include "exceptions/KeyNotFound.hpp"
#include "utils.hpp"

namespace SERVICE_NAMESPACE {
QuoteSearchHandler::QuoteSearchHandler(
    const components::ComponentConfig &config,
    const components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      pool_(context.FindComponent<components::Mongo>("mongo").GetPool()),
      httpClient_(context.FindComponent<components::HttpClient>("http-client")
                      .GetHttpClient()) {}

formats::json::Value
QuoteSearchHandler::HandleRequestJsonThrow(const HttpRequest &request,
                                           const Value &requestJson,
                                           RequestContext &context) const {
  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kApplicationJson);

  auto text = request.GetArg("text");
  if (text.empty())
    throw exception::KeyNotFoundException("text");

  // Extract documents with tokens from request
  auto tokens = Requester::instance().tokenize(httpClient_, text);
  auto uniqueTokens = getUnique(tokens);
  auto tokenDocuments = Requester::instance().getDocuments(httpClient_, uniqueTokens);

  std::vector<std::uint32_t> documentsIntersection =
      BooleanSearchHandler::getValuesIntersection(tokenDocuments);
	auto documentTokens = Requester::instance().getTokens(httpClient_, documentsIntersection);
  std::vector<std::uint32_t> documents{};
	for(const auto & [documentId, tokenIds] : documentTokens) 
		if(zStringContains<std::uint32_t>({tokenIds.data(), tokenIds.size()}, {tokens.data(), tokens.size()}))
			documents.emplace_back(documentId);

  // Extract page data from founded documents
  auto pagesData = Requester::instance().getPagesData(pool_, documents);
  std::vector<pageData::PageData> pages{};
  for (auto &[docId, pageData] : pagesData)
    pages.emplace_back(std::move(pageData));

  search::SearchResponseBody responseBody{std::move(pages)};
  return formats::json::ValueBuilder{responseBody}.ExtractValue();
}

} // namespace SERVICE_NAMESPACE
