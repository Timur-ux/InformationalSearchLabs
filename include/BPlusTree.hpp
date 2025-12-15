#ifndef B_PLUS_TREE_HPP_
#define B_PLUS_TREE_HPP_
#include "Bimap.hpp"
#include "Vector.hpp"
#include "algo.hpp"
#include "concepts.hpp"
#include "event.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <stdexcept>
#include <utility>
#pragma once

namespace IR {
namespace bplustree {
enum class SameKeyOrdering { Random, Increase, AsInserted };
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class INodeManager;

namespace impl {
using random_device = std::random_device;
template <Comparable TKey, typename TVal, SameKeyOrdering T> struct KeyType {};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
using _key_type = typename KeyType<TKey, TVal, TOrdering>::type;

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
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
struct Node;

/**
 * @brief Key factories used for creating tree keys
 * provided SameKeyOrdering policy
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class KeyFactory {};
} // namespace impl

// Manage nodes (loading, saving and creating)
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class INodeManager {
public:
  virtual ~INodeManager() {}
  using Node = impl::Node<TKey, TVal, TOrdering>;
  /**
   * @brief Load and read parent id from node's data
   * Throws error if parent node not pointed (i.e. < 0)
   * Used when you don't know parentId. When loading parent of current node
   * for example
   *
   * @param id -- node to load id
   *
   * @return loaded node
   */
  virtual Node load(long id) = 0;
  /**
   * @brief load node and set it parent's id
   * Used when you know parent id of loading node. When loading child of
   * current node for example It is IMPORTANT to use this method when you know
   * parent's id because in other cases links to parents may be incorrect via
   * splits and merges after insertions and deletions
   *
   * @param id -- node to load id
   * @param currentNodeId -- id that will be set as parent id
   *
   * @return loaded node
   */
  virtual Node load(long id, long parentId) = 0;
  virtual void save(const Node &node) = 0;
  virtual Node create(bool isLeaf, long parentId, size_t level,
                      long nextNodeId = -1) = 0;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class FileBasedNodeManager;
/**
 * @brief File system based B+ tree
 *
 * @tparam NodeCapacity maximum amount of links in node
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class BPlusTree {
  using key_type = impl::_key_type<TKey, TVal, TOrdering>;
  using value_type = TVal;
  using Node = impl::Node<TKey, TVal, TOrdering>;
  using NodeManager_type = INodeManager<TKey, TVal, TOrdering>;
  using KeyFactory_type = impl::KeyFactory<TKey, TVal, TOrdering>;

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
  size_t level_ = 0, nodeCapacity_ = 1024;
  std::shared_ptr<NodeManager_type> nodeManager_;
  long rootId_ = -1;
  KeyFactory_type keyFactory_;

  // Dispatch rootId and size between handlers when destroying
  event::Event<long> onDestroy_;
  friend FileBasedNodeManager<TKey, TVal, TOrdering>;

public:
  event::IEvent<long> &onDestroy;
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

  BPlusTree(std::shared_ptr<NodeManager_type> nodeManager, long rootId = -1)
      : onDestroy(onDestroy_), nodeManager_(nodeManager), rootId_(rootId) {}

  BPlusTree(std::shared_ptr<NodeManager_type> nodeManager, long rootId = -1,
            size_t nodeCapacity = 1024)
      : onDestroy(onDestroy_), nodeManager_(nodeManager), rootId_(rootId),
        nodeCapacity_(nodeCapacity) {
    if ((nodeCapacity_ & 1) || nodeCapacity_ <= 2)
      throw std::invalid_argument("Only even node capacity higher 2 allowed");
  }

  ~BPlusTree() { onDestroy_.invoke(std::forward<long>(rootId_)); }

  template <SameAs<TKey> UKey> Cursor find(UKey &&key);

  /**
   * @brief Find values with key in [lower, higher] range
   *
   * @param lower -- lower border for search(inclusive)
   * @param higher -- higher border for search(inclusive)
   *
   * @return Cursor with founded values
   */
  template <SameAs<TKey> UKey> Cursor find(UKey &&lower, UKey &&higher);

  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  void insert(UKey &&key, UVal &&value);

