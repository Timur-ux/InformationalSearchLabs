#ifndef VECTOR_HPP_
#define VECTOR_HPP_
#include "concepts.hpp"
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#pragma once

namespace IR {
template <typename T> class Vector {
protected:
  T *data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 1;

  void realloc(size_t newCapacity);

public:
  Vector();
  Vector(size_t n);
  Vector(size_t n, const T &value);
  Vector(T *data, size_t len);

  Vector(const Vector &other);
  Vector(Vector &&other) noexcept;
  Vector &operator=(const Vector &other);
  Vector &operator=(Vector &&other) noexcept;

  ~Vector();

  size_t size() const;
  size_t capacity() const;
  const T *data() const;
  T *data();

  Vector<T> &reserve(size_t n);

	template <SameAs<T> U>
  Vector<T> &push_back(U &&value);
  Vector<T> &pop_back();

  T &operator[](size_t i);
  const T &operator[](size_t i) const;

  T &at(size_t i);
  const T &at(size_t i) const;

  template <typename TData> class VectorIterator;

  using iterator = VectorIterator<T>;
  using const_iterator = VectorIterator<const T>;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  /**
   * @brief Insert value before given position
   *
   * @param it iterator before insertion will be
   * @param value value to insert
   *
   * @return iterator on newly inserted value
   */
  iterator insert(iterator it, const T &value);

  /**
   * @brief Insert value before given index
   *
   * @param index index before insertion will be
   * @param value value to insert
   *
   * @return iterator on newly inserted value
   */
  iterator insert(size_t index, const T &value);

  template <typename TData> class VectorIterator {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = TData;
    using pointer = TData *;
    using reference = TData &;

  protected:
    TData *ptr_;

  public:
    VectorIterator(TData *ptr) : ptr_(ptr) {}

    reference operator*() { return *ptr_; }
    pointer operator->() { return ptr_; }

    bool operator==(const VectorIterator<TData> &other) const {
      return ptr_ == other.ptr_;
    }

    // prefix
    VectorIterator<TData> &operator++() {
      ++ptr_;
      return *this;
    }

    // postfix
    VectorIterator<TData> operator++(int) {
      VectorIterator<TData> result(ptr_);
      ++(*this);
      return result;
    }

    VectorIterator<TData> &operator--() {
      --ptr_;
      return *this;
    }

    VectorIterator<TData> operator--(int) {
      VectorIterator<TData> result(ptr_);
      --(*this);
      return result;
    }

    VectorIterator<TData> operator+(difference_type diff) const {
      return VectorIterator<TData>(ptr_ + diff);
    }

    VectorIterator<TData> &operator+=(difference_type diff) {
      ptr_ += diff;
      return *this;
    }

    VectorIterator<TData> operator-(difference_type diff) const {
      return VectorIterator<TData>(ptr_ - diff);
    }

    difference_type operator-(const VectorIterator<TData> &other) const {
      return ptr_ - other.ptr_;
    }

    VectorIterator<TData> &operator-=(difference_type diff) {
      ptr_ -= diff;
      return *this;
    }
  };
};

// ------------------
// |  CONSTRUCTORS  |
// ------------------

template <typename T> Vector<T>::Vector() { data_ = new T[capacity_]; }

template <typename T> Vector<T>::Vector(size_t n) : size_(n), capacity_(2 * n) {
  data_ = new T[capacity_];
}

template <typename T>
Vector<T>::Vector(size_t n, const T &value) : size_(n), capacity_(2 * n) {
  data_ = new T[capacity_];
  for (size_t i = 0; i < size_; ++i)
    data_[i] = value;
}

template <typename T>
Vector<T>::Vector(const Vector<T> &other)
    : size_(other.size_), capacity_(other.capacity_) {
  data_ = new T[capacity_];
  for (size_t i = 0; i < size_; ++i)
    data_[i] = other[i];
}

template <typename T>
Vector<T>::Vector(Vector<T> &&other) noexcept
    : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;
}

template <typename T> Vector<T> &Vector<T>::operator=(const Vector<T> &other) {
  if (other.capacity_ > capacity_) {
    T *old = data_;
    data_ = new T[other.capacity_];
    delete[] old;
  }

  capacity_ = other.capacity_;
  size_ = other.size_;
  for (size_t i = 0; i < size_; ++i)
    data_[i] = other[i];

  return *this;
}

template <typename T>
Vector<T> &Vector<T>::operator=(Vector<T> &&other) noexcept {
  if (this == &other)
    return *this;

  if (data_)
    delete[] data_;
  data_ = other.data_;
  size_ = other.size_;
  capacity_ = other.capacity_;

  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;

  return *this;
}

template <typename T>
Vector<T>::Vector(T *data, size_t len) : size_(len), capacity_(len) {
  data_ = new T[capacity_];
  for (size_t i = 0; i < len; ++i)
    data_[i] = data[i];
}

// -----------------
// |  DESTRUCTORS  |
// -----------------

