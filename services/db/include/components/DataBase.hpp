#ifndef DATABASE_HPP_
#define DATABASE_HPP_
#include "Vector.hpp"
#include "bplustree/BPlusTree.hpp"
#include <cstdint>
#include <memory>
#include <string_view>
#include <userver/components/component_base.hpp>
#pragma once
namespace SERVICE_NAMESPACE {
class DataBase {
public:
  using id_t = uint32_t;

private:
  DataBase();
  std::shared_ptr<IR::bplustree::BPlusTree<
      id_t, id_t, IR::bplustree::SameKeyOrdering::AsInserted>>
      documentTokens_;
  std::shared_ptr<IR::bplustree::BPlusTree<
      id_t, id_t, IR::bplustree::SameKeyOrdering::Increase>>
      tokenDocuments_;

  friend class DataBaseComponent;

public:

  void insert(id_t documentId, const IR::VectorView<id_t> tokens);
  IR::Vector<id_t> findTokensByDocumentId(id_t documentId);
  IR::Vector<id_t> findDocumentsByTokenId(id_t tokenId);
};

using namespace userver;
class DataBaseComponent : public components::ComponentBase {
  DataBase db_;

public:
	static constexpr std::string_view kName = "database";

	using ComponentBase::ComponentBase;
	using ComponentBase::GetStaticConfigSchema;

	DataBase &GetDataBase();
};
} // namespace SERVICE_NAMESPACE
#endif // !DATABASE_HPP_
