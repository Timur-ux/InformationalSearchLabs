#include "Requester.hpp"
#include "schemas/document.hpp"
#include "schemas/pageData.hpp"
#include "schemas/token.hpp"
#include "schemas/tokenize.hpp"
#include <sstream>
#include <userver/formats/bson.hpp>
#include <userver/formats/bson/binary.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/mongo/options.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <utility>

namespace SERVICE_NAMESPACE {

Requester & Requester::instance() {
	static Requester _instance;
	return _instance;
}
Requester::Requester()
    : tokenizeServiceAddr_(getenv("TOKENIZER_SERVICE_ADDR")),
      dbServiceAddr_(getenv("DB_SERVICE_ADDR")) {
  if (!tokenizeServiceAddr_) {
    LOG_CRITICAL() << "Env variable [TOKENIZER_SERVICE_ADDR] is not set";
    throw std::runtime_error(
        "Env variable [TOKENIZER_SERVICE_ADDR] is not set");
  }

  if (!dbServiceAddr_) {
    LOG_CRITICAL() << "Env variable [DB_SERVICE_ADDR] is not set";
    throw std::runtime_error("Env variable [DB_SERVICE_ADDR] is not set");
  }
}
std::vector<std::uint32_t> Requester::tokenize(clients::http::Client &client,
                                               std::string data) const {
  tokenize::TokenizeRequestBody requestBody{std::move(data)};
  auto requestJson = formats::json::ValueBuilder{requestBody}.ExtractValue();
  auto requestData = formats::json::ToString(requestJson);

  auto request =
      client.CreateRequest()
          .post()
          .url(std::format("{}/{}", tokenizeServiceAddr_, "tokenize"))
          .data(std::move(requestData))
          .retry(1)
          .timeout(kTimeout_ms);

  auto response = request.perform();
  if (response->IsOk())
    return formats::json::FromString(std::move(*response).body())
        .As<std::vector<std::uint32_t>>();

  LOG_ERROR() << "Request on endpoint " << request.GetUrl()
              << " return bad response: " << response->status_code() << "\t"
              << response->body_view();
  return {};
}

std::vector<Requester::KeyValues>
Requester::getTokens(clients::http::Client &client,
                     std::vector<std::uint32_t> documentIds) const {
  if (documentIds.empty())
    return {};

  std::ostringstream oss;
  oss << dbServiceAddr_ << "/token?id=" << documentIds[0];
  for (size_t i = 1; i < documentIds.size(); ++i)
    oss << "&id=" << documentIds[i];
  auto request =
      client.CreateRequest().get().url(oss.str()).retry(1).timeout(kTimeout_ms);

  auto response = request.perform();
  if (!response->IsOk()) {
    LOG_ERROR() << "Request on endpoint " << request.GetUrl()
                << " return bad response: " << response->status_code() << "\t"
                << response->body_view();
    return {};
  }

  auto responseBody = formats::json::FromString(std::move(*response).body())
                          .As<token::TokenResponseBody>();
  if (responseBody.size() != documentIds.size())
    LOG_WARNING() << "[REQUESTER] getTokens: returned documents have size not "
                     "equal requested";

  std::vector<Requester::KeyValues> kv{};
  kv.reserve(responseBody.size());
  for (auto &[documentId, tokenIds] : responseBody) {
    if (!documentId.has_value()) {
      LOG_WARNING() << "[REQUESTER] getTokens: document id have no value";
      continue;
    }
    if (!tokenIds.has_value()) {
      LOG_WARNING() << "[REQUESTER] getTokens: token ids vector have no value";
      continue;
    }
    kv.emplace_back(Requester::KeyValues{*documentId, *tokenIds});
  }
  return kv;
}

std::vector<Requester::KeyValues>
Requester::getDocuments(clients::http::Client &client,
                        std::vector<std::uint32_t> tokenIds) const {
  if (tokenIds.empty())
    return {};

  std::ostringstream oss;
  oss << dbServiceAddr_ << "/document?id=" << tokenIds[0];
  for (size_t i = 1; i < tokenIds.size(); ++i)
    oss << "&id=" << tokenIds[i];
  auto request =
      client.CreateRequest().get().url(oss.str()).retry(1).timeout(kTimeout_ms);

  auto response = request.perform();
  if (!response->IsOk()) {
    LOG_ERROR() << "Request on endpoint " << request.GetUrl()
                << " return bad response: " << response->status_code() << "\t"
                << response->body_view();
    return {};
  }

  auto responseBody = formats::json::FromString(std::move(*response).body())
                          .As<document::DocumentsResponseBody>();
  if (responseBody.size() != tokenIds.size())
    LOG_WARNING() << "[REQUESTER] getDocuments: returned tokens have size not "
                     "equal requested";

  std::vector<Requester::KeyValues> kv{};
  kv.reserve(responseBody.size());
  for (auto &[tokenId, documentIds] : responseBody) {
    if (!tokenId.has_value()) {
      LOG_WARNING() << "[REQUESTER] getDocuments: document id have no value";
      continue;
    }
    if (!documentIds.has_value()) {
      LOG_WARNING()
          << "[REQUESTER] getDocuments: token ids vector have no value";
      continue;
    }
    kv.emplace_back(Requester::KeyValues{*tokenId, *documentIds});
  }
  return kv;
}

std::vector<std::pair<std::uint32_t, pageData::PageData>>
Requester::getPagesData(storages::mongo::PoolPtr mongo,
                        std::vector<std::uint32_t> documentIds) const {
  using formats::bson::MakeDoc;
  namespace options = storages::mongo::options;

  auto transaction = mongo->GetCollection("ParsedDocuments");
  options::Projection projection;
  projection.Exclude("raw");
  auto cursor = transaction.Find(
      MakeDoc("_id", MakeDoc("$in", documentIds)), projection,
      options::Sort{std::make_pair("timestamp", options::Sort::kDescending)});

  if (!cursor)
    return {};
  std::vector<std::pair<std::uint32_t, pageData::PageData>> pages{};
  for (const auto &doc : cursor) {
		LOG_DEBUG() << "DOC: " << formats::bson::ToBinaryString(doc).ToString();
    pages.emplace_back(doc["_id"].As<std::uint32_t>(),
                       pageData::PageData{doc["title"].As<std::string>(),
                                          doc["url"].As<std::string>()});
	}

	return pages;
}
} // namespace SERVICE_NAMESPACE
