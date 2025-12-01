#ifndef MAP_HPP_
#define MAP_HPP_

#include <cassert>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <stdexcept>
#pragma once
#include "concepts.hpp"
namespace IR {

// Balance = height Right - height Left subtree
enum Balance { LL = -2, L = -1, Balanced = 0, R = 1, RR = 2 };
Balance &operator+=(Balance &balance, int diff);
Balance &operator-=(Balance &balance, int diff);

/**
 * @brief AVL Tree based map class
 *
 * @tparam TKey key class that must support compare operations
 * @tparam TVal value class that stored at each node
 */
template <Comparable TKey, Defaulted TVal> class Map {

	struct NodeData {
		const TKey key;
		TVal value;

		NodeData() = default;
		NodeData(const TKey &k) : key(k) {}
	};

  struct Node : public NodeData {
    Balance balance = Balanced;

    Node *parent = nullptr, *left = nullptr, *right = nullptr;

    Node() = default;
    Node(const TKey &k, Node *p) : NodeData(k), parent(p) {}

		void print(std::ostream & os, size_t depth = 0) const;
  };

  size_t size_ = 0;
  Node *root_ = nullptr;

  TVal &findOrInsert(const TKey &key);
  void rebalance(Node *node);

  void rotateLeft(Node *node);
  void rotateRight(Node *node);

  void balanceLeft(Node *node);
  void balanceRight(Node *node);

public:
  Map() = default;

  size_t size() const { return size_; }
  void insert(const TKey &key, const TVal &value);

  TVal &operator[](const TKey &key);
  const TVal &operator[](const TKey &key) const;

	void print(std::ostream & os) const;
	bool contains(const TKey & key) const;

	template <typename TData>
	class MapIterator;

	using iterator = MapIterator<NodeData>;
	using const_iterator = MapIterator<const NodeData>;

	template <typename TData>
	class MapIterator {
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = TData;
			using pointer = TData *;
			using reference = TData &;
		protected:
			Node * ptr_;
		public:
			MapIterator(Node * ptr) : ptr_(ptr) {}

			reference operator*() { return *ptr_; }
			pointer operator->() {return ptr_;}
	};
	
	iterator begin() {
		if(!root_)
			return iterator(root_);
		Node * node = root_;
		while(node->left != nullptr) 
			node = node->left;

		return iterator(node);
	}
};

template <Comparable TKey, Defaulted TVal>
TVal &Map<TKey, TVal>::findOrInsert(const TKey &key) {
  if (!root_)
    return (root_ = new Node(key, nullptr)), size_ = 1, root_->value;

  Node *node = root_, *p = node;
  while (node != nullptr && node->key != key) {
    p = node;
    if (key < node->key)
      node = node->left;
    else
      node = node->right;
  }
  if (node != nullptr)
    return node->value;

  node = new Node(key, p);
  if (key < p->key)
    p->left = node;
  else
    p->right = node;
  ++size_;

  rebalance(node);
  while (root_->parent != nullptr)
    root_ = root_->parent;

  return node->value;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::insert(const TKey &key, const TVal &value) {
  findOrInsert(key) = value;
}

template <Comparable TKey, Defaulted TVal>
TVal &Map<TKey, TVal>::operator[](const TKey &key) {
  return findOrInsert(key);
}

template <Comparable TKey, Defaulted TVal>
const TVal &Map<TKey, TVal>::operator[](const TKey &key) const {
  return findOrInsert(key);
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::rebalance(Node *node) {
  if (!node || !node->parent)
    return;

  Node *parent = node->parent;
  if (parent->left == node)
    parent->balance -= 1;
  else
    parent->balance += 1;

  switch (parent->balance) {
  case Balanced:
    return;
  case L:
  case R:
    return rebalance(parent);
  case LL:
    return balanceLeft(parent);
  case RR:
    return balanceRight(parent);
  default:
    throw std::runtime_error("Node get invalid balance value");
  }
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::balanceLeft(Node *node) {
  Node *left = node->left;
  if (left->balance == L) {
    rotateRight(node);
    node->balance = Balanced;
    left->balance = Balanced;
    return;
  }
  // left->balance == R
  Node *leftRight = left->right;
  rotateLeft(left);
  rotateRight(leftRight);
  node->balance = Balanced;
  left->balance = Balanced;

  switch (leftRight->balance) {
  case L:
    node->balance = R;
    break;
  case R:
    left->balance = L;
    break;
  }

  leftRight->balance = Balanced;
  return;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::balanceRight(Node *node) {
  Node *right = node->right;
  if (right->balance == R) {
    rotateLeft(node);
    node->balance = Balanced;
    right->balance = Balanced;
    return;
  }
  // right->balance == L
  Node *rightLeft = right->left;
  rotateRight(right);
  rotateLeft(rightLeft);
  node->balance = Balanced;
  right->balance = Balanced;

  switch (rightLeft->balance) {
  case L:
    right->balance = R;
    break;
  case R:
    node->balance = L;
    break;
  }

  rightLeft->balance = Balanced;
  return;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::rotateLeft(Node *node) {
  Node *parent = node->parent, *right = node->right;

  if (right)
    right->parent = parent;

  if (parent) {
    if (parent->right == node)
      parent->right = right;
    else
      parent->left = right;
  }

  node->right = (right ? right->left : nullptr);
  if (node->right)
    node->right->parent = node;

  if (right)
    right->left = node;
  node->parent = right;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::rotateRight(Node *node) {
  Node *parent = node->parent, *left = node->left;

  if (left)
    left->parent = parent;

  if (parent) {
    if (parent->right == node)
      parent->right = left;
    else
      parent->left = left;
  }

  node->left = (left ? left->right : nullptr);
  if (node->left)
    node->left->parent = node;

  if (left)
    left->right = node;
  node->parent = left;
}

template <Comparable TKey, Defaulted TVal>
std::ostream & operator<<(std::ostream & os, const Map<TKey, TVal>& map) {
	map.print(os);

	return os;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::print(std::ostream & os) const {
	if(root_) 
		root_->print(os);
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::Node::print(std::ostream & os, size_t depth) const {
	if(right) 
		right->print(os, depth+1);
		
	size_t d = depth;
	while(d-- > 0) 
		os << '\t';
	os << this->key << '\n';

	if(left) 
		left->print(os, depth+1);
}

template <Comparable TKey, Defaulted TVal>
bool Map<TKey, TVal>::contains(const TKey & key) const {
	Node * node = root_;
	while(node != nullptr && node->key != key) {
		if(key < node->key) 
			node = node->left;
		else
			node = node->right;
	}

	return (node != nullptr);
}

} // namespace IR
#endif // !MAP_HPP_