template <typename T> Vector<T>::~Vector() {
  if (data_)
    delete[] data_;
}

template <typename T> Vector<T>::iterator Vector<T>::begin() {
  return iterator(data_);
}

template <typename T> Vector<T>::iterator Vector<T>::end() {
  return iterator(data_ + size_);
}

template <typename T> Vector<T>::const_iterator Vector<T>::begin() const {
  return iterator(data_);
}

template <typename T> Vector<T>::const_iterator Vector<T>::end() const {
  return iterator(data_ + size_);
}

template <typename T>
void Vector<T>::realloc(size_t newCapacity) {
	if(newCapacity <= capacity_) 
		return;

	
	capacity_ = newCapacity;
	T* old = data_;
	data_ = new T[capacity_];
	for(size_t i = 0; i < size_; ++i) 
		data_[i] = std::move(old[i]);
	delete [] old;
}



template <typename T>
template <SameAs<T> U>
Vector<T> &Vector<T>::push_back(U &&value) {
  data_[size_++] = std::forward<U>(value);
  if (size_ >= capacity_)
    realloc(capacity_ * 2);
  return *this;
}

template <typename T> Vector<T> &Vector<T>::pop_back() {
  if (size_ > 0)
    --size_;
	return *this;
}

template <typename T> size_t Vector<T>::size() const { return size_; }

template <typename T> size_t Vector<T>::capacity() const { return capacity_; }

template <typename T> T &Vector<T>::operator[](size_t i) { return data_[i]; }

template <typename T> const T &Vector<T>::operator[](size_t i) const {
  return data_[i];
}

template <typename T> T &Vector<T>::at(size_t i) {
  if (i >= size_)
    throw std::out_of_range("Index out of range(i >= vector::size)");

  return data_[i];
}

template <typename T> const T &Vector<T>::at(size_t i) const {
  if (i >= size_)
    throw std::out_of_range("Index out of range(i >= vector::size)");

  return data_[i];
}

template <typename T> const T *Vector<T>::data() const { return data_; }

template <typename T> T *Vector<T>::data() { return data_; }

template <typename T>
Vector<T>::iterator Vector<T>::insert(iterator it, const T &value) {
	++size_;
	auto current = end() - 1;
	while(current != it)
		*current = *(current - 1), --current;

	*current = value;
	size_t i = current - begin();
  if (size_ >= capacity_)
    realloc(capacity_ * 2);

	return begin() + i;
}

template <typename T> class VectorView {
  const T *data_;
  size_t size_;

public:
  using const_iterator = Vector<T>::const_iterator;

  VectorView() = default;
  VectorView(const VectorView &other);
  VectorView(VectorView &&other) noexcept;
  VectorView &operator=(const VectorView &other);
  VectorView &operator=(VectorView &&other) noexcept;

  VectorView(const Vector<T> &vector);
  VectorView(const T *data, size_t len);
  ~VectorView() {}

  const T &operator[](size_t i) const;
  const T &at(size_t i) const;

  const_iterator begin() const;
  const_iterator end() const;

  size_t size() const;
  const T *data() const;
};

template <typename T>
VectorView<T>::VectorView(const VectorView<T> &other)
    : data_(other.data_), size_(other.size_) {}

template <typename T>
VectorView<T>::VectorView(VectorView<T> &&other) noexcept
    : data_(other.data_), size_(other.size_) {}

template <typename T>
VectorView<T> &VectorView<T>::operator=(const VectorView<T> &other) {
  data_ = other.data_;
  size_ = other.size_;
  return *this;
}

template <typename T>
VectorView<T> &VectorView<T>::operator=(VectorView<T> &&other) noexcept {
  data_ = other.data_;
  size_ = other.size_;
  return *this;
}

template <typename T>
VectorView<T>::VectorView(const Vector<T> &vector)
    : data_(vector.data()), size_(vector.size_) {}

template <typename T>
VectorView<T>::VectorView(const T *data, size_t len)
    : data_(data), size_(len) {}

template <typename T> const T &VectorView<T>::operator[](size_t i) const {
  return data_[i];
}

template <typename T> const T &VectorView<T>::at(size_t i) const {
  if (i >= size_)
    throw std::out_of_range("Index out of range(i >= vector::size)");

  return data_[i];
}
template <typename T>
VectorView<T>::const_iterator VectorView<T>::begin() const {
  return const_iterator(data_);
}

template <typename T> VectorView<T>::const_iterator VectorView<T>::end() const {
  return const_iterator(data_ + size_);
}

template <typename T> size_t VectorView<T>::size() const { return size_; }

template <typename T> const T *VectorView<T>::data() const { return data_; }

template <typename T> Vector<T> &Vector<T>::reserve(size_t n) {
  if (n < capacity_)
    return *this;

  capacity_ = n;
  T *old = data_;
  data_ = new T[capacity_];
  for (size_t i = 0; i < size_; ++i)
    data_[i] = std::move(old[i]);

  delete[] old;
  return *this;
}

} // namespace IR
#endif // !VECTOR_HPP_
