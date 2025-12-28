#include "components/index.hpp"
#include "Vector.hpp"
#include "schemas/insert.hpp"
#include "schemas/tokenize.hpp"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <format>
#include <stdexcept>
#include <userver/components/component_base.hpp>
#include <userver/engine/sleep.hpp>
#include <userver/engine/task/cancel.hpp>
#include <userver/formats/bson.hpp>
#include <userver/formats/bson/document.hpp>
#include <userver/formats/bson/inline.hpp>
#include <userver/formats/json/serialize.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/content_type.hpp>
#include <userver/http/http_version.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/mongo/write_result.hpp>
#include <userver/utils/async.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

#include "exceptions/KeyNotFound.hpp"

namespace SERVICE_NAMESPACE {
Indexer::Indexer(const components::ComponentConfig &config,
                 const components::ComponentContext &context)
    : ComponentBase(config, context),
      pool_(context.FindComponent<components::Mongo>("mongo-index").GetPool()),
      httpClient_(
          context.FindComponent<components::HttpClient>("index-http-client")
              .GetHttpClient()),
      batchSize_(config["batch-size"].As<int>()),
      batchDelay_(config["batch-delay"].As<long>()) {
  if (batchSize_ <= 0)
    throw std::invalid_argument("Batch size must be positive");
  if (batchDelay_ <= 0)
    throw std::invalid_argument("Batch delay must be positive");

  tokenizeServiceAddr_ = getenv("TOKENIZER_SERVICE_ADDR");
  if (!tokenizeServiceAddr_) {
    LOG_CRITICAL() << "Env variable [TOKENIZER_SERVICE_ADDR] is not set";
    throw std::runtime_error(
        "Env variable [TOKENIZER_SERVICE_ADDR] is not set");
  }

  dbServiceAddr_ = getenv("DB_SERVICE_ADDR");
  if (!dbServiceAddr_) {
    LOG_CRITICAL() << "Env variable [DB_SERVICE_ADDR] is not set";
    throw std::runtime_error("Env variable [DB_SERVICE_ADDR] is not set");
  }

  const auto taskProcessorName = config["task-processor"].As<std::string>();
  auto &taskProcessor = context.GetTaskProcessor(taskProcessorName);
  utils::Async(taskProcessor, "indexer/task", [this]() {
    this->taskFunc();
  }).Get();
}

void Indexer::taskFunc() {
  using formats::bson::MakeDoc;
  LOG_DEBUG() << "Task started";

  while (true) {
    LOG_DEBUG() << "Task await for " << batchDelay_ << " seconds";
    engine::InterruptibleSleepFor(std::chrono::seconds(batchDelay_));
    if (engine::current_task::ShouldCancel()) {
      LOG_DEBUG() << "Task awaiting stops by outer signal";
      break;
    }

    LOG_DEBUG() << "Fetching " << batchSize_ << " pages needs to index...";
    IR::Vector<formats::bson::Document> docs{};
    try {
      auto collection = pool_->GetCollection("ParsedDocuments");

      for (int i = 0; i < batchSize_; ++i) {
        storages::mongo::WriteResult item =
            collection.FindAndModify(MakeDoc("indexed", false),
                                     MakeDoc("$set", MakeDoc("indexed", true)));
        if (item.MatchedCount() == 0)
          break;

        auto errors = item.ServerErrors();
        if (!errors.empty()) {
          for (const auto &[id, error] : errors)
            LOG_WARNING() << "Error while performing FindAndModify op. id: "
                          << id << "; Error: " << error.Message();
          break;
        }
        auto doc = item.FoundDocument();
        if (!doc.has_value()) {
          LOG_WARNING() << "Item found but doc have no value";
          break;
        }

        docs.push_back(*doc);
      }
    } catch (std::exception &e) {
      LOG_ERROR() << "Error was occured while fetching pages from mongo, stop "
                     "fetching. Error: "
                  << e.what();
    } catch (...) {
      LOG_CRITICAL()
          << "Undefined error was occured while fetching pages from mongo";
      throw;
    }
    LOG_DEBUG() << "Found " << docs.size() << "/" << batchSize_
                << " documents, updating...";
    for (auto &doc : docs) {
      try {
        indexPage(doc);
      } catch (std::exception &e) {
        LOG_ERROR() << "Error was occured while indexng page, "
                       "skip this page indexing. Error: "
                    << e.what();
      } catch (...) {
        LOG_CRITICAL() << "Undefined error was occured while indexng page";
        throw;
      }
    }
    LOG_DEBUG() << "Updated " << docs.size() << " pages";
  }
}

void Indexer::indexPage(formats::bson::Document doc) const {
  auto transaction = pool_->GetCollection("ParsedDocuments");
  using formats::bson::MakeDoc;

  auto content = doc["raw"].As<std::string>();
  auto tokens = tokenize(content);
  auto pageId = doc["_id"].As<std::uint32_t>();
  insertTokensToDB(pageId, tokens);
  LOG_DEBUG() << "Page with id: " << pageId
              << " have raw content length: " << content.size()
              << " and tokens amount: " << tokens.size();
}

std::vector<std::uint32_t> Indexer::tokenize(std::string data) const {
  tokenize::TokenizeRequestBody requestBody{std::move(data)};
  auto requestJson = formats::json::ValueBuilder{requestBody}.ExtractValue();
  auto requestData = formats::json::ToString(requestJson);

  auto request =
      httpClient_.CreateRequest()
          .post()
          .url(std::format("{}/{}", tokenizeServiceAddr_, "tokenize"))
          .data(std::move(requestData))
          .retry(1)
          .timeout(kTimeout_ms);

  auto res = request.perform();
  if (res->IsOk())
    return formats::json::FromString(std::move(*res).body())
        .As<std::vector<std::uint32_t>>();

  LOG_ERROR() << "Request on endpoint " << request.GetUrl()
              << " return bad response: " << res->status_code() << "\t"
              << res->body_view();
  return {};
}

void Indexer::insertTokensToDB(std::uint32_t pageId,
                               std::vector<std::uint32_t> tokens) const {
  insert::InsertRequestBody requestBody{pageId, std::move(tokens)};
  auto requestJson = formats::json::ValueBuilder{requestBody}.ExtractValue();
  auto requestData = formats::json::ToString(requestJson);

  auto request = httpClient_.CreateRequest()
                     .post()
                     .url(std::format("{}/{}", dbServiceAddr_, "insert"))
                     .data(std::move(requestData))
                     .retry(1)
                     .timeout(kTimeout_ms);

  auto res = request.perform();
  if (res->IsError())
    LOG_ERROR() << "Request on endpoint " << request.GetUrl()
                << " return bad response: " << res->status_code() << "\t"
                << res->body_view();
}

yaml_config::Schema Indexer::GetStaticConfigSchema() {
  return yaml_config::MergeSchemas<components::ComponentBase>(R"(
type: object
description: Periodically pull new pages from mongo and indexing it
additionalProperties: false
properties:
  task-processor:
    type: string
    description: name of the task processor the indexer will run on
  batch-size:
    type: integer
    description: number of pages pulled per circle
  batch-delay:
    type: integer
    description: delay between pulls in seconds
)");
}
} // namespace SERVICE_NAMESPACE
