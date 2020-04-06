
#pragma once
#include <cstdint>

#include "types.hpp"

namespace greenboy {
/**
 * Provides memory mapped methods for reading and writing to memory.
 */
class MemoryBus {
public:
  //! @cond Doxygen_Suppress
  MemoryBus() = default;
  MemoryBus(const MemoryBus &) = delete;
  MemoryBus(MemoryBus &&) = delete;

  virtual ~MemoryBus() = default;

  MemoryBus &operator=(const MemoryBus &) = delete;
  MemoryBus &operator=(MemoryBus &&) = delete;
  //! @endcond

  /**
   * Performs a memory mapped read which delegates part of the memory
   * space to different components.
   *
   * \param address the address to read from.
   * \returns the byte at the address pointed to by the address.
   */
  virtual byte read(word address) const = 0;

  /**
   * Performs a memory mapped write which delegates part of the memory
   * space to different components.
   *
   * \param address the address to write to.
   * \param value the value to write to memory.
   */
  virtual void write(word address, byte value) = 0;
};
} // namespace greenboy
