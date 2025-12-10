#ifndef CONTIGIOUS_ALLOCATOR_HPP_
#define CONTIGIOUS_ALLOCATOR_HPP_
#pragma once

namespace IR::allocator {
template <typename T>
class ContigiousAllocator {
public:
	using value_type = T;
}
} // namespace IR::allocator
#endif // !CONTIGIOUS_ALLOCATOR_HPP_
