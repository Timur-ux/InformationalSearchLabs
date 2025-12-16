#ifndef DATABASE_HPP_
#define DATABASE_HPP_
#include "BPlusTree.hpp"
#include "Vector.hpp"
#include <cstdint>
#include <memory>
#pragma once
namespace SERVICE_NAMESPACE {
class DataBase {
public:
	using id_t = uint32_t;
	private:
	DataBase();
	std::shared_ptr<IR::bplustree::BPlusTree<id_t, id_t, IR::bplustree::SameKeyOrdering::AsInserted>> documentTokens_;
	std::shared_ptr<IR::bplustree::BPlusTree<id_t, id_t, IR::bplustree::SameKeyOrdering::Increase>> tokenDocuments_;
public:
	static DataBase& instance();

	void insert(id_t documentId, const IR::VectorView<id_t> tokens);
	IR::Vector<id_t> findTokensByDocumentId(id_t documentId);
	IR::Vector<id_t> findDocumentsByTokenId(id_t tokenId);
};
} // namespace dbService
#endif // !DATABASE_HPP_
