#include "exceptions/PageNotFound.hpp"
#include <cstdint>
#include <format>
#include <userver/server/handlers/exceptions.hpp>

namespace SERVICE_NAMESPACE::exception {
PageNotFoundException::PageNotFoundException(std::uint32_t documentId)
    : ush::ResourceNotFound(ush::ExternalBody{
          std::format("Page with id: {} not found in DB", documentId)}) {}
} // namespace SERVICE_NAMESPACE::exception
