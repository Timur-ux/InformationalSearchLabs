#ifndef MAP_HPP_
#define MAP_HPP_

#include <cassert>
#include <cstddef>
#include <format>
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
    TKey key;
    TVal value;

    NodeData() = default;
    NodeData(const TKey &k) : key(k) {}
    NodeData(const TKey &k, const TVal &val) : key(k), value(val) {}
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
    void destroyRecurse() {
      if (left)
        left->destroyRecurse(), delete left;
      if (right)
        right->destroyRecurse(), delete right;
    }

    ~Node() {}
  };

  size_t size_ = 0;
  Node *root_ = nullptr;

  TVal &findOrInsert(const TKey &key);
  const TVal &find(const TKey &key) const;
  void rebalanceInsert(Node *node);
  void rebalanceRemove(Node *node);

  void rotateLeft(Node *node);
  void rotateRight(Node *node);

  void balanceLeft(Node *node, bool isDelete = false);
  void balanceRight(Node *node, bool isDelete = false);

  void removeNode(Node *node);

  long validate(Node *node, int depth = 0);

public:
  Map() = default;
  Map(const Map &other) {
    if (other.size() > 0)
      root_ = new Node(*other.root_), size_ = other.size_;
  }
  Map(Map &&other) noexcept : root_(other.root_), size_(other.size_) {
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
      root_->destroyRecurse(), delete root_;
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
  void remove(const TKey &key);

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
      root_->destroyRecurse(), delete root_;
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

  rebalanceInsert(node);
  while (root_->parent != nullptr)
    root_ = root_->parent;

  return node->value;
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::insert(const TKey &key, const TVal &value) {
  findOrInsert(key) = value;
#ifdef DEBUG
  validate(root_);
#endif // !DEBUG
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
void Map<TKey, TVal>::rebalanceInsert(Node *node) {
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
    return rebalanceInsert(parent);
  case LL:
    return balanceLeft(parent);
  case RR:
    return balanceRight(parent);
  default:
    throw std::runtime_error("Node get invalid balance value");
  }
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::balanceLeft(Node *node, bool isDelete) {
  Node *left = node->left;
  if (left->balance == L || (isDelete && left->balance == Balanced)) {
    rotateRight(node);
    if (isDelete && left->balance == Balanced)
      node->balance = L, left->balance = R;
    else
      node->balance = Balanced, left->balance = Balanced;
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
void Map<TKey, TVal>::balanceRight(Node *node, bool isDelete) {
  Node *right = node->right;
  if (right->balance == R || (isDelete && right->balance == Balanced)) {
    rotateLeft(node);
    if (isDelete && right->balance == Balanced)
      node->balance = R, right->balance = L;
    else
      node->balance = Balanced, right->balance = Balanced;
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
  while (root_->parent != nullptr)
    root_ = root_->parent;
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
  while (root_->parent != nullptr)
    root_ = root_->parent;
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
  os << this->key << ' '
     << (balance == Balanced ? 'B'
         : balance == L      ? 'l'
         : balance == R      ? 'r'
         : balance == LL     ? 'L'
                             : 'R')
     << '\n';

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

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::remove(const TKey &key) {
  Node *node = root_;
  while (node != nullptr && node->key != key) {
    if (key < node->key)
      node = node->left;
    else
      node = node->right;
  }
  if (!node)
    return;

  --size_;
#ifdef DEBUG
  removeNode(node);
  try {
    validate(root_);
  } catch (...) {
    std::cerr << "Catched error while removing: " << key << '\n';
    print(std::cerr);
    throw;
  }
  return;
#else
  return removeNode(node);
#endif // !DEBUG
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::removeNode(Node *node) {
  Node *parent = node->parent;
  // case 1: no childs
  if (!node->left && !node->right) {
    if (node == root_) {
      delete node;
      return root_ = nullptr, void();
    }
    if (parent->left == node)
      parent->left = nullptr, parent->balance += 1;
    else
      parent->right = nullptr, parent->balance -= 1;
    delete node;
    return rebalanceRemove(parent);
  } else if (!node->left) { // case 2: only right
    Node *right = node->right;
    if (parent->left == node)
      parent->left = right, parent->balance += 1;
    else
      parent->right = right, parent->balance -= 1;
    right->parent = parent;
    delete node;
    return rebalanceRemove(parent);
  } else if (!node->right) { // case 3: only left
    Node *left = node->left;
    if (parent->left == node)
      parent->left = left, parent->balance += 1;
    else
      parent->right = left, parent->balance -= 1;
    left->parent = parent;
    delete node;
    return rebalanceRemove(parent);
  } else { // case 4: both childs
    Node *right = node->right;
    while (right->left)
      right = right->left;

    node->key = std::move(right->key);
    node->value = std::move(right->value);
    removeNode(right);
  }
}

template <Comparable TKey, Defaulted TVal>
void Map<TKey, TVal>::rebalanceRemove(Node *node) {
  if (!node)
    return;

  Node *parent = node->parent;
  if (node->balance == L || node->balance == R)
    return;

  bool isNodeLeftSided = (parent && parent->left == node);
  bool needRebalance = true;
  if (node->balance == LL)
    needRebalance = node->left && node->left->balance != Balanced,
    balanceLeft(node, true);
  else if (node->balance == RR)
    needRebalance = node->right && node->right->balance != Balanced,
    balanceRight(node, true);

  if (!parent || !needRebalance)
    return;
		
  if (isNodeLeftSided)
    parent->balance += 1;
  else
    parent->balance -= 1;

  return rebalanceRemove(parent);
}

template <Comparable TKey, Defaulted TVal>
long Map<TKey, TVal>::validate(Node *node, int depth) {
  if (!node)
    return 0;
  long leftH = validate(node->left, depth + 1),
       rightH = validate(node->right, depth + 1);
  if (std::abs(leftH - rightH) >= 2 || (rightH - leftH != node->balance))
    throw std::runtime_error(std::format(
        "Map tree invalid! Invalidation found on depth: {}, node key: {}",
        depth, node->key));
  return std::max(leftH, rightH) + 1;
}
} // namespace IR
#endif // !MAP_HPP_
