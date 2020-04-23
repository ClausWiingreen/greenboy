#pragma once

#include "greenboy/cpu.hpp"
#include "greenboy/memory_bus.hpp"

namespace greenboy::data_access {
/**
 * @brief This class provides an interface that with read and write access to a
 * byte
 *
 */
class ByteAccess {
public:
  ByteAccess() = default;
  ByteAccess(const ByteAccess &) = delete;
  ByteAccess(ByteAccess &&) = delete;

  virtual ~ByteAccess() = default;

  ByteAccess &operator=(const ByteAccess &) = delete;
  ByteAccess &operator=(ByteAccess &&) = delete;

  /**
   * @brief read the byte
   *
   * Might throw a std::runtime_error if read or write is not supported in that
   * given implementation
   *
   * @param registers the registers from which the byte might be read from
   * @param memory the memory bus where the byte might be read from
   * @return byte the read byte
   */
  [[nodiscard]] virtual byte read(CPU::RegisterSet &registers,
                                  MemoryBus &memory) const = 0;
  /**
   * @brief write the value to a byte
   *
   * @param registers the registers from which the byte might be written to
   * @param memory the memory from which the byte might be written to
   * @param value the byte to write
   */
  virtual void write(CPU::RegisterSet &registers, MemoryBus &memory,
                     byte value) = 0;
  /**
   * @brief
   *
   * @return cycles
   */
  [[nodiscard]] virtual cycles access_time() const;
};
} // namespace greenboy::data_access