#ifndef B_PLUS_TREE_FACTORY_HPP_
#define B_PLUS_TREE_FACTORY_HPP_
#include "bplustree/BPlusTree.hpp"
#include "bplustree/INodeCache.hpp"
#include "bplustree/NodeManager/FileBased.hpp"
#include "bplustree/NodeManager/InMemory.hpp"
#include "bplustree/key_type.hpp"
#include "concepts.hpp"
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#pragma once
namespace IR::bplustree {
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class IBPlusTreeFactory {
public:
  using tree_type = BPlusTree<TKey, TVal, TOrdering>;
  using manager_type = INodeManager<TKey, TVal, TOrdering>;

  virtual std::shared_ptr<INodeManager<TKey, TVal, TOrdering>>
  createNodeManager() = 0;
  virtual std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>> createTree() = 0;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class InMemoryBPlusTreeFactory
    : public IBPlusTreeFactory<TKey, TVal, TOrdering> {
  using parent_type = IBPlusTreeFactory<TKey, TVal, TOrdering>;
  using manager_type = parent_type::manager_type;
  using tree_type = parent_type::tree_type;

  size_t nodeCapacity_;

public:
  InMemoryBPlusTreeFactory(size_t nodeCapacity = 1024)
      : nodeCapacity_(nodeCapacity) {}

  std::shared_ptr<INodeManager<TKey, TVal, TOrdering>>
  createNodeManager() override final;
  std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>> createTree() override final;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class FileBasedBPlusTreeFactory
    : public IBPlusTreeFactory<TKey, TVal, TOrdering> {
  size_t nodeCapacity_;
  long rootId_ = -1;
  size_t cacheCapacity_ = 1024;
  std::filesystem::path storagePath_ = "/tmp/";

  std::shared_ptr<FileBasedNodeManager<TKey, TVal, TOrdering>>
  createNodeManager_();

public:
  /**
   * @brief Construct file storage at temporary folder /tmp/ with random storage
   * name
   */
  FileBasedBPlusTreeFactory(size_t nodeCapacity) : nodeCapacity_(nodeCapacity) {
    static char name[21];
    static std::random_device device;
    for (size_t i = 0; i < 20; ++i)
      name[i] = char(device() % 26 + 'a');
    name[20] = 0;
    storagePath_ /= name;
  }

  /**
   * @brief Construct file storage at given path
   *
   * @param storagePath path to storage folder
   * @param name name of storage
   *
   * Storage files will be placed at storagePath/name/
   */
  FileBasedBPlusTreeFactory(size_t nodeCapacity,
                            std::filesystem::path storagePath, const char *name)
      : nodeCapacity_(nodeCapacity), storagePath_(storagePath) {
    storagePath_ /= name;
  }

  FileBasedBPlusTreeFactory(size_t nodeCapacity,
                            std::filesystem::path storagePath, const char *name,
                            size_t cacheCapacity)
      : FileBasedBPlusTreeFactory(nodeCapacity, storagePath, name) {
    cacheCapacity_ = cacheCapacity;
  }

  std::shared_ptr<INodeManager<TKey, TVal, TOrdering>>
  createNodeManager() override final;
  std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>> createTree() override final;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>>
InMemoryBPlusTreeFactory<TKey, TVal, TOrdering>::createTree() {
  return std::make_shared<tree_type>(createNodeManager(), -1, nodeCapacity_);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<INodeManager<TKey, TVal, TOrdering>>
InMemoryBPlusTreeFactory<TKey, TVal, TOrdering>::createNodeManager() {
  using namespace std::filesystem;
  return std::make_shared<InMemoryNodeManager<TKey, TVal, TOrdering>>();
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>>
FileBasedBPlusTreeFactory<TKey, TVal, TOrdering>::createTree() {
  auto nodeManager = createNodeManager_();
  auto tree = std::make_shared<BPlusTree<TKey, TVal, TOrdering>>(
      nodeManager, rootId_, nodeCapacity_);
  nodeManager->bindTo(tree);

  return tree;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<INodeManager<TKey, TVal, TOrdering>>
FileBasedBPlusTreeFactory<TKey, TVal, TOrdering>::createNodeManager() {
  return createNodeManager_();
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
std::shared_ptr<FileBasedNodeManager<TKey, TVal, TOrdering>>
FileBasedBPlusTreeFactory<TKey, TVal, TOrdering>::createNodeManager_() {
  using namespace std::filesystem;

  using NodeManager = FileBasedNodeManager<TKey, TVal, TOrdering>;
  std::shared_ptr<INodeCache<TKey, TVal, TOrdering>> cache = nullptr;
  if (cacheCapacity_ > 0)
    cache = std::make_shared<DefaultNodeCache<TKey, TVal, TOrdering>>(
        storagePath_, cacheCapacity_);
  else
    cache = std::make_shared<NullNodeCache<TKey, TVal, TOrdering>>();

  if (create_directories(storagePath_))
    std::cerr << "Created new storage created at: " << storagePath_ << '\n';
  else
    std::cerr << "Using already existing storage at: " << storagePath_ << '\n';

  path schemaPath = storagePath_ / NodeManager::schemaName;

  std::shared_ptr<NodeManager> nodeManager;
  if (exists(schemaPath)) {
    std::cerr << "Found schema file for [" << storagePath_
              << "] storage. Node manager data will be loaded from it\n";
    FILE *file = fopen(schemaPath.c_str(), "rb");
    if (!file)
      throw std::runtime_error("Can't open schema file");

    typename NodeManager::Schema schema;
    if (fread(&schema, sizeof(schema), 1, file) != 1)
      throw std::runtime_error("Schema file is invalid");
    fclose(file);

    nodeManager = std::make_shared<NodeManager>(storagePath_, schema, cache);
    rootId_ = schema.rootId;
  } else {
    std::cerr << "Schema not found, so i create clear node manager\n";
    nodeManager = std::make_shared<NodeManager>(storagePath_, cache);
  }
  return nodeManager;
}
} // namespace IR::bplustree
#endif // !B_PLUS_TREE_FACTORY_HPP_
