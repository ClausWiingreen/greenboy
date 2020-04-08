#pragma once
#include "cpu.hpp"

#include <memory>

#include "memory_bus.hpp"
#include "opcode_translator.hpp"

namespace greenboy {
/**
 * emulates the cpu using a classic fetch-execution instruction cycle
 */
class FetchExecuteCPU : public CPU {
  std::unique_ptr<MemoryBus> m_memory;
  CPU::RegisterSet m_registers{};
  std::unique_ptr<OpcodeTranslator> m_controlUnit;

public:
  /**
   * The main constructor for the FetchExecuteCPU.
   * It provides opertunities for dependency injection.
   *
   * \param memory The memory bus to use for reading or writing.
   * \param controlUnit A unit that translates opcodes to instructions.
   * Will throw std::invalid_argument if it is null.
   */
  FetchExecuteCPU(std::unique_ptr<MemoryBus> memory,
                  std::unique_ptr<OpcodeTranslator> controlUnit);

  cycles update() override;
};
} // namespace greenboy
