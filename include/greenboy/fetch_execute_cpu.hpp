#pragma once
#include "cpu.hpp"

#include <memory>

namespace greenboy {
class MemoryBus;
class OpcodeTranslator;

class FetchExecuteCPU final : public CPU {
  std::unique_ptr<MemoryBus> m_memory;
  CPU::RegisterSet m_registers{};
  std::unique_ptr<OpcodeTranslator> m_controlUnit;

public:
  FetchExecuteCPU(std::unique_ptr<MemoryBus> memory,
                  std::unique_ptr<OpcodeTranslator> controlUnit) noexcept;

  cycles update() override;
};
} // namespace greenboy
