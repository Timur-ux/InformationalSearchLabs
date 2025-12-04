#ifndef TRIE_HPP_
#define TRIE_HPP_
#include "Map.hpp"
#include "Vector.hpp"
#include "concepts.hpp"
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace IR {
template <Comparable TChar, typename TVal> class Trie {
public:
  struct INode {
    virtual bool isTerm() const = 0;
    virtual bool canGoTo(const TChar &c) const = 0;
    virtual const INode *goTo(const TChar &c, const Trie<TChar, TVal> & trie) const = 0;
    virtual const std::optional<TVal> &value() const = 0;
    virtual ~INode() = default;
  };

private:
  class Node : public INode {
    Map<TChar, size_t> translations_;
    std::optional<TVal> value_ = std::nullopt;
    friend Trie<TChar, TVal>;
  public:
    Node() = default;

    Node(const TVal &value) : value_(value) {}
    Node(TVal &&value) noexcept : value_(std::move(value)) {}

		Node(const Node &other)
		: translations_(other.translations_), value_(other.value_) {}

		Node(Node &&other) noexcept
			: translations_(std::move(other.translations_)), value_(std::move(other.value_)) {}

		Node& operator=(const Node&other) {
			translations_ = other.translations_;
			value_ = other.value_;
			return *this;
		}

		Node& operator=(Node &&other) noexcept {
			if(this == &other) 
				return *this;
			translations_ = std::move(other.translations_);
			value_ = std::move(other.value_);

			return *this;
		}
		

    ~Node() = default;

    bool isTerm() const override final;
    bool canGoTo(const TChar &c) const override final;

    const INode *goTo(const TChar &c, const Trie<TChar, TVal> & trie) const override final;

    const std::optional<TVal> &value() const override final;
    std::optional<TVal> &value();
		std::basic_ostream<wchar_t> & print(std::basic_ostream<wchar_t> & os, const Trie<TChar, TVal> & trie, size_t depth) const {
			for(const auto& [key, child] : translations_) {
				for(size_t i = 0; i < depth; ++i) 
					os << L'\t';
				os << key << L'\n';
				trie.nodes_[child].print(os, trie, depth+1);
			}

			return os;
		}
  };

  Vector<Node> nodes_;
  size_t size_ = 0;

public:
  Trie() { nodes_.push_back(Node()); };

  ~Trie() = default;

  Trie &insert(const VectorView<TChar> &s, const TVal &value);
  Trie &insert(const TChar *s, size_t len, const TVal &value);

  std::optional<TVal> get(const TChar *s, size_t len);
  std::optional<TVal> get(const VectorView<TChar> &s);
  const std::optional<TVal> &get(const TChar *s, size_t len) const;
  const std::optional<TVal> &get(const VectorView<TChar> &s) const;

  TVal &getOrInsert(const TChar *s, size_t len, const TVal &value);
  TVal &getOrInsert(const VectorView<TChar> &s, const TVal &value);

  size_t size() const { return size_; }

	std::basic_ostream<wchar_t> & print(std::basic_ostream<wchar_t> & os) const {
		if(nodes_.size() > 0) 
			nodes_[0].print(os, *this, 0);
		return os;
	}
};

template <Comparable TChar, typename TVal>
bool Trie<TChar, TVal>::Node::isTerm() const {
  return value_.has_value();
}

template <Comparable TChar, typename TVal>
bool Trie<TChar, TVal>::Node::canGoTo(const TChar &c) const {
  return translations_.contains(c);
}


template <Comparable TChar, typename TVal>
const Trie<TChar, TVal>::INode *Trie<TChar, TVal>::Node::goTo(const TChar &c, const Trie<TChar, TVal> & trie) const {
  if (canGoTo(c))
    return &trie.nodes_[translations_[c]];
  throw std::runtime_error(
      "Can go to at given value via no translation exists");
}

template <Comparable TChar, typename TVal>
const std::optional<TVal> &Trie<TChar, TVal>::Node::value() const {
  return value_;
}

template <Comparable TChar, typename TVal>
std::optional<TVal> &Trie<TChar, TVal>::Node::value() {
  return value_;
}

template <Comparable TChar, typename TVal>
Trie<TChar, TVal> &Trie<TChar, TVal>::insert(const VectorView<TChar> &s,
                                             const TVal &value) {
  return insert(s.data(), s.size(), value);
}

template <Comparable TChar, typename TVal>
Trie<TChar, TVal> &Trie<TChar, TVal>::insert(const TChar *s, size_t len,
                                             const TVal &value) {
  size_t currentNode = 0;
  for (size_t i = 0; i < len; ++i) {
		if(!nodes_[currentNode].translations_.contains(s[i])) 
			nodes_.push_back(Node()), nodes_[currentNode].translations_[s[i]] = nodes_.size() - 1;
    currentNode = nodes_[currentNode].translations_[s[i]];
	}

  nodes_[currentNode].value_ = value;
  ++size_;

  return *this;
}

template <Comparable TChar, typename TVal>
std::optional<TVal> Trie<TChar, TVal>::get(const TChar *s, size_t len) {
  size_t currentNode = 0;
  for (size_t i = 0; i < len; ++i) {
    if (!nodes_[currentNode].canGoTo(s[i]))
      return std::nullopt;

    currentNode = nodes_[currentNode].translations_[s[i]];
  }
  return nodes_[currentNode].value_;
}

template <Comparable TChar, typename TVal>
std::optional<TVal> Trie<TChar, TVal>::get(const VectorView<TChar> &s) {
  return get(s.data(), s.size());
}

template <Comparable TChar, typename TVal>
const std::optional<TVal> &Trie<TChar, TVal>::get(const TChar *s,
                                                  size_t len) const {
  size_t currentNode = 0;
  for (size_t i = 0; i < len; ++i) {
    if (!nodes_[currentNode].canGoTo(s[i]))
      return std::nullopt;

    currentNode = nodes_[currentNode].translations_[s[i]];
  }
  return nodes_[currentNode].value_;
}

template <Comparable TChar, typename TVal>
const std::optional<TVal> &
Trie<TChar, TVal>::get(const VectorView<TChar> &s) const {
  return get(s.data(), s.size());
}

template <Comparable TChar, typename TVal>
TVal &Trie<TChar, TVal>::getOrInsert(const TChar *s, size_t len,
                                     const TVal &value) {
  size_t currentNode = 0;
  for (size_t i = 0; i < len; ++i) {
		if(!nodes_[currentNode].translations_.contains(s[i])) 
			nodes_.push_back(Node()), nodes_[currentNode].translations_[s[i]] = nodes_.size() - 1;
			
    currentNode = nodes_[currentNode].translations_[s[i]];
	}

  if (!nodes_[currentNode].value_)
    nodes_[currentNode].value_ = value, ++size_;
  return *nodes_[currentNode].value_;
}

template <Comparable TChar, typename TVal>
TVal &Trie<TChar, TVal>::getOrInsert(const VectorView<TChar> &s,
                                     const TVal &value) {
  return getOrInsert(s.data(), s.size(), value);
}

} // namespace IR
#endif // !TRIE_HPP_
