#ifndef B_PLUS_TREE_HPP_
#define B_PLUS_TREE_HPP_
#include "Bimap.hpp"
#include "Map.hpp"
#include "Vector.hpp"
#include "algo.hpp"
#include "concepts.hpp"
#include <cassert>
#include <iterator>
#include <stdexcept>
#include <utility>
#pragma once

namespace IR {

/**
 * @brief File system based B+ tree
 *
 * @tparam NodeCapacity maximum amount of links in node
 */
template <Comparable TKey, typename TVal> class BPlusTree {
  /**
   * @brief Each node have keys.
   * If it inner node(i.e. not leaf) it have links to childs
   * It it leaf node it have values
   *
   * For each inner node we have n keys and n+1 links
   *			 key[0]	 key[1] ...				key[n-1]
   * link[0]	link[1]	 ...		link[n-1]		link[n]
   *
   * For key[i] link[i] points to node with LESS OR EQUAL values, link[i+1]
   * points to node with HIGHER values
   */
  struct Node {
    long id;
    size_t level;

    bool isLeaf = false;
    Vector<TKey> keys;
    Vector<long> links;
    Vector<TVal> values;
    long nextNodeId, parentId;

    size_t n() const { return keys.size(); }
  };

  // Manage nodes (loading, saving)
  class NodeManager {
    Vector<Node> storage_; // TODO: file based store
    Bimap<long, size_t> index_;
    long nextFreeId_ = 0;

  public:
    /**
     * @brief Load and read parent id from node's data
     * Throws error if parent node not pointed (i.e. < 0)
     *
     * @param id -- node to load id
     *
     * @return loaded node
     */
    Node load(long id);

    /**
     * @brief load node and set it parent's id
     *
     * @param id -- node to load id
     * @param currentNodeId -- id that will be set as parent id
     *
     * @return
     */
    Node load(long id, long parentId);
    void save(const Node &node);
    Node create(bool isLeaf, long parentId, size_t level, long nextNodeId = -1);
  };

  // Tree level -- 0 is leaf and root have maximum level
  size_t level_ = 0, nodeCapacity_ = 8192, size_ = 0;
  NodeManager nodeManager_;
  long rootId_ = -1;

  /**
   * @brief Split node by 2 nodes by the middle element. First element from
   * second half passes to parent node
   *
   * @param node -- node to be splitted
   *
   * @return parent's node id
   */
  long split(Node &node);

public:
  class Cursor {
    using values_type = Vector<std::pair<TKey, TVal>>;
    values_type vals_;

  public:
    Cursor(const values_type &values) : vals_(values) {}
    Cursor(values_type &&values) : vals_(std::move(values)) {}

    using iterator = values_type::iterator;
    using const_iterator = values_type::const_iterator;

    iterator begin() { return vals_.begin(); }
    iterator end() { return vals_.end(); }

    const_iterator begin() const { return vals_.begin(); }
    const_iterator end() const { return vals_.end(); }
  };

  BPlusTree() {
    if ((nodeCapacity_ & 1) || nodeCapacity_ <= 2)
      throw std::invalid_argument("Only even node capacity higher 2 allowed");
  }

  Cursor find(const TKey &key);

  /**
   * @brief Find values with key in [lower, higher] range
   *
   * @param lower -- lower border for search(inclusive)
   * @param higher -- higher border for search(inclusive)
   *
   * @return Cursor with founded values
   */
  Cursor find(const TKey &lower, const TKey &higher);

  void insert(const TKey &key, const TVal &value);
  void remove(const TKey &key);

private:
};

template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::insert(const TKey &key, const TVal &value) {
  Node node;
  if (rootId_ < 0)
    node = nodeManager_.create(true, -1), rootId_ = node.id;
  else
    node = nodeManager_.load(rootId_, -1);

  // while not in leaf
  while (node.level > 0 || node.n() == nodeCapacity_) {
    if (node.n() == nodeCapacity_) {
      auto parentId = split(node);
      node = nodeManager_.load(parentId);
      continue;
    }

    assert(("Only nodes with 0 level may be marked as leaf node",
            node.level > 0 && !node.isLeaf));
    auto it =
        algo::lowerBound(std::begin(node.keys), std::end(node.keys), value);
    auto i = it - node.keys.begin();
    node = nodeManager_.load(node.links.at(i));
  }

  // do insertion
  assert(("Insertion allowed only in leaf node with 0 level",
          node.level == 0 && node.isLeaf));
  auto it = algo::lowerBound(std::begin(node.keys), std::end(node.keys), value);
  node.keys.push_back(TKey());
  node.values.spush_back(TVal());
  auto i = it - std::begin(node.keys);
  for (auto itKey = it + 1; itKey != std::end(node.keys); ++itKey)
    *itKey = *(itKey - 1);
  for (auto itVal = std::begin(node.values) + (i + 1);
       itVal != std::end(node.values); ++itVal)
    *itVal = *(itVal - 1);

  node.keys[i] = key;
  node.values[i] = value;
  nodeManager_.save(node);
  ++size_;
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Cursor BPlusTree<TKey, TVal>::find(const TKey &key) {
  return find(key, key);
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Cursor BPlusTree<TKey, TVal>::find(const TKey &lower,
                                                          const TKey &higher) {
  assert((lower <= higher));
  if (rootId_ < 0)
    return Cursor(Cursor::values_type());

  Node node = nodeManager_.load(rootId_);
  while (node.level > 0) {
    assert(("Only leaf nodes may be placed at level 0", !node.isLeaf));

    auto it = algo::lowerBound(node.keys.begin(), node.keys.end(), lower);
    auto i = it - node.keys.begin();
    node = nodeManager_.load(node.links[i]);
  }
  typename Cursor::values_type result;
  auto begin = node.keys.begin(), end = node.keys.end(),
       it = algo::lowerBound(begin, end, lower);
  while (*it <= higher) {
    result.push_back({*it, node.values.at(it - begin)});
    ++it;
    if (it == end) {
      if (node.nextNodeId < 0)
        break;
      node = nodeManager_.load(node.nextNodeId);
      begin = node.keys.begin(), end = node.keys.end(), it = begin;
    }
  }

  return Cursor(result);
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Node
BPlusTree<TKey, TVal>::NodeManager::load(long nodeId) {
  if (!index_.contains(nodeId))
    throw std::invalid_argument("Node with given id doesn't exist");

  size_t index = index_[nodeId];
  return storage_[index];
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Node
BPlusTree<TKey, TVal>::NodeManager::load(long nodeId, long parentId) {
  if (!index_.contains(nodeId))
    throw std::invalid_argument("Node with given id doesn't exist");

  size_t index = index_[nodeId];
  Node node = storage_[index];
  node.parentId = parentId;
  return node;
}
template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::NodeManager::save(const Node &node) {
  if (!index_.contains(node.id))
    throw std::invalid_argument("Node with given id doesn't exist");

  storage_[index_[node.id]] = node;
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Node
BPlusTree<TKey, TVal>::NodeManager::create(bool isLeaf, long parentId, size_t level, long nextNodeId) {
	assert(("Leaves may be placed only at 0 level", (isLeaf && level == 0) || (!isLeaf && level != 0)));
	storage_.push_back(Node{nextFreeId_++, level, isLeaf, {}, {}, {}, nextNodeId, parentId});
	return storage_[storage_.size() - 1];
}
} // namespace IR
#endif // !B_PLUS_TREE_HPP_
