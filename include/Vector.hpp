#ifndef VECTOR_HPP_
#define VECTOR_HPP_
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <vector>
#pragma once

namespace IR {
template <typename T> class Vector {
private:
  T *data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 32;

public:
  Vector();
  Vector(size_t n);
  Vector(size_t n, const T &value);

  Vector(const Vector &other);
  Vector(Vector &&other) noexcept;
  Vector &operator=(const Vector &other);
  Vector &operator=(Vector &&other) noexcept;

  ~Vector();
};

template <typename T> Vector<T>::Vector() { data_ = new T[capacity_]; }

template <typename T> Vector<T>::Vector(size_t n) : size_(n), capacity_(2 * n) {
  data_ = (T*)malloc(capacity_*sizeof(T));
  for (size_t i = 0; i < size_; ++i)
    data_[i] = T();
}

template <typename T>
Vector<T>::Vector(size_t n, const T &value) : size_(n), capacity_(2 * n) {
  data_ = (T*)malloc(capacity_*sizeof(T));
  for (size_t i = 0; i < size_; ++i)
    data_[i] = value;
}

template <typename T>
Vector<T>::Vector(const Vector<T> &other)
    : size_(other.size_), capacity_(other.capacity_) {
  data_ = (T*)malloc(capacity_*sizeof(T));
	memcpy(data_, other.data_, sizeof(T)*size_);
}

template <typename T>
 Vector<T>::Vector(Vector<T> && other) noexcept
	: size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
	other.data_ = nullptr;
	other.size_ = 0;
	other.capacity_ = 0;
}

template <typename T>
Vector<T> & Vector<T>::operator=(const Vector<T> &other) {
	if(other.capacity_ > capacity_) 
		data_ = (T*)realloc(data_, other.capacity_*sizeof(T));

	capacity_	 = other.capacity_;
	size_ = other.size_;
	memcpy(data_, other.data_, sizeof(T)*size_);

	return *this;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T> &&other) noexcept {
	if(this == &other) 
		return *this;

	if(data_) 
		free(data_);
	data_ = other.data_;
	size_ = other.size_;
	capacity_ = other.capacity_;

	other.data_ = nullptr;
	other.size_ = 0;
	other.capacity_ = 0;

	return *this;
}

template <typename T>
Vector<T>::~Vector<T>() {
	if(data_) 
		free(data_);
}


} // namespace IR
#endif // !VECTOR_HPP_
