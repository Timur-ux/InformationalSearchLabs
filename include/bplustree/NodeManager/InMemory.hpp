#ifndef BPLUSTREE_IN_MEMORY_NODE_MANAGER_HPP_
#define BPLUSTREE_IN_MEMORY_NODE_MANAGER_HPP_
#include "Bimap.hpp"
#include "bplustree/BPlusTree.hpp"
#include "concepts.hpp"
#pragma once
namespace IR::bplustree {

/**
 * @brief Stores nodes in RAM memory
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class InMemoryNodeManager : public INodeManager<TKey, TVal, TOrdering> {
  using Node = INodeManager<TKey, TVal, TOrdering>::Node;
  Vector<Node> storage_;
  Bimap<long, size_t> index_;
  long nextFreeId_ = 0;

public:
  Node load(long id) override;

  Node load(long id, long parentId) override;
  void save(const Node &node) override;
  Node create(bool isLeaf, long parentId, size_t level,
              long nextNodeId = -1) override;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
InMemoryNodeManager<TKey, TVal, TOrdering>::Node
InMemoryNodeManager<TKey, TVal, TOrdering>::load(long nodeId) {
  assert(("node id must be set(i.e. must be non negative)", nodeId >= 0));
  if (!index_.contains(nodeId))
    throw std::invalid_argument("Node with given id doesn't exist");

  size_t index = index_[nodeId];
  return storage_[index];
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
InMemoryNodeManager<TKey, TVal, TOrdering>::Node
InMemoryNodeManager<TKey, TVal, TOrdering>::load(long nodeId, long parentId) {
  if (!index_.contains(nodeId))
    throw std::invalid_argument("Node with given id doesn't exist");
  if (!index_.contains(parentId))
    throw std::invalid_argument("Node with given parent id doesn't exist");

  size_t index = index_[nodeId];
  Node node = storage_[index];
  node.parentId = parentId;
  save(node);
  return node;
}
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void InMemoryNodeManager<TKey, TVal, TOrdering>::save(const Node &node) {
  if (!index_.contains(node.id))
    throw std::invalid_argument("Node with given id doesn't exist");

  storage_[index_[node.id]] = node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
InMemoryNodeManager<TKey, TVal, TOrdering>::Node
InMemoryNodeManager<TKey, TVal, TOrdering>::create(bool isLeaf, long parentId,
                                                   size_t level,
                                                   long nextNodeId) {
  assert(("Leaves may be placed only at 0 level",
          (isLeaf && level == 0) || (!isLeaf && level != 0)));
  Node node{nextFreeId_++, level, isLeaf, {}, {}, {}, nextNodeId, parentId};
  index_.insert(node.id, storage_.size());
  storage_.push_back(node);
  return storage_[storage_.size() - 1];
}
} // namespace IR::bplustree
#endif // !BPLUSTREE_IN_MEMORY_NODE_MANAGER_HPP_
