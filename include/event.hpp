#ifndef EVENT_HPP_
#define EVENT_HPP_
#include <memory>
#include "List.hpp"
#pragma once

namespace IR::event {
template<typename ...Args>
class IEvent {
	public:

};

template<typename ...Args>
class IEventHandler {
public:
	virtual void operator()(Args&& ...args) = 0;
};

template<typename ...Args>
class Event : public IEvent<Args...> {
	List<std::weak_ptr<IEventHandler<Args...>>> handlers_;
	public:
	
};
} // namespace IR::event

#endif // !EVENT_HPP_
