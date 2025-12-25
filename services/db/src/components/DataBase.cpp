#include "components/DataBase.hpp"
#include "Vector.hpp"
#include "factory/BPlusTreeFactory.hpp"
#include <cstring>
#include <random>
#include <userver/logging/log.hpp>
namespace SERVICE_NAMESPACE {
DataBase::DataBase() {
  static char tempStoragePath[30];
  static std::random_device device;
  const char *storagePath = getenv("STORAGE_PATH");
  if (!storagePath) {
    LOG_WARNING() << "environment variable [STORAGE PATH] not set, so i use "
                     "random generated directory in /tmp folder";
    strcpy(tempStoragePath, "/tmp/");
    for (size_t i = 0; i < 24; ++i)
      tempStoragePath[i + 5] = device() % 26 + 'a';
    tempStoragePath[29] = 0;
    storagePath = tempStoragePath;
  }

  documentTokens_ =
      IR::bplustree::FileBasedBPlusTreeFactory<
          uint32_t, uint32_t, IR::bplustree::SameKeyOrdering::AsInserted>(
          1024, storagePath, "documentTokens")
          .createTree();

  tokenDocuments_ =
      IR::bplustree::FileBasedBPlusTreeFactory<
          uint32_t, uint32_t, IR::bplustree::SameKeyOrdering::Increase>(
          1024, storagePath, "tokenDocuments")
          .createTree();
}

void DataBase::insert(id_t documentId, const IR::VectorView<id_t> tokens) {
  for (const id_t &token : tokens)
    documentTokens_->insert(documentId, token),
        tokenDocuments_->insert(token, documentId);
  LOG_DEBUG() << "Inserted document id: " << documentId
              << "; tokens amount: " << tokens.size();
}

IR::Vector<id_t> DataBase::findTokensByDocumentId(id_t documentId) {
  auto cursor = documentTokens_->find(documentId);
  IR::Vector<id_t> result;
  for (auto [docId, tokenId] : cursor)
    result.emplace_back(tokenId);

  LOG_DEBUG() << "For document id: " << documentId << " found " << result.size()
              << " tokens";
  return result;
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
