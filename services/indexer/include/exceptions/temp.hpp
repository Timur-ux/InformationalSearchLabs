#ifndef EXCEPTION_TEMP_HPP_
#define EXCEPTION_TEMP_HPP_
#include <userver/server/handlers/exceptions.hpp>
#pragma once

namespace SERVICE_NAMESPACE::exception {
	class TempException : userver::server::handlers::ClientError {
		public:
		TempException();
	};
} // namespace SERVICE_NAMESPACE::exception

#endif // !EXCEPTION_TEMP_HPP_
