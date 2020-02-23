#pragma once
#include <cstdint>

#include "types.hpp"

namespace greenboy {
class MemoryBus {
public:
  virtual ~MemoryBus() = default;

  virtual byte read(word address) = 0;
  virtual void write(word address, byte value) = 0;
};
} // namespace greenboy
