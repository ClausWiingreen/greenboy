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
  explicit FetchExecuteCPU(std::unique_ptr<MemoryBus> memory);

  cycles update() override;
};
} // namespace greenboy
