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
		NodeData(const TKey &k, const TVal & val) : key(k), value(val) {}
  };

  struct Node : public NodeData {
    Balance balance = Balanced;

    Node *parent = nullptr, *left = nullptr, *right = nullptr;

    Node() = default;
    Node(const Node &other)
        : NodeData(other.key, other.value), balance(other.balance) {
      if (other.left)
        left = new Node(*other.left), left->parent = this;
      if (other.right)
        right = new Node(*other.right), right->parent = this;
    }

    Node(Node &&other) noexcept
        : NodeData(std::move(other.key), std::move(other.value)),
          balance(std::move(other.balance)) {
      if (other.left)
        left = other.left, other.left = nullptr, left->parent = this;
      if (other.right)
        right = other.right, other.right = nullptr, right->parent = this;
    }

    Node &operator=(const Node &other) {
      this->key = other.key;
      this->value = other.value;
      balance = other.balance;
      parent = other.parent;

      if (other.left)
        left = new Node(*other.left), left->parent = this;
      if (other.right)
        right = new Node(*other.right), right->parent = this;

      return *this;
    }

    Node &operator=(Node &&other) noexcept {
      if (this == &other)
        return *this;

      this->key = std::move(other.key);
      this->value = std::move(other.value);
      balance = std::move(other.balance);
      parent = std::move(other.parent);

      if (other.left)
        left = other.left, left->parent = this;
      if (other.right)
        right = other.right, right->parent = this;

      return *this;
    }

    Node(const TKey &k, Node *p) : NodeData(k), parent(p) {}

    void print(std::ostream &os, size_t depth = 0) const;

    ~Node() {
      if (left)
        delete left;
      if (right)
        delete right;
    }
  };

  size_t size_ = 0;
  Node *root_ = nullptr;

  TVal &findOrInsert(const TKey &key);
  const TVal &find(const TKey &key) const;
  void rebalance(Node *node);

  void rotateLeft(Node *node);
  void rotateRight(Node *node);

  void balanceLeft(Node *node);
  void balanceRight(Node *node);

public:
  Map() = default;
  Map(const Map &other) {
    if (other.size() > 0)
      root_ = new Node(*other.root_), size_ = other.size_;
  }
  Map(Map &&other) noexcept
      : root_(other.root_), size_(other.size_) {
    other.root_ = nullptr;
    other.size_ = 0;
  }
  Map &operator=(const Map &other) {
    if (other.size() > 0)
      root_ = new Node(*other.root_), size_ = other.size_;
    else if (root_)
      delete root_, root_ = nullptr, size_ = 0;
    return *this;
  }

  Map &operator=(Map &&other) noexcept {
    if (this == &other)
      return *this;
    if (root_)
      delete root_;
    root_ = other.root_;
    size_ = other.size_;

    other.root_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  size_t size() const { return size_; }
  void insert(const TKey &key, const TVal &value);

  TVal &operator[](const TKey &key);
  const TVal &operator[](const TKey &key) const;

  void print(std::ostream &os) const;
  bool contains(const TKey &key) const;

  template <typename TData> class MapIterator;

  using iterator = MapIterator<NodeData>;
  using const_iterator = MapIterator<const NodeData>;

  template <typename TData> class MapIterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = TData;
    using pointer = TData *;
    using reference = TData &;

  protected:
    Node *ptr_;

  public:
    MapIterator(Node *ptr) : ptr_(ptr) {}

    reference operator*() { return *ptr_; }
    pointer operator->() { return ptr_; }

    bool operator==(const MapIterator<TData> &other) const {
      return ptr_ == other.ptr_;
    }

    // prefix
    MapIterator<TData> &operator++() {
      if (!ptr_)
        return *this;

      if (ptr_->right) {
        ptr_ = ptr_->right;
        while (ptr_->left)
          ptr_ = ptr_->left;
        return *this;
      }

      Node *parent = ptr_->parent;
      while (parent != nullptr && parent->key < ptr_->key)
        parent = parent->parent;

      ptr_ = parent;
      return *this;
    }

    // postfix
    MapIterator<TData> operator++(int) {
      MapIterator<TData> result(ptr_);
      ++(*this);
      return result;
    }

    MapIterator<TData> &operator--() {
      if (!ptr_)
        return *this;

      if (ptr_->left) {
        ptr_ = ptr_->left;
        while (ptr_->right)
          ptr_ = ptr_->right;
        return *this;
      }

      Node *parent = ptr_->parent;
      while (parent != nullptr && ptr_->key < parent->key)
        parent = parent->parent;

      ptr_ = ptr_->parent;
      return *this;
    }

    MapIterator<TData> operator--(int) {
      MapIterator<TData> result(ptr_);
      --(*this);
      return result;
    }
  };

  iterator begin() {
    if (!root_)
      return iterator(root_);
    Node *node = root_;
    while (node->left != nullptr)
      node = node->left;

    return iterator(node);
  }
  iterator end() { return iterator(nullptr); }

  const_iterator begin() const {
    if (!root_)
      return const_iterator(root_);
    Node *node = root_;
    while (node->left != nullptr)
      node = node->left;

    return const_iterator(node);
  }

  const_iterator end() const { return const_iterator(nullptr); }

  ~Map() {
    if (root_)
      delete root_;
  }
};

template <Comparable TKey, Defaulted TVal>
const TVal &Map<TKey, TVal>::find(const TKey &key) const {
  if (!root_)
    throw std::invalid_argument("Node with given key not found");

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
  throw std::invalid_argument("Node with given key not found");
}

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
  return find(key);
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
  rotateRight(node);
  node->balance = Balanced;
  left->balance = Balanced;

  switch (leftRight->balance) {
  case L:
    node->balance = R;
    break;
  case R:
    left->balance = L;
    break;
	default:
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
  rotateLeft(node);
  node->balance = Balanced;
  right->balance = Balanced;

  switch (rightLeft->balance) {
  case L:
    right->balance = R;
    break;
  case R:
    node->balance = L;
    break;
	default:
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
std::ostream &operator<<(std::ostream &os, const Map<TKey, TVal> &map) {
  map.print(os);

  return os;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::print(std::ostream &os) const {
  if (root_)
    root_->print(os);
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::Node::print(std::ostream &os, size_t depth) const {
  if (right)
    right->print(os, depth + 1);

  size_t d = depth;
  while (d-- > 0)
    os << '\t';
  os << this->key << '\n';

  if (left)
    left->print(os, depth + 1);
}

template <Comparable TKey, Defaulted TVal>
bool Map<TKey, TVal>::contains(const TKey &key) const {
  Node *node = root_;
  while (node != nullptr && node->key != key) {
    if (key < node->key)
      node = node->left;
    else
      node = node->right;
  }

  return (node != nullptr);
}

} // namespace IR
#endif // !MAP_HPP_
