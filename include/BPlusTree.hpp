#ifndef B_PLUS_TREE_HPP_
#define B_PLUS_TREE_HPP_
#include "Map.hpp"
#include "Vector.hpp"
#include "concepts.hpp"
#pragma once

namespace IR {

	/**
	 * @brief File system based B+ tree
	 *
	 * @tparam NodeCapacity maximum amount of links in node
	 */
template <Comparable TKey, typename TVal>
class BPlusTree {
	struct Node {
		long id;
		size_t level;

		bool isLeaf = false;
		Vector<TKey> keys;
		Vector<long> links;
		Vector<TVal> values;
		long nextNodeId, parentId;
	};

	// Manage nodes (loading, saving)
	class NodeManager {
		Vector<Node> storage_; // TODO: file based store
		Map<long, size_t> index_;
		long nextFreeId_ = 0;
	public:
		Node load(long id);
		void save(const Node & node);
		Node create(bool isLeaf, long parentId);
	};

	// Tree level -- 0 is leaf and root have maximum level
	size_t level_ = 0, nodeCapacity_ = 8192, size_ = 0;
	NodeManager nodeManager_;
	long rootId_ = -1;

	// split node by key and stores copies by nodeManager
	void split(Node & node, const TKey& key);
public:
	struct Cursor {
		TKey lower, higher;
		Node activeNode;
		NodeManager & nodeMaganger;
	};

	Cursor find(const TKey& key);
	Cursor find(const TKey& lower, const TKey & higher);

	void insert(const TKey & key, const TVal & value);
	void remove(const TKey & key);
};

template <Comparable TKey, typename TVal>
void BPlusTree<TKey, TVal>::insert(const TKey & key, const TVal & value) {
	Node node;
	if(rootId_ < 0) 
		node = nodeManager_.create(true, -1), rootId_ = node.id;
	else
		node = nodeManager_.load(rootId_);
}
} // namespace IR
#endif // !B_PLUS_TREE_HPP_
