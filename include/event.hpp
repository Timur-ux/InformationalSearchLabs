#ifndef EVENT_HPP_
#define EVENT_HPP_
#include "List.hpp"
#include <memory>
#pragma once

namespace IR::event {
template <typename... Args> class IEventHandler;

template <typename... Args> class IEvent {
public:
  virtual IEvent<Args...> &
  operator+=(std::shared_ptr<IEventHandler<Args...>> handler) = 0;
  virtual IEvent<Args...> &
  operator-=(std::shared_ptr<IEventHandler<Args...>> handler) = 0;
};

template <typename... Args> class IEventHandler {
public:
  virtual void operator()(Args &&...args) = 0;
};

template <typename... Args> class Event : public IEvent<Args...> {
  using handler_t = IEventHandler<Args...>;
  List<std::weak_ptr<handler_t>> handlers_;

public:
  void invoke(Args &&...args) {
    for (auto handler : handlers_)
      if (std::shared_ptr<handler_t> ptr = handler.lock())
        (*ptr)(std::forward(args)...);
  }

  IEvent<Args...> &
  operator+=(std::shared_ptr<handler_t> handler) override final {
    handlers_.emplace_back(handler);
    return *this;
  }

  IEvent<Args...> &
  operator-=(std::shared_ptr<handler_t> handler) override final {
    auto it = handlers_.begin(), end = handlers_.end();
    while (*it != handler && it != end)
      ++it;

    if (it == end)
      return *this;

    handlers_.remove(it);
    return *this;
  }
};
} // namespace IR::event

#endif // !EVENT_HPP_
