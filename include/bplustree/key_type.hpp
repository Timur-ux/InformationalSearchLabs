#ifndef BPLUSTREE_KEY_TYPE_HPP_
#define BPLUSTREE_KEY_TYPE_HPP_
#include "bplustree/fwd.hpp"
#include "concepts.hpp"
#pragma once
namespace IR::bplustree::impl {

template <Comparable TKey, typename TVal>
struct KeyType<TKey, TVal, SameKeyOrdering::Random> {
  using type = std::pair<TKey, random_device::result_type>;
};

template <Comparable TKey, Comparable TVal>
struct KeyType<TKey, TVal, SameKeyOrdering::Increase> {
  using type = std::pair<TKey, TVal>;
};

template <Comparable TKey, typename TVal>
struct KeyType<TKey, TVal, SameKeyOrdering::AsInserted> {
  using type = std::pair<TKey, size_t>;
};

template <Comparable TKey, typename TVal>
class KeyFactory<TKey, TVal, SameKeyOrdering::Random> {
  random_device device_;
  using key_type = _key_type<TKey, TVal, SameKeyOrdering::Random>;

public:
  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  key_type operator()(UKey &&key, UVal &&val) {
    return key_type{std::forward<UKey>(key), device_()};
  }
};

template <Comparable TKey, typename TVal>
class KeyFactory<TKey, TVal, SameKeyOrdering::Increase> {
  using key_type = _key_type<TKey, TVal, SameKeyOrdering::Increase>;

public:
  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  key_type operator()(UKey &&key, UVal &&val) {
    return key_type{std::forward<UKey>(key), std::forward<UVal>(val)};
  }
};

template <Comparable TKey, typename TVal>
class KeyFactory<TKey, TVal, SameKeyOrdering::AsInserted> {
  size_t nextValue_ = 0;
  using key_type = _key_type<TKey, TVal, SameKeyOrdering::AsInserted>;

public:
  KeyFactory(size_t nextValue = 0) : nextValue_(nextValue) {}
  template <SameAs<TKey> UKey, SameAs<TVal> UVal>
  key_type operator()(UKey &&key, UVal &&val) {
    return key_type{std::forward<UKey>(key), nextValue_++};
  }
};
} // namespace IR::bplustree::imlp
#endif // !BPLUSTREE_KEY_TYPE_HPP_
