#ifndef B_PLUS_TREE_HPP_
#define B_PLUS_TREE_HPP_
#include "Bimap.hpp"
#include "Map.hpp"
#include "Queue.hpp"
#include "Vector.hpp"
#include "algo.hpp"
#include "concepts.hpp"
#include <cassert>
#include <ios>
#include <iterator>
#include <random>
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
  using key_type =
      std::pair<TKey, std::random_device::result_type>; // second is random
                                                        // number used for
                                                        // creating unique keys
  using value_type = TVal;
  class NodeManager;
  /**
   * @brief Each node have keys.
   * If it inner node(i.e. not leaf) it have links to childs
   * It it leaf node it have values
   *
   * For each inner node we have n keys and n+1 links
   *
   *			 key[0]	 key[1] ...				key[n-1]
   *
   * link[0]	link[1]	 ...		link[n-1]		link[n]
   *
   * For key[i] link[i] points to node with LESS OR EQUAL values, link[i+1]
   * points to node with HIGHER values
   */
  struct Node {
    long id;
    size_t level;

    bool isLeaf = false;
    Vector<key_type> keys;
    Vector<long> links;
    Vector<value_type> values;
    long nextNodeId, parentId;

    size_t n() const { return keys.size(); }
    void print(std::ostream &os, int depth, NodeManager &manager);
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
		 * Used when you don't know parentId. When loading parent of current node for example
     *
     * @param id -- node to load id
     *
     * @return loaded node
     */
    Node load(long id);

    /**
     * @brief load node and set it parent's id
		 * Used when you know parent id of loading node. When loading child of current node for example
		 * It is IMPORTANT to use this method when you know parent's id because in other cases links to
		 * parents may be incorrect via splits and merges when inserting and deleting
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

  /**
   * @brief Split node by 2 nodes by the middle element. First element from
   * second half passes to parent node
   *
   * @param node -- node to be splitted
   *
   * @return parent's node id
   */
  long split(Node &node);

  // Tree level -- 0 is leaf and root have maximum level
  size_t level_ = 0, nodeCapacity_ = 8192, size_ = 0;
  NodeManager nodeManager_;
  long rootId_ = -1;
  std::random_device randomGenerator_;

public:
  class Cursor {
		friend BPlusTree;
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

  BPlusTree(size_t nodeCapacity) : nodeCapacity_(nodeCapacity) {
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

  /**
   * @brief For each node print
   *
   * nodeId; isLeaf; level; nextNodeId; parentId;
   * key[0] ... key[n-1]
   * (link[0]/value[0]) ... (link[n]/value[n-1])
   *
   * with intendation equal node's depth
   */
  void print(std::ostream &os);

  size_t size() const { return size_; }
};

template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::insert(const TKey &key_, const TVal &value) {
  key_type key = {key_, randomGenerator_()};
  Node node;
  if (rootId_ < 0)
    node = nodeManager_.create(true, -1l, 0, -1), rootId_ = node.id;
  else
    node = nodeManager_.load(rootId_);

  // while not in leaf
  while (node.level > 0 || node.n() == nodeCapacity_) {
    if (node.n() == nodeCapacity_) {
      auto parentId = split(node);
      node = nodeManager_.load(parentId);
      continue;
    }

    assert(("Only nodes with 0 level may be marked as leaf node",
            node.level > 0 && !node.isLeaf));
    auto it = algo::lowerBound(std::begin(node.keys), std::end(node.keys), key);
    auto i = it - node.keys.begin();
    node = nodeManager_.load(node.links.at(i), node.id);
  }

  // do insertion
  assert(("Insertion allowed only in leaf node with 0 level",
          node.level == 0 && node.isLeaf));
  auto itKey =
      algo::lowerBound(std::begin(node.keys), std::end(node.keys), key);
  auto itVal = std::begin(node.values) + (itKey - std::begin(node.keys));
  node.keys.insert(itKey, key);
  node.values.insert(itVal, value);

  nodeManager_.save(node);
  ++size_;
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Cursor BPlusTree<TKey, TVal>::find(const TKey &key) {
  return find(key, key);
}

template <Comparable TKey, typename TVal>
BPlusTree<TKey, TVal>::Cursor BPlusTree<TKey, TVal>::find(const TKey &lower_,
                                                          const TKey &higher_) {
  typename Cursor::values_type result;
  key_type lower = {lower_, randomGenerator_()},
           higher = {higher_, randomGenerator_()};
	std::function<bool(const key_type &, const key_type&)> cmp = [](const key_type &k1, const key_type &k2) -> bool {
    return k1.first <= k2.first;
  };
  assert((cmp(lower, higher)));
  if (rootId_ < 0)
    return Cursor(result);

  Node node = nodeManager_.load(rootId_);
  while (node.level > 0) {
    assert(("Only leaf nodes may be placed at level 0", !node.isLeaf));

    auto it = algo::lowerBound(node.keys.begin(), node.keys.end(), lower, cmp);
    auto i = it - node.keys.begin();
    node = nodeManager_.load(node.links.at(i), node.id);
  }
  auto begin = node.keys.begin(), end = node.keys.end(),
       it = algo::lowerBound(begin, end, lower, cmp);
  while (cmp(*it, higher)) {
    result.push_back(std::pair{it->first, node.values.at(it - begin)});
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
  assert(("node id must be set(i.e. must be non negative)", nodeId >= 0));
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
  if (!index_.contains(parentId))
    throw std::invalid_argument("Node with given parent id doesn't exist");

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
BPlusTree<TKey, TVal>::NodeManager::create(bool isLeaf, long parentId,
                                           size_t level, long nextNodeId) {
  assert(("Leaves may be placed only at 0 level",
          (isLeaf && level == 0) || (!isLeaf && level != 0)));
  Node node{nextFreeId_++, level, isLeaf, {}, {}, {}, nextNodeId, parentId};
  index_.insert(node.id, storage_.size());
  storage_.push_back(node);
  return storage_[storage_.size() - 1];
}

template <Comparable TKey, typename TVal>
long BPlusTree<TKey, TVal>::split(Node &node) {
  assert(("For non root node parent Id must be set",
          node.id == rootId_ || node.id >= 0));
  Node parent;
  if (node.id == rootId_)
    parent = nodeManager_.create(false, -1, node.level + 1),
    parent.links.push_back(node.id), rootId_ = parent.id,
    node.parentId = parent.id;
  else
    parent = nodeManager_.load(node.parentId);

  Node right = nodeManager_.create(node.isLeaf, parent.id, node.level);
  Node &left = node;
  assert(("Only nodes with level 0 may be marked as leaves",
          !left.isLeaf && left.level > 0 || left.isLeaf && left.level == 0));
  assert(("At same level must be only leaves or only non leaves",
          left.level == right.level && left.isLeaf == right.isLeaf));
  assert(("Parent level must be higher by 1 than left and right childs level",
          parent.level == left.level + 1));

  size_t midInd = left.n() / 2;
  key_type key = left.keys.at(midInd);
  // Insert key and new link to parent node
  auto itKey = algo::lowerBound(parent.keys.begin(), parent.keys.end(), key);
  auto itLink =
      std::begin(parent.links) + (itKey - std::begin(parent.keys)) + 1;
  itKey = parent.keys.insert(itKey, key);
  parent.links.insert(itLink, right.id);

  // fill keys and (links/values) between left and right nodes
  for (size_t currentInd = midInd + 1; currentInd < left.keys.size();
       ++currentInd)
    right.keys.push_back(std::move(left.keys.at(currentInd)));
  size_t nKeys = left.keys.size();
  if (!left.isLeaf) // if left inner node we additionaly remove key that was
                    // moved up
    ++nKeys;
  for (size_t currentInd = midInd + 1; currentInd < nKeys; ++currentInd)
    left.keys.pop_back();

  if (left.isLeaf) {
    // move values to right node
    for (size_t currentInd = midInd + 1; currentInd < left.values.size();
         ++currentInd)
      right.values.push_back(std::move(left.values.at(currentInd)));
    size_t nVals = left.values.size();
    for (size_t currentInd = midInd + 1; currentInd < nVals; ++currentInd)
      left.values.pop_back();
  } else {
    // move links to right node
    for (size_t currentInd = midInd + 1; currentInd < left.links.size();
         ++currentInd)
      right.links.push_back(std::move(left.links.at(currentInd)));
    size_t nLinks = left.links.size();
    for (size_t currentInd = midInd + 1; currentInd < nLinks; ++currentInd)
      left.links.pop_back();
  }

  // update link to next node
  right.nextNodeId = left.nextNodeId;
  left.nextNodeId = right.id;

  long parentId = parent.id;
  // save chanded nodes
  nodeManager_.save(parent);
  nodeManager_.save(left);
  nodeManager_.save(right);

  return parentId;
}

template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::Node::print(std::ostream &os, int depth,
                                        NodeManager &manager) {
  for (size_t _ = 0; _ < depth; ++_)
    os << " | ";
  os << "id = " << id << std::boolalpha << "; isLeaf = " << isLeaf
     << "; level = " << level << "; nextNodeId = " << nextNodeId << "; parent id = " << parentId << '\n';
  for (size_t _ = 0; _ < depth; ++_)
    os << " | ";
  os << "keys = ";
  for (const auto &key : keys)
    os << key.first << ' ';
  os << '\n';
  for (size_t _ = 0; _ < depth; ++_)
    os << " | ";
  if (isLeaf) {
    os << "values = ";
    for (const auto &value : values)
      os << value << ' ';
  } else {
    os << "links = ";
    for (auto link : links)
      os << link << ' ';
  }
  os << '\n';

  for (auto link : links) {
    Node child = manager.load(link, id);
    child.print(os, depth + 1, manager);
  }
}

template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::print(std::ostream &os) {
  if (rootId_ < 0)
    return;
  Node root = nodeManager_.load(rootId_);
  root.print(os, 0, nodeManager_);
}

template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::remove(const TKey &key) {
  throw std::logic_error("Removing not realized yet");
}
} // namespace IR
#endif // !B_PLUS_TREE_HPP_
