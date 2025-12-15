#ifndef TRIE_HPP_
#define TRIE_HPP_
#include "Map.hpp"
#include "Vector.hpp"
#include "concepts.hpp"
#include <cstdio>
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
    virtual const INode *goTo(const TChar &c,
                              const Trie<TChar, TVal> &trie) const = 0;
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
        : translations_(std::move(other.translations_)),
          value_(std::move(other.value_)) {}

    Node &operator=(const Node &other) {
      translations_ = other.translations_;
      value_ = other.value_;
      return *this;
    }

    Node &operator=(Node &&other) noexcept {
      if (this == &other)
        return *this;
      translations_ = std::move(other.translations_);
      value_ = std::move(other.value_);

      return *this;
    }

    ~Node() = default;

    bool isTerm() const override final;
    bool canGoTo(const TChar &c) const override final;

    const INode *goTo(const TChar &c,
                      const Trie<TChar, TVal> &trie) const override final;

    const std::optional<TVal> &value() const override final;
    std::optional<TVal> &value();
    std::basic_ostream<wchar_t> &print(std::basic_ostream<wchar_t> &os,
                                       const Trie<TChar, TVal> &trie,
                                       size_t depth) const {
      for (const auto &[key, child] : translations_) {
        for (size_t i = 0; i < depth; ++i)
          os << L'\t';
        os << key << L'\n';
        trie.nodes_[child].print(os, trie, depth + 1);
      }

      return os;
    }
  };

  Vector<Node> nodes_;
  size_t size_ = 0;

  void serializeNode(size_t i, FILE *file);
  size_t deserializeNode(FILE *file);

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

  void serialize(const char *pathToFile);
  void deserialize(const char *pathToFile);

  std::basic_ostream<wchar_t> &print(std::basic_ostream<wchar_t> &os) const {
		os << L"*\n";
    if (nodes_.size() > 0)
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
const Trie<TChar, TVal>::INode *
Trie<TChar, TVal>::Node::goTo(const TChar &c,
                              const Trie<TChar, TVal> &trie) const {
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
    if (!nodes_[currentNode].translations_.contains(s[i]))
      nodes_.push_back(Node()),
          nodes_[currentNode].translations_[s[i]] = nodes_.size() - 1;
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
    if (!nodes_[currentNode].translations_.contains(s[i]))
      nodes_.push_back(Node()),
          nodes_[currentNode].translations_[s[i]] = nodes_.size() - 1;

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

template <Comparable TChar, typename TVal>
void Trie<TChar, TVal>::serialize(const char *pathToFile) {
  FILE *file = fopen(pathToFile, "wb");
  if (!file)
    throw std::runtime_error("Can't open file with trie's data");
  if (fwrite(&size_, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error("Can't write trie's size to file");

  serializeNode(0, file);
  fclose(file);
}

template <Comparable TChar, typename TVal>
void Trie<TChar, TVal>::deserialize(const char *pathToFile) {
  FILE *file = fopen(pathToFile, "rb");
  if (!file)
    throw std::runtime_error("Can't open file with trie's data");
  if (fread(&size_, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error("Can't read trie's size from file");

  nodes_.clear();
  nodes_.reserve(size_);
  deserializeNode(file);

  fclose(file);
}

template <Comparable TChar, typename TVal>
void Trie<TChar, TVal>::serializeNode(size_t i, FILE *file) {
  Node &node = nodes_[i];
  bool isTerm = node.isTerm();
  size_t nTranslations = node.translations_.size();
  if (fwrite(&isTerm, sizeof(bool), 1, file) != 1)
    throw std::runtime_error("Can't write trie node's isTerm flag to file");

  if (isTerm) {
		TVal value = node.value_.value();
		if(fwrite(&value, sizeof(TVal), 1, file) != 1)
    throw std::runtime_error("Can't write trie node's value to file");
	}

  if (fwrite(&nTranslations, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error(
        "Can't write trie node's tranlations count to file");

  for (auto [letter, iTranslation] : node.translations_) {
    if (fwrite(&letter, sizeof(TChar), 1, file) != 1)
      throw std::runtime_error(
          "Can't write trie node's translation letter to file");
    serializeNode(iTranslation, file);
  }
}

template <Comparable TChar, typename TVal>
size_t Trie<TChar, TVal>::deserializeNode(FILE *file) {
  nodes_.push_back(Node());
  size_t result = nodes_.size() - 1;

  bool isTerm;
  TVal value;
  size_t nTranslations;
  if (fread(&isTerm, sizeof(bool), 1, file) != 1)
    throw std::runtime_error("Can't read trie node's isTerm flag from file");

  if (isTerm) {
    if (fread(&value, sizeof(TVal), 1, file) != 1)
      throw std::runtime_error("Can't read trie node's value from file");
    nodes_[result].value_ = value;
  }

  if (fread(&nTranslations, sizeof(size_t), 1, file) != 1)
    throw std::runtime_error(
        "Can't read trie node's tranlations count from file");

  TChar letter;
  for (size_t i = 0; i < nTranslations; ++i) {
    if (fread(&letter, sizeof(TChar), 1, file) != 1)
      throw std::runtime_error(
          "Can't read trie node's translation letter from file");
    nodes_[result].translations_[letter] = deserializeNode(file);
  }

	return result;
}
} // namespace IR
#endif // !TRIE_HPP_
