#ifndef REQUESTER_HPP_
#define REQUESTER_HPP_
#include "schemas/pageData.hpp"
#include <userver/clients/http/client.hpp>
#include <userver/logging/log.hpp>
#include <userver/storages/mongo.hpp>
#include <userver/storages/mongo/pool.hpp>
#include <vector>
#pragma once

namespace SERVICE_NAMESPACE {
using namespace userver;
class Requester {
  const char *tokenizeServiceAddr_;
  const char *dbServiceAddr_;
	const unsigned long kTimeout_ms = 10000;

  Requester();

public:
  static Requester &instance();

  using KeyValues = std::pair<std::uint32_t, std::vector<std::uint32_t>>;
	using token_t = std::uint32_t;
	using document_t = std::uint32_t;

  std::vector<token_t> tokenize(clients::http::Client &client,
                                      std::string data) const;
  std::vector<KeyValues> getTokens(clients::http::Client &client,
                                   std::vector<std::uint32_t> documentIds) const;
  std::vector<KeyValues> getDocuments(clients::http::Client &client,
                                      std::vector<std::uint32_t> tokenIds) const;


  std::vector<std::pair<document_t, pageData::PageData>>
  getPagesData(storages::mongo::PoolPtr mongo,
          std::vector<std::uint32_t> documentIds) const;
};
} // namespace SERVICE_NAMESPACE
#endif // !REQUESTER_HPP_
