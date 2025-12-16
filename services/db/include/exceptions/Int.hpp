#ifndef EXCEPTION_INT_HPP_
#define EXCEPTION_INT_HPP_
#include <userver/server/handlers/exceptions.hpp>
#pragma once

namespace SERVICE_NAMESPACE::exception {
class IdOutOfRange : public userver::server::handlers::RequestParseError {
	public:
		IdOutOfRange(const std::string & id);
};

class IdInvalid : public userver::server::handlers::RequestParseError {
	public:
		IdInvalid(const std::string & id);
};
} // namespace SERVICE_NAMESPACE::exception

#endif // !EXCEPTION_INT_HPP_
