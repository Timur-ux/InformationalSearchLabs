#ifndef B_PLUS_TREE_FACTORY_HPP_
#define B_PLUS_TREE_FACTORY_HPP_
#include "BPlusTree.hpp"
#pragma once
namespace IR::bplustree {
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class BPlusTreeFactory {
  using tree_type = BPlusTree<TKey, TVal, TOrdering>;

public:
  static tree_type createInMemory(size_t nodeCapacity = 1024);
  static tree_type createFileBased(const std::string &storageName,
                                   size_t nodeCapacity = 8192);
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
BPlusTree<TKey, TVal, TOrdering>
BPlusTreeFactory<TKey, TVal, TOrdering>::createInMemory(size_t nodeCapacity) {
  return BPlusTree<TKey, TVal, TOrdering>(
      std::make_unique<InMemoryNodeManager<TKey, TVal, TOrdering>>(), 0,
      nodeCapacity);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
BPlusTree<TKey, TVal, TOrdering>
BPlusTreeFactory<TKey, TVal, TOrdering>::createFileBased(const std::string &storageName,
                                   size_t nodeCapacity) {
	
}

} // namespace IR::bplustree
#endif // !B_PLUS_TREE_FACTORY_HPP_
