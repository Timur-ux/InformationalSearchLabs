#ifndef EXCEPTION_KEY_NOT_FOUND_HPP_
#define EXCEPTION_KEY_NOT_FOUND_HPP_
#include <userver/server/handlers/exceptions.hpp>
#pragma once
namespace SERVICE_NAMESPACE::exception {
class KeyNotFound : public userver::server::handlers::ResourceNotFound {
public:
	KeyNotFound(const char * key);
};
} // namespace dbService::exception 
#endif // !EXCEPTION_KEY_NOT_FOUND_HPP_
