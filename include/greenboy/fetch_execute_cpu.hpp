#pragma once
#include "cpu.hpp"

#include <memory>

#include "memory_bus.hpp"

namespace greenboy {
/**
 * emulates the cpu using a classic fetch-execution instruction cycle
 */
class FetchExecuteCPU : public CPU {
  std::unique_ptr<MemoryBus> m_memory;

public:
  /**
   * The main constructor for the FetchExecuteCPU.
   * It provides opertunities for dependency injection.
   * 
   * \param memory The memory bus to use for reading or writing.
   * Will throw std::invalid_argument if it is null.
   */
  explicit FetchExecuteCPU(std::unique_ptr<MemoryBus> memory);

  cycles update() override;
};
} // namespace greenboy
