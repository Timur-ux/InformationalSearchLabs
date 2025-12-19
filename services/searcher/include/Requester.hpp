#ifndef REQUESTER_HPP_
#define REQUESTER_HPP_
#include <userver/clients/http/client.hpp>
#pragma once

namespace SERVICE_NAMESPACE {
using namespace userver;
class Requester {
	const char * tokenizeServiceAddr_;
	const char * dbServiceAddr_;

	Requester();
public:
	Requester & instance();

  std::vector<std::uint32_t> tokenize(clients::http::Client& client, std::string data) const;

};
} // namespace SERVICE_NAMESPACE
#endif // !REQUESTER_HPP_
