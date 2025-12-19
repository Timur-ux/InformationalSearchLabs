#include "handlers/boolean.hpp"
#include "utils.hpp"
#include "Map.hpp"
#include "Requester.hpp"
#include "schemas/pageData.hpp"
#include "schemas/search.hpp"
#include <cstddef>
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

namespace SERVICE_NAMESPACE {
BooleanSearchHandler::BooleanSearchHandler(
    const components::ComponentConfig &config,
    const components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      pool_(context.FindComponent<components::Mongo>("mongo").GetPool()),
      httpClient_(context.FindComponent<components::HttpClient>("http-client")
                      .GetHttpClient()) {}

formats::json::Value
BooleanSearchHandler::HandleRequestJsonThrow(const HttpRequest &request,
                                     const Value &requestJson,
                                     RequestContext &context) const {
  request.GetHttpResponse().SetContentType(
      userver::http::content_type::kApplicationJson);

	auto text = request.GetArg("text");
  if (text.empty())
    throw exception::KeyNotFoundException("text");

	// Extract documents with tokens from request
	auto tokens = Requester::instance().tokenize(httpClient_, text);
	tokens = getUnique(tokens);
	auto tokenDocuments = Requester::instance().getDocuments(httpClient_, tokens);

	// Get documents that contains all tokens
	std::vector<std::uint32_t> documents = getValuesIntersection(tokenDocuments);

	// Extract page data from founded documents
	auto pagesData = Requester::instance().getPagesData(pool_, documents);
	std::vector<pageData::PageData> pages{};
	for(auto &[docId, pageData] : pagesData) 
		pages.emplace_back(std::move(pageData));
	
	search::SearchResponseBody responseBody{std::move(pages)};
	return formats::json::ValueBuilder{responseBody}.ExtractValue();
}

std::vector<std::uint32_t> BooleanSearchHandler::getValuesIntersection(std::vector<typename Requester::KeyValues> kv) {
	std::vector<std::uint32_t> values{};
	if(kv.empty()) 
		return values;

	std::vector<std::size_t> indexes(kv.size(), 0);
	std::uint32_t currentMax = 0;
	for(const auto &[_, values] : kv) 
		currentMax = std::max(currentMax, values.empty() ? 0 : values[0]);

	bool isSomeValuesSequenceEnds = false;
	while(!isSomeValuesSequenceEnds) {
		bool allStopsAtCurrentMax = true;
		for(size_t i = 0; i < kv.size(); ++i) {
			while(indexes[i] < kv[i].second.size() && kv[i].second[indexes[i]] < currentMax) 
				++indexes[i];
			if(indexes[i] == kv[i].second.size()) 
				isSomeValuesSequenceEnds = true, allStopsAtCurrentMax = false;
			else if(kv[i].second[indexes[i]] > currentMax) 
				currentMax = kv[i].second[indexes[i]], allStopsAtCurrentMax = false;
		}

		if(allStopsAtCurrentMax) {
			values.emplace_back(kv[0].second[indexes[0]]);
			for(size_t i = 0; i < indexes.size(); ++i) 
				++indexes[i];
		}
	}

	return values;
}
} // namespace SERVICE_NAMESPACE
