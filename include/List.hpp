#ifndef LIST_HPP_
#define LIST_HPP_
#include "concepts.hpp"
#include <utility>
#pragma once
namespace IR {
template <typename T> class List {
  struct Node;
  struct TermNode {
    Node *next, *prev;
  };
  struct Node : public TermNode {
    T value;
  };

  Node *end_;
  size_t size_ = 0;
  void destroyAllNodes(bool destroyTerminator = true);

public:
  List();
  List(const List &other);
  List(List &&other) noexcept;
  List &operator=(const List &other);
  List &operator=(List &&other) noexcept;

	~List();

  template <SameAs<T> U> List &push_back(U &&value);
  template <typename... Args> List &emplace_back(Args &&...args);

  List &pop_back();
  List &pop_front();

  template <typename TVal> class ListIterator;

  using iterator = ListIterator<T>;
  using const_iterator = ListIterator<const T>;

  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  template <SameAs<T> U> iterator insert(iterator it, U &&other);
  template <typename... Args> iterator emplace(iterator it, Args &&...args);

  iterator remove(iterator it);

  size_t size() const;
  bool empty() const;

  template <typename TVal> class ListIterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = TVal;
    using pointer = TVal *;
    using reference = TVal &;

  protected:
		friend List<T>;
    Node *ptr_;

  public:
    ListIterator(Node *ptr) : ptr_(ptr) {}

    reference operator*() { return ptr_->value; }
    pointer operator->() { return &ptr_->value; }

    bool operator==(const ListIterator<TVal> &other) const {
      return ptr_ == other.ptr_;
    }

    // prefix
    ListIterator<TVal> &operator++() {
      ptr_ = ptr_->next;
      return *this;
    }

    // postfix
    ListIterator<TVal> operator++(int) {
      ListIterator<TVal> result(ptr_);
      ++(*this);
      return result;
    }

    ListIterator<TVal> &operator--() {
      ptr_ = ptr_->prev;
      return *this;
    }

    ListIterator<TVal> operator--(int) {
      ListIterator<TVal> result(ptr_);
      --(*this);
      return result;
    }

    ListIterator<TVal> operator+(difference_type diff) const {
      ListIterator<TVal> res(ptr_);
      for (difference_type i = 0; i < diff; ++i)
        ++res;
      return res;
    }

    ListIterator<TVal> operator-(difference_type diff) const {
      ListIterator<TVal> res(ptr_);
      for (difference_type i = 0; i < diff; ++i)
        --res;
      return res;
    }

    difference_type operator-(const ListIterator<TVal> &other) const {
      difference_type result = 0;
      ListIterator<TVal> temp(ptr_);
      while (temp != other)
        ++result, --temp;
      return result;
    }
  };
};

template <typename T> List<T>::List() {
  end_ = reinterpret_cast<Node*>(malloc(sizeof(TermNode)));
  end_->next = (Node *)(end_->prev = (Node *)end_);
}

template <typename T> List<T>::List(const List &other) : List() {
  for (const auto &el : other)
    push_back<decltype(el)>(el);
}

template <typename T>
List<T>::List(List &&other) noexcept : end_(other.end_), size_(other.size_) {
  other.end_ = nullptr;
  other.size_ = 0;
}

template <typename T> List<T> &List<T>::operator=(const List<T> &other) {
  if (end_)
    destroyAllNodes(false);

  for (const auto &el : other)
    push_back(*el);

  return *this;
}

template <typename T> List<T> &List<T>::operator=(List &&other) noexcept {
  if (this == &other)
    return *this;
  if (end_)
    destroyAllNodes();

  end_ = other.end_;
  size_ = other.size_;

  other.end_ = nullptr;
  other.size_ = 0;

  return *this;
}

template <typename T>
template <SameAs<T> U>
List<T> &List<T>::push_back(U &&value) {
  insert(end(), std::forward<U>(value));
  return *this;
}

template <typename T>
template <typename... Args>
List<T> &List<T>::emplace_back(Args &&...args) {
  emplace(end(), std::forward<Args>(args)...);
  return *this;
}

template <typename T> List<T> &List<T>::pop_back() {
  remove(--end());
  return *this;
}
template <typename T> List<T> &List<T>::pop_front() {
  remove(begin());
  return *this;
}

template <typename T> List<T>::iterator List<T>::begin() {
  return iterator(end_->next);
}

template <typename T> List<T>::const_iterator List<T>::begin() const {
  return const_iterator(end_->next);
}

template <typename T> List<T>::iterator List<T>::end() {
  return iterator(end_);

}

template <typename T> List<T>::const_iterator List<T>::end() const {
  return const_iterator(end_);
}

template <typename T>
template <SameAs<T> U>
List<T>::iterator List<T>::insert(iterator it, U &&other) {
  iterator prev = it - 1;
  Node *node = reinterpret_cast<Node*>(malloc(sizeof(Node)));
  new (&node->value) T(std::forward<U>(other));

	node->next = it.ptr_;
	node->prev = prev.ptr_;

	it.ptr_->prev = node;
	prev.ptr_->next = node;

	++size_;
	return iterator(node);
}

template <typename T>
template <typename... Args>
List<T>::iterator List<T>::emplace(iterator it, Args &&...args) {
	static_assert(std::is_constructible_v<T, Args...>);
  iterator prev = it - 1;
  Node *node = reinterpret_cast<Node*>(malloc(sizeof(Node)));
  new (&node->value) T(std::forward<Args>(args)...);

	node->next = it.ptr_;
	node->prev = prev.ptr_;

	it.ptr_->prev = node;
	prev.ptr_->next = node;

	++size_;
	return iterator(node);
}

template <typename T> List<T>::iterator List<T>::remove(iterator it) {
	if(it == end() || empty()) 
		return end();

	--size_;
	Node *next = it.ptr_->next, *prev = it.ptr_->prev;
	next->prev = prev;
	prev->next = next;

	it->~T();
	free(it.ptr_);

	return iterator(next);
}

template <typename T> size_t List<T>::size() const { return size_; }
template <typename T> bool List<T>::empty() const { return size_ == 0; }

template <typename T> void List<T>::destroyAllNodes(bool destroyTerminator) {
  if (!end_)
    return;
  while (!empty())
    pop_front();

  if (destroyTerminator)
    free(end_), end_ = nullptr, size_ = 0;
}

template <typename T> List<T>::~List() {
	destroyAllNodes(true);
}

} // namespace IR
#endif // !LIST_HPP_
