#pragma once

#include "greenboy/cpu.hpp"
#include "greenboy/memory_bus.hpp"

namespace greenboy::data_access {
/**
 * @brief An interface that provides read and write access to a word
 *
 */
class WordAccess {
public:
  WordAccess() = default;
  WordAccess(const WordAccess &) = delete;
  WordAccess(WordAccess &&) = delete;

  virtual ~WordAccess() = default;

  WordAccess &operator=(const WordAccess &) = delete;
  WordAccess &operator=(WordAccess &&) = delete;
  /**
   * @brief
   *
   * @param registers
   * @param memory
   * @return byte
   */
  [[nodiscard]] virtual word read(CPU::RegisterSet &registers,
                                  MemoryBus &memory) const = 0;
  /**
   * @brief
   *
   * @param registers
   * @param memory
   * @param value
   */
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     word value) = 0;
  /**
   * @brief
   *
   * @return cycles
   */
  [[nodiscard]] virtual cycles access_time() const;
};
} // namespace greenboy::data_access
