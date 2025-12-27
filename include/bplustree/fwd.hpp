#ifndef BPLUSTREE_FWD_HPP_
#define BPLUSTREE_FWD_HPP_
#include "concepts.hpp"
#include <random>
#pragma once
namespace IR::bplustree {

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
template <Comparable TKey, typename TVal, SameKeyOrdering T> struct Node;

/**
 * @brief Key factories used for creating tree keys
 * provided SameKeyOrdering policy
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class KeyFactory {};
} // namespace impl

// Manage nodes (loading, saving and creating)
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class INodeManager;

/**
 * @brief Stores nodes on the files
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class FileBasedNodeManager;

/**
 * @brief File system based B+ tree
 *
 * @tparam NodeCapacity maximum amount of links in node
 */
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class BPlusTree;
} // namespace IR::bplustree
#endif // !BPLUSTREE_FWD_HPP_
