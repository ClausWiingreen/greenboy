#pragma once

#include "greenboy/cpu.hpp"

namespace greenboy {
class MemoryBus;

namespace data_access {
class ByteAccess {
public:
  ByteAccess() noexcept = default;
  ByteAccess(const ByteAccess &) = delete;
  ByteAccess(ByteAccess &&) = delete;

  virtual ~ByteAccess() = default;

  ByteAccess &operator=(const ByteAccess &) = delete;
  ByteAccess &operator=(ByteAccess &&) = delete;

  [[nodiscard]] virtual byte read(CPU::RegisterSet &registers,
                                  MemoryBus &memory) const = 0;
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     byte value) = 0;
  [[nodiscard]] virtual cycles access_time() const noexcept { return cycles{}; }
};
} // namespace data_access
} // namespace greenboy