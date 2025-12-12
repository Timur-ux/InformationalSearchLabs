#ifndef B_PLUS_TREE_FACTORY_HPP_
#define B_PLUS_TREE_FACTORY_HPP_
#include "BPlusTree.hpp"
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#pragma once
namespace IR::bplustree {
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class BPlusTreeFactory {
  using tree_type = BPlusTree<TKey, TVal, TOrdering>;

public:
  static std::shared_ptr<tree_type> createInMemory(size_t nodeCapacity = 1024);
  static std::shared_ptr<tree_type>
  createFileBased(const std::string &storageName, size_t nodeCapacity = 8192);
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>>
BPlusTreeFactory<TKey, TVal, TOrdering>::createInMemory(size_t nodeCapacity) {
  return BPlusTree<TKey, TVal, TOrdering>(
      std::make_unique<InMemoryNodeManager<TKey, TVal, TOrdering>>(), 0,
      nodeCapacity);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>>
BPlusTreeFactory<TKey, TVal, TOrdering>::createFileBased(
    const std::string &storageName, size_t nodeCapacity) {
  using namespace std::filesystem;
  using NodeManager = FileBasedNodeManager<TKey, TVal, TOrdering>;
  const char *storagePath = getenv("DATABASE_STORAGE_PATH");
  if (!storagePath)
    throw std::invalid_argument("Environment variable [DATABASE_STORAGE_PATH] "
                                "not setted, i don't know where store files");

  path currentStoragePath = storagePath;
  currentStoragePath /= storageName;

  if (fs::create_directories(currentStoragePath))
    std::cerr << "Storage for [" << storageName
              << "] created at: " << currentStoragePath << '\n';
  else
    std::cerr << "Storage for [" << storageName
              << "] already created at: " << currentStoragePath << '\n';

  fs::path schemaPath = currentStoragePath / NodeManager::schemaName;

  std::shared_ptr<NodeManager> nodeManager;
	long initialRootId = -1;
  if (fs::exists(schemaPath)) {
    std::cerr << "Found schema file for [" << storageName
              << "] storage. Node manager data will be loaded from it\n";
    FILE *file = fopen(schemaPath.c_str(), "rb");
    typename NodeManager::Schema schema;
    if (fread(&schema, sizeof(schema), 1, file) != 1)
      throw std::runtime_error("Schema file is invalid");
    fclose(file);

    nodeManager = std::make_shared<NodeManager>(currentStoragePath, schema);
		initialRootId = schema.rootId;
  } else {
    std::cerr << "Schema not found, so i create clear node manager\n";
    nodeManager = std::make_shared<NodeManager>(currentStoragePath);
  }

	auto tree = std::make_shared<BPlusTree<TKey, TVal, TOrdering>>(nodeManager, initialRootId, nodeCapacity);
	nodeManager->bindTo(tree);

	return tree;
}

} // namespace IR::bplustree
#endif // !B_PLUS_TREE_FACTORY_HPP_
