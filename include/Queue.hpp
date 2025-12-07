#ifndef QUEUE_HPP_
#define QUEUE_HPP_
#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#pragma once

namespace IR {
template <typename T> class Queue {
  struct Node {
    T value_;
    Node *next_ = nullptr;
  };

  Node *first_ = nullptr;
  Node *last_ = nullptr;
  size_t size_ = 0;

  void copyReverse(const Node *node) {
    if (!node)
      return;
    copyReverse(node->next_);
    push(node->value_);
  }

public:
  Queue() = default;
  Queue(const Queue &other) { copyReverse(other.first_); }
  Queue(Queue &&other) noexcept
      : first_(std::move(other.first_)), last_(std::move(other.last_)),
        size_(other.size_) {
    other.first_ = nullptr;
    other.last_ = nullptr;
    other.size_ = 0;
  }
  Queue &operator=(const Queue &other) {
    while (!empty())
      pop();

    copyReverse(other.first_);
    return *this;
  }

  Queue &operator=(Queue &&other) noexcept {
    if (this == &other)
      return *this;

    while (!empty())
      pop();

		first_ = other.first_;
		last_ = other.last_;
		size_ = other.size_;

    other.first_ = nullptr;
    other.last_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  T &front();
  const T &front() const;

  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; };

  void push(const T &value);
  void push(T &&value);

  void pop();
};

template <typename T> void Queue<T>::pop() {
  if (empty())
    return;
  --size_;
  Node *node = first_;
  first_ = first_->next_;
  delete node;

	if(empty()) 
		first_ = (last_ = nullptr);
}

template <typename T> void Queue<T>::push(T &&value) {
  Node *node = new Node{.value_ = std::move(value), .next_ = nullptr};
  if (empty())
    return first_ = (last_ = node), ++size_, void();

  last_ = (last_->next_ = node);
  ++size_;
}

template <typename T> void Queue<T>::push(const T &value) {
  Node *node = new Node{.value_ = value, .next_ = nullptr};
  if (empty())
    return first_ = (last_ = node), ++size_, void();

  last_ = (last_->next_ = node);
  ++size_;
}

template <typename T> T &Queue<T>::front() {
  if (empty())
    throw std::runtime_error("No front element. Queue is empty");

  return first_->value_;
}

template <typename T> const T &Queue<T>::front() const {
  if (empty())
    throw std::runtime_error("No front element. Queue is empty");

  return first_->value_;
}

} // namespace IR
#endif // !QUEUE_HPP_
