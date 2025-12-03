#ifndef TRIE_HPP_
#define TRIE_HPP_
#include "Map.hpp"
#include "concepts.hpp"
#include <optional>
#include <stdexcept>

namespace IR {
template <Comparable TChar, typename TVal>
class Trie {
public:
	struct INode {
		virtual bool isTerm() const = 0;
		virtual bool canGoTo(const TChar& c) const = 0;
		virtual INode *goTo(const TChar& c) const = 0;
		virtual const std::optional<TVal> & value() const = 0;
		virtual ~INode() = default;
	};
private:

	class Node : public INode {
		Map<TChar, Node*> translations_;
		std::optional<TVal> value_;
	public:
		Node(const TVal & value)
			: value_(value) {}

		Node(TVal && value) noexcept
			: value_(std::move(value)) {}
		
		bool isTerm() const override final;
		bool canGoTo(const TChar& c) const override final;

		INode* goTo(const TChar& c) const override final;
		Node* goTo(const TChar& c);

		const std::optional<TVal> & value() const override final;
		std::optional<TVal> & value();

		bool isTerm_ = false;
	};

	Node * root_ = nullptr;
	size_t size = 0;

	Trie() = default;
};

template <Comparable TChar, typename TVal>
bool Trie<TChar, TVal>::Node::isTerm() const {
	return isTerm_;
}

template <Comparable TChar, typename TVal>
bool Trie<TChar, TVal>::Node::canGoTo(const TChar &c) const {
	return translations_.contains(c);
}

template <Comparable TChar, typename TVal>
Trie<TChar, TVal>::Node* Trie<TChar, TVal>::Node::goTo(const TChar &c) {
	if(canGoTo(c)) 
		return translations_[c];
	Node* node = new Node();
	translations_[c] = node;
	return node;
}

template <Comparable TChar, typename TVal>
Trie<TChar, TVal>::INode* Trie<TChar, TVal>::Node::goTo(const TChar &c) const {
	if(canGoTo(c)) 
		return translations_[c];
	throw std::runtime_error("Can go to at given value via no translation exists");
}

template <Comparable TChar, typename TVal>
const std::optional<TVal>& Trie<TChar, TVal>::Node::value() const {
	return value_;
}

template <Comparable TChar, typename TVal>
std::optional<TVal>& Trie<TChar, TVal>::Node::value() {
	return value_;
}

} // namespace IR
#endif // !TRIE_HPP_
