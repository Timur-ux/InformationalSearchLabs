#ifndef BIMAP_HPP_
#define BIMAP_HPP_
#include "Map.hpp"
#include "concepts.hpp"
#include <type_traits>
#pragma once

namespace IR {
template <Comparable T1, Comparable T2>
  requires(!std::is_same_v<T1, T2>)
class Bimap {
  Map<T1, T2> forward_;
  Map<T2, T1> backward_;

public:
  enum class Direction { Forward, Backward };

  Bimap() = default;
  Bimap(const Bimap &other)
      : forward_(other.forward_), backward_(other.backward_) {}
  Bimap(Bimap &&other) noexcept
      : forward_(std::move(other.forward_)),
        backward_(std::move(other.backward_)) {}
  Bimap &operator=(const Bimap &other) {
    forward_ = other.forward_;
    backward_ = other.backward_;

    return *this;
  }

  Bimap &operator=(Bimap &&other) noexcept {
    if (this == &other)
      return *this;

    forward_ = std::move(other.forward_);
    backward_ = std::move(other.backward_);
    return *this;
  }

  size_t size() const { return forward_.size(); }

  Bimap &insert(const T1 &v1, const T2 &v2) {
    forward_[v1] = v2;
    backward_[v2] = v1;

    return *this;
  }

  const T2 &operator[](const T1 &key) const { return forward_[key]; }
  const T1 &operator[](const T2 &key) const { return backward_[key]; }

  bool contains(const T1 &key) const { return forward_.contains(key); }
  bool contains(const T2 &key) const { return backward_.contains(key); }
};
} // namespace IR
#endif // !BIMAP_HPP_
