#ifndef BPLUSTREE_FILE_BASED_NODE_MANAGER_HPP_
#define BPLUSTREE_FILE_BASED_NODE_MANAGER_HPP_
#include "bplustree/BPlusTree.hpp"
#include "bplustree/INodeCache.hpp"
#include "bplustree/Node.hpp"
#include "bplustree/fwd.hpp"
#include "concepts.hpp"
#include "event.hpp"
#include <filesystem>
#pragma once
namespace IR::bplustree {
namespace fs = std::filesystem;
/**
 * @brief Stores nodes on the files
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class FileBasedNodeManager : public INodeManager<TKey, TVal, TOrdering> {
public:
  struct Schema {
    long nextFreeId = 0;
    long rootId = -1;
  };

private:
  using Node = impl::Node<TKey, TVal, TOrdering>;
  struct SchemaUpdater : public event::IEventHandler<long> {
    Schema *schema;
    SchemaUpdater(Schema *schema) : schema(schema) {}
    void operator()(long &&rootId) override final { schema->rootId = rootId; }
  };

  fs::path storagePath_;
  Schema schema_;
  std::shared_ptr<SchemaUpdater> schemaUpdater_;
  std::shared_ptr<INodeCache<TKey, TVal, TOrdering>> cache_ =
      std::make_shared<NullNodeCache<TKey, TVal, TOrdering>>();

public:
  FileBasedNodeManager(
      const fs::path &storagePath,
      std::shared_ptr<INodeCache<TKey, TVal, TOrdering>> cache = nullptr)
      : storagePath_(storagePath),
        schemaUpdater_(std::make_shared<SchemaUpdater>(&schema_)) {
    if (cache)
      cache_ = cache;
  }

  FileBasedNodeManager(
      const fs::path &storagePath, Schema loadedSchema,
      std::shared_ptr<INodeCache<TKey, TVal, TOrdering>> cache = nullptr)
      : FileBasedNodeManager(storagePath, cache) {
    schema_ = loadedSchema;
  }

  constexpr static const char *schemaName = "schema";

  Node load(long id) override;
  Node load(long id, long parentId) override;
  void save(const Node &node) override;
  Node create(bool isLeaf, long parentId, size_t level,
              long nextNodeId = -1) override;

  void bindTo(std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>> tree);

  ~FileBasedNodeManager();
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void FileBasedNodeManager<TKey, TVal, TOrdering>::bindTo(
    std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>> tree) {
  tree->onDestroy += schemaUpdater_;
  tree->rootId_ = schema_.rootId;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::~FileBasedNodeManager() {
  if (cache_)
    cache_->flush(storagePath_);

  FILE *file = fopen((storagePath_ / schemaName).c_str(), "wb");
  if (!file)
    std::cerr << "Can't open schema file\n";
  else if (fwrite(&schema_, sizeof(Schema), 1, file) != 1)
    std::cerr << "Can't write schema data to file\n";
  cache_->flush(storagePath_);

  if (file)
    fclose(file);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
impl::Node<TKey, TVal, TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::create(bool isLeaf, long parentId,
                                                    size_t level,
                                                    long nextNodeId) {
  return Node{.id = schema_.nextFreeId++,
              .level = level,
              .isLeaf = isLeaf,
              .keys = {},
              .links = {},
              .values = {},
              .nextNodeId = nextNodeId,
              .parentId = parentId};
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
impl::Node<TKey, TVal, TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::load(long id, long parentId) {
  Node node = load(id);
  if (node.parentId != parentId)
    node.parentId = parentId, save(node);

  return node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
impl::Node<TKey, TVal, TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::load(long id) {
  Node node;
  if (!cache_->contains(id))
    cache_->insert(node = Node::load(storagePath_, id));
  else
    node = cache_->get(id);

  return node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void FileBasedNodeManager<TKey, TVal, TOrdering>::save(const Node &node) {
  if (cache_->contains(node.id))
    cache_->update(node);
  else if (!cache_->full())
    cache_->insert(node);

  Node::save(storagePath_, node);
}
} // namespace IR::bplustree
#endif // !BPLUSTREE_FILE_BASED_NODE_MANAGER_HPP_
