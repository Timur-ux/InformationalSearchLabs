#ifndef EXCEPTION_KEY_NOT_FOUND_HPP_
#define EXCEPTION_KEY_NOT_FOUND_HPP_
#include <userver/server/handlers/exceptions.hpp>
#pragma once

namespace SERVICE_NAMESPACE::exception {
	namespace ush = userver::server::handlers;
	class KeyNotFoundException : public ush::RequestParseError {
		public:
		KeyNotFoundException(const char * key);
	};
} // namespace SERVICE_NAMESPACE::exception

#endif // !EXCEPTION_KEY_NOT_FOUND_HPP_
