#ifndef EXCEPTION_DOCUMENT_NOT_FOUND_HPP_
#define EXCEPTION_DOCUMENT_NOT_FOUND_HPP_
#include <userver/server/handlers/exceptions.hpp>
#pragma once

namespace SERVICE_NAMESPACE::exception {
	namespace ush = userver::server::handlers;
	class PageNotFoundException : public ush::ResourceNotFound {
		public:
		PageNotFoundException(std::uint32_t documentId);
	};
} // namespace SERVICE_NAMESPACE::exception
#endif // !EXCEPTION_DOCUMENT_NOT_FOUND_HPP_