  template <SameAs<TKey> UKey> void remove(UKey &&key);

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
};

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

namespace fs = std::filesystem;
/**
 * @brief Stores nodes on the files
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class FileBasedNodeManager : public INodeManager<TKey, TVal, TOrdering> {
  struct NodeSchema {
    long id;
    size_t level;
    bool isLeaf;
    size_t nKeys, nLinks, nValues;
    long nextNodeId, parentId;
  };

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

public:
  FileBasedNodeManager(const fs::path &storagePath)
      : storagePath_(storagePath),
        schemaUpdater_(std::make_shared<SchemaUpdater>(&schema_)) {}

  FileBasedNodeManager(const fs::path &storagePath, Schema loadedSchema)
      : FileBasedNodeManager(storagePath) {
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

// -----------------------------------------
// |  Out-line classes method definitions  |
// -----------------------------------------

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
template <SameAs<TKey> UKey, SameAs<TVal> UVal>
void BPlusTree<TKey, TVal, TOrdering>::insert(UKey &&key_, UVal &&value) {
  key_type key =
      keyFactory_(std::forward<UKey>(key_), std::forward<UVal>(value));
  Node node;
  if (rootId_ < 0)
    node = nodeManager_->create(true, -1l, 0, -1), rootId_ = node.id;
  else
    node = nodeManager_->load(rootId_);

  // while not in leaf
  while (node.level > 0) {
    assert(("Only nodes with 0 level may be marked as leaf node",
            node.level > 0 && !node.isLeaf));
    auto it = algo::lowerBound(std::begin(node.keys), std::end(node.keys), key);
    auto i = it - node.keys.begin();
    node = nodeManager_->load(node.links.at(i), node.id);
  }

  // do insertion
  assert(("Insertion allowed only in leaf node with 0 level",
          node.level == 0 && node.isLeaf));
  auto itKey =
      algo::lowerBound(std::begin(node.keys), std::end(node.keys), key);
  auto itVal = std::begin(node.values) + (itKey - std::begin(node.keys));
  node.keys.insert(itKey, key);
  node.values.insert(itVal, value);
	while(node.n() >= nodeCapacity_) {
		long parentId = split(node);
		node = nodeManager_->load(parentId);
	}
		

  nodeManager_->save(node);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
template <SameAs<TKey> UKey>
BPlusTree<TKey, TVal, TOrdering>::Cursor
BPlusTree<TKey, TVal, TOrdering>::find(UKey &&key) {
  return find(key, key);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
template <SameAs<TKey> UKey>
BPlusTree<TKey, TVal, TOrdering>::Cursor
BPlusTree<TKey, TVal, TOrdering>::find(UKey &&lower_, UKey &&higher_) {
  typename Cursor::values_type result;
  key_type lower = keyFactory_(std::forward<UKey>(lower_), TVal()),
           higher = keyFactory_(std::forward<UKey>(higher_), TVal());
  std::function<bool(const key_type &, const key_type &)> cmp =
      [](const key_type &k1, const key_type &k2) -> bool {
    return k1.first < k2.first;
  };
  auto cmpBorder = [](const key_type &k1, const key_type &k2) -> bool {
    return k1.first <= k2.first;
  };
  assert((cmp(lower, higher)));
  if (rootId_ < 0)
    return Cursor(result);

  Node node = nodeManager_->load(rootId_);
  while (node.level > 0) {
    assert(("Only leaf nodes may be placed at level 0", !node.isLeaf));

    auto it = algo::lowerBound(node.keys.begin(), node.keys.end(), lower, cmp);
    auto i = it - node.keys.begin();
    node = nodeManager_->load(node.links.at(i), node.id);
  }
  auto begin = node.keys.begin(), end = node.keys.end(),
       it = algo::lowerBound(begin, end, lower, cmp);
  while (cmpBorder(*it, higher)) {
    result.push_back(std::pair{it->first, node.values.at(it - begin)});
    ++it;
    if (it == end) {
      if (node.nextNodeId < 0)
        break;
      node = nodeManager_->load(node.nextNodeId);
      begin = node.keys.begin(), end = node.keys.end(), it = begin;
    }
  }

  return Cursor(result);
}

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

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
long BPlusTree<TKey, TVal, TOrdering>::split(Node &node) {
  assert(("For non root node parent Id must be set",
          node.id == rootId_ || node.id >= 0));
  Node parent;
  if (node.id == rootId_)
    parent = nodeManager_->create(false, -1, node.level + 1),
    parent.links.push_back(node.id), rootId_ = parent.id,
    node.parentId = parent.id;
  else
    parent = nodeManager_->load(node.parentId);

  Node right = nodeManager_->create(node.isLeaf, parent.id, node.level);
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
  nodeManager_->save(parent);
  nodeManager_->save(left);
  nodeManager_->save(right);

  return parentId;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void BPlusTree<TKey, TVal, TOrdering>::print(std::ostream &os) {
  if (rootId_ < 0)
    return;
  Node root = nodeManager_->load(rootId_);
  root.print(os, 0, *nodeManager_);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
template <SameAs<TKey> UKey>
void BPlusTree<TKey, TVal, TOrdering>::remove(UKey &&key) {
  throw std::logic_error("Removing not realized yet");
}

namespace impl {
template <Comparable TKey, typename TVal>
class KeyFactory<TKey, TVal, SameKeyOrdering::Random> {
  random_device device_;
  using key_type = _key_type<TKey, TVal, SameKeyOrdering::Random>;

public:
  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  key_type operator()(UKey &&key, UVal &&val) {
    return key_type{std::forward<TKey>(key), device_()};
  }
};

template <Comparable TKey, typename TVal>
class KeyFactory<TKey, TVal, SameKeyOrdering::Increase> {
  using key_type = _key_type<TKey, TVal, SameKeyOrdering::Increase>;

public:
  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  key_type operator()(UKey &&key, UVal &&val) {
    return key_type{std::forward<TKey>(key), std::forward<TVal>(val)};
  }
};

template <Comparable TKey, typename TVal>
class KeyFactory<TKey, TVal, SameKeyOrdering::AsInserted> {
  size_t nextValue_ = 0;
  using key_type = _key_type<TKey, TVal, SameKeyOrdering::AsInserted>;

public:
  KeyFactory(size_t nextValue = 0) : nextValue_(nextValue) {}
  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  key_type operator()(UKey &&key, UVal &&val) {
    return key_type{std::forward<TKey>(key), nextValue_++};
  }
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
struct Node {
  using INodeManager = ::IR::bplustree::INodeManager<TKey, TVal, TOrdering>;
  using key_type = _key_type<TKey, TVal, TOrdering>;
  using link_type = long;
  using value_type = TVal;
  long id;
  size_t level;

  bool isLeaf = false;
  Vector<key_type> keys;
  Vector<long> links;
  Vector<TVal> values;
  long nextNodeId, parentId;

  size_t n() const { return keys.size(); }
  void print(std::ostream &os, int depth, INodeManager &manager);
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void Node<TKey, TVal, TOrdering>::print(std::ostream &os, int depth,
                                        INodeManager &manager) {
  for (int _ = 0; _ < depth; ++_)
    os << " | ";
  os << "id = " << id << std::boolalpha << "; isLeaf = " << isLeaf
     << "; level = " << level << "; nextNodeId = " << nextNodeId
     << "; parent id = " << parentId << '\n';
  for (int _ = 0; _ < depth; ++_)
    os << " | ";
  os << "keys = ";
  for (const auto &key : keys)
    os << key.first << ' ';
  os << '\n';
  for (int _ = 0; _ < depth; ++_)
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
struct KeyType<TKey, TVal, SameKeyOrdering::Random> {
  using type = std::pair<TKey, random_device::result_type>;
};

template <Comparable TKey, Comparable TVal>
struct KeyType<TKey, TVal, SameKeyOrdering::Increase> {
  using type = std::pair<TKey, TVal>;
};

template <Comparable TKey, typename TVal>
struct KeyType<TKey, TVal, SameKeyOrdering::AsInserted> {
  using type = std::pair<TKey, size_t>;
};

} // namespace impl

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void FileBasedNodeManager<TKey, TVal, TOrdering>::bindTo(
    std::shared_ptr<BPlusTree<TKey, TVal, TOrdering>> tree) {
  tree->onDestroy += schemaUpdater_;
  tree->rootId_ = schema_.rootId;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::~FileBasedNodeManager() {
  FILE *file = fopen((storagePath_ / schemaName).c_str(), "wb");
  if (!file)
    std::cerr << "Can't open schema file\n";
  else if (fwrite(&schema_, sizeof(Schema), 1, file) != 1)
    std::cerr << "Can't write schema data to file\n";

	if(file) 
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
              .nextNodeId = nextNodeId,
              .parentId = parentId};
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
impl::Node<TKey, TVal, TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::load(long id, long parentId) {
  Node node = load(id);
  node.parentId = parentId;
	save(node);
  return node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
impl::Node<TKey, TVal, TOrdering>
FileBasedNodeManager<TKey, TVal, TOrdering>::load(long id) {
  fs::path nodePath = storagePath_ / algo::lltostring(id);
  if (!fs::exists(nodePath))
    throw std::invalid_argument("Node with requested id doesn't exist");
  NodeSchema nodeSchema;
  FILE *file = fopen(nodePath.c_str(), "rb");
  if (!file)
    throw std::runtime_error("Can't open file with node");

  if (fread(&nodeSchema, sizeof(NodeSchema), 1, file) != 1)
    throw std::runtime_error("Can't read node schema to file");

  void *buffer = malloc(std::max(
      nodeSchema.nKeys * sizeof(typename Node::key_type),
      std::max(nodeSchema.nLinks * sizeof(typename Node::link_type),
               nodeSchema.nValues * sizeof(typename Node::value_type))));
  Node node{.id = nodeSchema.id,
            .level = nodeSchema.level,
            .isLeaf = nodeSchema.isLeaf,
            .nextNodeId = nodeSchema.nextNodeId,
            .parentId = nodeSchema.parentId};

  if (nodeSchema.nKeys > 0) {
    if (fread(buffer, sizeof(typename Node::key_type), nodeSchema.nKeys,
              file) != nodeSchema.nKeys)
      throw std::runtime_error("Can't read node keys to file");
    else
      node.keys = Vector<typename Node::key_type>(
          reinterpret_cast<Node::key_type *>(buffer), nodeSchema.nKeys);
  }

  if (nodeSchema.nLinks > 0) {
    if (fread(buffer, sizeof(typename Node::link_type), nodeSchema.nLinks,
              file) != nodeSchema.nLinks)
      throw std::runtime_error("Can't read node links to file");
    else
      node.links = Vector<typename Node::link_type>(
          reinterpret_cast<Node::link_type *>(buffer), nodeSchema.nLinks);
  }

  if (nodeSchema.nValues > 0) {
    if (fread(buffer, sizeof(typename Node::value_type), nodeSchema.nValues,
              file) != nodeSchema.nValues)
      throw std::runtime_error("Can't read node values to file");
    else
      node.values = Vector<typename Node::value_type>(
          reinterpret_cast<Node::value_type *>(buffer), nodeSchema.nValues);
  }

  fclose(file);
  free(buffer);
	return node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void FileBasedNodeManager<TKey, TVal, TOrdering>::save(const Node &node) {
  fs::path nodePath = storagePath_ / algo::lltostring(node.id);
  NodeSchema nodeSchema{node.id,          node.level,        node.isLeaf,
                        node.keys.size(), node.links.size(), node.values.size(),
                        node.nextNodeId,  node.parentId};
  FILE *file = fopen(nodePath.c_str(), "wb");
  if (!file)
    throw std::runtime_error("Can't open file with node");

  if (fwrite(&nodeSchema, sizeof(NodeSchema), 1, file) != 1)
    throw std::runtime_error("Can't write node schema to file");

  if (nodeSchema.nKeys > 0 &&
      fwrite(node.keys.data(), sizeof(typename Node::key_type),
             nodeSchema.nKeys, file) != nodeSchema.nKeys)
    throw std::runtime_error("Can't write node keys to file");

  if (nodeSchema.nLinks > 0 &&
      fwrite(node.links.data(), sizeof(typename Node::link_type),
             nodeSchema.nLinks, file) != nodeSchema.nLinks)
    throw std::runtime_error("Can't write node links to file");

  if (nodeSchema.nValues > 0 &&
      fwrite(node.values.data(), sizeof(typename Node::value_type),
             nodeSchema.nValues, file) != nodeSchema.nValues)
    throw std::runtime_error("Can't write node values to file");

  fclose(file);
}
} // namespace bplustree
} // namespace IR
#endif // !B_PLUS_TREE_HPP_
