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
template <Comparable TKey, typename TVal, size_t NodeCapacity = 8192>
class BPlusTree {
	
	struct Node {
		long id;
		size_t level;

		bool isLeaf = false;
		Vector<TVal> values;

		Vector<TKey> keys;
		Vector<long> links;
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
	size_t level_ = 0;
	NodeManager nodeManager_;
	long rootId_ = -1;

	// split node with id nodeId and stores copies by nodeManager
	static void split(long nodeId, const TKey& key, NodeManager & nodeManager);
};
} // namespace IR
#endif // !B_PLUS_TREE_HPP_
