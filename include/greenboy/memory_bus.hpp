#pragma once
#include "types.hpp"

namespace greenboy {
class MemoryBus {
public:
  MemoryBus() = default;
  MemoryBus(const MemoryBus &) = delete;
  MemoryBus(MemoryBus &&) = delete;

  virtual ~MemoryBus() = default;

  MemoryBus &operator=(const MemoryBus &) = delete;
  MemoryBus &operator=(MemoryBus &&) = delete;

  virtual byte read(word address) const = 0;

  virtual void write(word address, byte value) = 0;
};
} // namespace greenboy
