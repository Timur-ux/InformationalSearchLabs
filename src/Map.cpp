#include "Map.hpp"

namespace IR {
Balance &operator+=(Balance &balance, int diff) {
  int newBalance = static_cast<int>(balance) + diff;
  assert(newBalance <= 2 && newBalance >= -2);

  balance = static_cast<Balance>(newBalance);
  return balance;
}

Balance &operator-=(Balance &balance, int diff) { return balance += (-diff); }
} // namespace IR
