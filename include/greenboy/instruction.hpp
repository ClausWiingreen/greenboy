#pragma once
#include "cpu.hpp"
#include "memory_bus.hpp"
#include "timing.hpp"

namespace greenboy {
class Instruction {
public:
  Instruction() = default;
  Instruction(const Instruction &) = delete;
  Instruction(Instruction &&) = delete;

  virtual ~Instruction() = default;

  Instruction &operator=(const Instruction &) = delete;
  Instruction &operator=(Instruction &&) = delete;

  virtual cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) = 0;
};

namespace instructions {
class NOP : public Instruction {
public:
  cycles execute(CPU::RegisterSet &registers, MemoryBus & /*memory*/) override {
    registers.pc++;
    return cycles{4};
  };
};
} // namespace instructions
} // namespace greenboy
