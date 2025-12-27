#include "components/DataBase.hpp"
#include "bplustree/key_type.hpp"
#include "Vector.hpp"
#include "algo.hpp"
#include "factory/BPlusTreeFactory.hpp"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <userver/logging/log.hpp>
namespace SERVICE_NAMESPACE {
DataBase::DataBase() {
  static char tempStoragePath[30];
  static std::random_device device;
  const char *storagePath = getenv("STORAGE_PATH");
  if (!storagePath) {
    LOG_WARNING() << "environment variable [STORAGE_PATH] not set, so i use "
                     "random generated directory in /tmp folder";
    strcpy(tempStoragePath, "/tmp/");
    for (size_t i = 0; i < 24; ++i)
      tempStoragePath[i + 5] = device() % 26 + 'a';
    tempStoragePath[29] = 0;
    storagePath = tempStoragePath;
  }

  const char *cacheCapacityStr = getenv("CACHE_CAPACITY");
  size_t cacheCapacity = 1024;
  if (!cacheCapacityStr)
    LOG_WARNING() << "environment variable [CACHE_CAPACITY] not set, so i use "
                     "default value is"
                  << cacheCapacity;
  else {
    long _cacheCapacity = std::stol(cacheCapacityStr);
    if (_cacheCapacity < 0)
      LOG_WARNING()
          << "negative value for [CACHE_CAPACITY] not allowed, so i use "
             "default value is"
          << cacheCapacity;
    else if (_cacheCapacity > std::numeric_limits<std::uint32_t>::max())
      LOG_WARNING() << "value for [CACHE_CAPACITY] bigger that "
                    << std::numeric_limits<std::uint32_t>::max()
                    << "not allowed, so i use "
                       "default value is"
                    << cacheCapacity;
    else
      cacheCapacity = _cacheCapacity;
  }

  documentTokens_ =
      IR::bplustree::FileBasedBPlusTreeFactory<
          uint32_t, uint32_t, IR::bplustree::SameKeyOrdering::AsInserted>(
          1024, storagePath, "documentTokens", cacheCapacity)
          .createTree();

  tokenDocuments_ =
      IR::bplustree::FileBasedBPlusTreeFactory<
          uint32_t, uint32_t, IR::bplustree::SameKeyOrdering::Increase>(
          1024, storagePath, "tokenDocuments", cacheCapacity)
          .createTree();
}

void DataBase::insert(id_t documentId, const IR::VectorView<id_t> tokens) {
  std::vector<std::uint32_t> tokens_{};
  for (const id_t &token : tokens)
    documentTokens_->insert(documentId, token), tokens_.push_back(token);
  tokens_ = IR::algo::getUnique(tokens_);
  for (std::uint32_t tokenId : tokens_)
    tokenDocuments_->insert(tokenId, documentId);
  LOG_DEBUG() << "Inserted document id: " << documentId
              << "; tokens amount: " << tokens.size()
              << "; unique tokens: " << tokens_.size();
}

IR::Vector<id_t> DataBase::findTokensByDocumentId(id_t documentId) {
  try {
    auto cursor = documentTokens_->find(documentId);
    IR::Vector<id_t> result;
    for (auto [docId, tokenId] : cursor)
      result.emplace_back(tokenId);

    LOG_DEBUG() << "For document id: " << documentId << " found "
                << result.size() << " tokens";
    return result;
  } catch (std::exception &e) {
    LOG_ERROR() << "ERROR in find tokens by doc id:" << e.what();
    return {};
  }
}

IR::Vector<id_t> DataBase::findDocumentsByTokenId(id_t tokenId) {
  auto cursor = tokenDocuments_->find(tokenId);
  IR::Vector<id_t> result;
  for (auto [tokenId, docId] : cursor)
    result.emplace_back(docId);

  LOG_DEBUG() << "For token id: " << tokenId << " found " << result.size()
              << " documents";
  return result;
}

DataBase &DataBaseComponent::GetDataBase() { return db_; }
} // namespace SERVICE_NAMESPACE
