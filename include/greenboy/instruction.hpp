#pragma once
#include <functional>
#include <memory>
#include <utility>

#include "cpu.hpp"
#include "memory_bus.hpp"
#include "timing.hpp"

#include "data_access/byte_access.hpp"
#include "data_access/word_access.hpp"

namespace greenboy {
/**
 * @brief This class acts as an interface for instructions which performed by
 * the CPU
 *
 */
class Instruction {
protected:
  /**
   * @brief Construct a new Instruction object
   *
   */
  Instruction() = default;

public:
  Instruction(const Instruction &) = delete;
  Instruction(Instruction &&) = delete;

  /**
   * @brief Destroy the Instruction object
   *
   * Virtual to allow destruction of child objects
   *
   */
  virtual ~Instruction() = default;

  Instruction &operator=(const Instruction &) = delete;
  Instruction &operator=(Instruction &&) = delete;

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
} // namespace greenboy
