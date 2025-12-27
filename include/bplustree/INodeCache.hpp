#ifndef INODE_CACHE_HPP_
#define INODE_CACHE_HPP_
#include "List.hpp"
#include "Map.hpp"
#include "Queue.hpp"
#include "bplustree/Node.hpp"
#include "concepts.hpp"
#include "metrics.hpp"
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#pragma once
namespace IR::bplustree {
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class INodeCache {
public:
  using node_type = impl::Node<TKey, TVal, TOrdering>;
  virtual ~INodeCache() = default;

  virtual bool contains(long nodeId) const = 0;
  virtual node_type get(long nodeId) = 0;
  virtual void update(const node_type &node) = 0;
  virtual void insert(const node_type &node) = 0;
  virtual void flush(std::filesystem::path storage) const = 0;
  virtual bool full() const = 0;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class NullNodeCache : public INodeCache<TKey, TVal, TOrdering> {
public:
  using node_type = impl::Node<TKey, TVal, TOrdering>;
  NullNodeCache() = default;

  bool contains(long nodeId) const override final { return false; };
  node_type get(long nodeId) override final {
    throw std::logic_error("Default node cache can't get any nodes");
  };
  void update(const node_type &node) override final { return; };
  void insert(const node_type &node) override final { return; };
  void flush(std::filesystem::path storage) const override final { return; };
  bool full() const override final { return true; }
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
class DefaultNodeCache : public INodeCache<TKey, TVal, TOrdering> {
  void print(std::ostream &os) const {
    os << "------------\n";
    os << "Capacity: " << capacity_ << "; storagePath: " << storagePath_
       << "; activeNodes size: " << activeNodes_.size()
       << "; storage size: " << storage_.size()
       << "; history size: " << history_.size() << '\n';
    for (const auto &item : storage_)
      os << item.timePoint << ' ' << item.node.id << '\n';
    os << "------------\n";
  }

public:
  using node_type = impl::Node<TKey, TVal, TOrdering>;
  size_t currentTimePoint_ = 0;

private:
  size_t capacity_;
  std::filesystem::path storagePath_;
  struct CacheItem {
    size_t timePoint;
    node_type node;
  };

  struct CachingHistory {
    size_t timePoint;
    long nodeId;
  };

  List<CacheItem> storage_;
  Queue<CachingHistory> history_;
  Map<long, std::optional<typename List<CacheItem>::iterator>> activeNodes_;

public:
  DefaultNodeCache(std::filesystem::path storagePath, size_t capacity = 1024)
      : storagePath_(storagePath), capacity_(capacity) {}

  bool contains(long nodeId) const override final;
  node_type get(long nodeId) override final;
  void update(const node_type &node) override final;
  void insert(const node_type &node) override final;
  void flush(std::filesystem::path storage) const override final;
  bool full() const override final;
};

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
bool DefaultNodeCache<TKey, TVal, TOrdering>::contains(long nodeId) const {
  return activeNodes_.contains(nodeId);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
DefaultNodeCache<TKey, TVal, TOrdering>::node_type
DefaultNodeCache<TKey, TVal, TOrdering>::get(long nodeId) {
  if (!contains(nodeId))
    throw std::logic_error("Can't get node from cache that not exists in it");
	incrementCacheUsage();

  auto it = *activeNodes_[nodeId];
  history_.push(CachingHistory{currentTimePoint_, nodeId});
  it->timePoint = (currentTimePoint_++);
  return it->node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void DefaultNodeCache<TKey, TVal, TOrdering>::update(const node_type &node) {
  if (!contains(node.id))
    insert(node);
  else
    incrementCacheUsage();
  auto it = *activeNodes_[node.id];
  it->timePoint = currentTimePoint_++;
  it->node = node;
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void DefaultNodeCache<TKey, TVal, TOrdering>::insert(const node_type &node) {
  incrementCacheUsage();
  if (full()) { // find first not changed node
    auto oldest = storage_.end();
    do {
      CachingHistory oldestHistory = history_.front();
      history_.pop();
      if (!activeNodes_.contains(oldestHistory.nodeId))
        continue;
      auto current = *activeNodes_[oldestHistory.nodeId];

      if (current->timePoint == oldestHistory.timePoint ||
          oldest == storage_.end()) {
        oldest = current;
        break;
      } else if (oldest->timePoint > current->timePoint)
        oldest = current,
        history_.push(CachingHistory{current->timePoint, current->node.id});
    } while (!history_.empty());
    if (oldest == storage_.end()) {
      oldest = storage_.begin();
      for (auto it = std::begin(storage_); it != std::end(storage_); ++it)
        if (it->timePoint < oldest->timePoint)
          oldest = it;
    }
    node_type::save(storagePath_, oldest->node);
    activeNodes_.remove(oldest->node.id);
    storage_.remove(oldest);
  }

  auto it =
      storage_.emplace(storage_.end(), CacheItem{currentTimePoint_++, node});
  activeNodes_.insert(node.id, it);
  history_.push(CachingHistory{it->timePoint, node.id});
}
template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
void DefaultNodeCache<TKey, TVal, TOrdering>::flush(
    std::filesystem::path storage) const {
  for (const CacheItem &item : storage_)
    node_type::save(storage, item.node);
}

template <Comparable TKey, typename TVal, SameKeyOrdering TOrdering>
bool DefaultNodeCache<TKey, TVal, TOrdering>::full() const {
  return storage_.size() == capacity_;
}

} // namespace IR::bplustree
#endif // !INODE_CACHE_HPP_
