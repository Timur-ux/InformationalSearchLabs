#ifndef EXCEPTION_UNDEFINED_TOKEN_HPP_
#define EXCEPTION_UNDEFINED_TOKEN_HPP_
#include <cstdint>
#pragma once
#include <userver/server/handlers/exceptions.hpp>
namespace SERVICE_NAMESPACE::exception {
namespace ush = userver::server::handlers;
class UndefinedTokenException : public ush::ClientError {
	public:
		UndefinedTokenException(std::uint32_t token);
};
} // namespace SERVICE_NAMESPACE::exception

#endif // !EXCEPTION_UNDEFINED_TOKEN_HPP_
