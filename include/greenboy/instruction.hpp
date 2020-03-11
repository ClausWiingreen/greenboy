#pragma once
#include "cpu.hpp"
#include "memory_bus.hpp"
#include "timing.hpp"

namespace greenboy {
/**
 * @brief an interface for instructions which are performed by the CPU
 *
 */
class Instruction {
public:
  //! @cond Doxygen_Suppress
  Instruction() = default;
  Instruction(const Instruction &) = delete;
  Instruction(Instruction &&) = delete;

  virtual ~Instruction() = default;

  Instruction &operator=(const Instruction &) = delete;
  Instruction &operator=(Instruction &&) = delete;
  //! @endcond

  /**
   * @brief executes the instruction
   *
   * @param registers the non memorymapped registers which may be modified by
   * the instruction
   * @param memory the memory map which may be written to or read from
   * @return the number of cpu cycles it took to execute this instruction
   */
  virtual cycles execute(CPU::RegisterSet &registers,
                         MemoryBus &memory) const = 0;
};

namespace instructions {
/**
 * @brief a no operation instruction.
 * advanced the pc by one over 4 cycles.
 */
class NOP : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers,
                 MemoryBus & /*memory*/) const override;
};

class CALL : public Instruction {
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

class RET : public Instruction {
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

} // namespace instructions
} // namespace greenboy
