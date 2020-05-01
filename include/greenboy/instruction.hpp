#pragma once
#include "cpu.hpp"

namespace greenboy {
class MemoryBus;

class Instruction {
protected:
  Instruction() = default;

public:
  Instruction(const Instruction &) = delete;
  Instruction(Instruction &&) = delete;

  virtual ~Instruction() = default;

  Instruction &operator=(const Instruction &) = delete;
  Instruction &operator=(Instruction &&) = delete;

  virtual cycles execute(CPU::RegisterSet &registers,
                         MemoryBus &memory) const = 0;
};
} // namespace greenboy
