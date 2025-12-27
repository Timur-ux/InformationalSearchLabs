#ifndef BPLUSTREE_NODE_HPP_
#define BPLUSTREE_NODE_HPP_
#include "Vector.hpp"
#include "algo.hpp"
#include "bplustree/fwd.hpp"
#include "bplustree/key_type.hpp"
#include "concepts.hpp"
#include "metrics.hpp"
#include <filesystem>
#pragma once
namespace IR::bplustree::impl {
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
  static Node load(std::filesystem::path storage, long id);
  static void save(std::filesystem::path storage, const Node &node);
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

struct NodeSchema {
  long id;
  size_t level;
  bool isLeaf;
  size_t nKeys, nLinks, nValues;
  long nextNodeId, parentId;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
Node<TKey, TVal, TOrdering>
Node<TKey, TVal, TOrdering>::load(std::filesystem::path storage, long id) {
	incrementNodeLoadUsage();
  std::filesystem::path nodePath = storage / algo::lltostring(id);
  if (!std::filesystem::exists(nodePath))
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

  Vector<typename Node::key_type> keys;
  Vector<typename Node::link_type> links;
  Vector<typename Node::value_type> values;
  if (nodeSchema.nKeys > 0) {
    if (fread(buffer, sizeof(typename Node::key_type), nodeSchema.nKeys,
              file) != nodeSchema.nKeys)
      throw std::runtime_error("Can't read node keys to file");
    else
      keys = Vector<typename Node::key_type>(
          reinterpret_cast<Node::key_type *>(buffer), nodeSchema.nKeys);
  }

  if (nodeSchema.nLinks > 0) {
    if (fread(buffer, sizeof(typename Node::link_type), nodeSchema.nLinks,
              file) != nodeSchema.nLinks)
      throw std::runtime_error("Can't read node links to file");
    else
      links = Vector<typename Node::link_type>(
          reinterpret_cast<Node::link_type *>(buffer), nodeSchema.nLinks);
  }

  if (nodeSchema.nValues > 0) {
    if (fread(buffer, sizeof(typename Node::value_type), nodeSchema.nValues,
              file) != nodeSchema.nValues)
      throw std::runtime_error("Can't read node values to file");
    else
      values = Vector<typename Node::value_type>(
          reinterpret_cast<Node::value_type *>(buffer), nodeSchema.nValues);
  }

  Node node{.id = nodeSchema.id,
            .level = nodeSchema.level,
            .isLeaf = nodeSchema.isLeaf,
            .keys = keys,
            .links = links,
            .values = values,
            .nextNodeId = nodeSchema.nextNodeId,
            .parentId = nodeSchema.parentId};

  fclose(file);
  free(buffer);
  return node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void Node<TKey, TVal, TOrdering>::save(
    std::filesystem::path storage, const Node<TKey, TVal, TOrdering> &node) {
	incrementNodeSaveUsage();
  std::filesystem::path nodePath = storage / algo::lltostring(node.id);
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

} // namespace IR::bplustree::impl
#endif // !BPLUSTREE_NODE_HPP_
