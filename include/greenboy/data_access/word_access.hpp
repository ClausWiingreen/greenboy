#pragma once

#include "greenboy/cpu.hpp"

namespace greenboy {
class MemoryBus;

namespace data_access {
class WordAccess {
public:
  WordAccess() = default;
  WordAccess(const WordAccess &) = delete;
  WordAccess(WordAccess &&) = delete;

  virtual ~WordAccess() = default;

  WordAccess &operator=(const WordAccess &) = delete;
  WordAccess &operator=(WordAccess &&) = delete;

  [[nodiscard]] virtual word read(CPU::RegisterSet &registers,
                                  MemoryBus &memory) const = 0;
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     word value) = 0;
  [[nodiscard]] virtual cycles access_time() const;
};
} // namespace data_access
} // namespace greenboy
