#ifndef TOKEN_HANDLER_HPP_
#define TOKEN_HANDLER_HPP_

#include "Vector.hpp"
#include <chrono>
#include <string_view>
#include <userver/clients/http/client.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/http/request.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/components/component_config.hpp>
#include <userver/formats/bson/document.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/storages/mongo/pool.hpp>
#include <userver/yaml_config/schema.hpp>
#include <vector>

namespace SERVICE_NAMESPACE {
using namespace userver;
class Indexer final : public components::ComponentBase {
  storages::mongo::PoolPtr pool_;
  clients::http::Client &httpClient_;

	int batchSize_;
	long batchDelay_;
	const unsigned long kTimeout_ms = 300000;


	const char * tokenizeServiceAddr_;
	const char * dbServiceAddr_;

  void indexPage(formats::bson::Document doc) const;
  std::vector<std::uint32_t> tokenize(std::string data) const;
  void insertTokensToDB(std::uint32_t pageId, std::vector<std::uint32_t> tokens) const;

	void taskFunc();


public:
  static constexpr std::string_view kName = "indexer";
	static yaml_config::Schema GetStaticConfigSchema();


  Indexer(const components::ComponentConfig &config,
               const components::ComponentContext &context);
};
} // namespace SERVICE_NAMESPACE

#endif // !TOKEN_HANDLER_HPP_
