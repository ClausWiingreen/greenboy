#include "greenboy/instruction.hpp"

namespace greenboy::instructions {
cycles NOP::execute(CPU::RegisterSet &registers, MemoryBus & /*memory*/) const {
  registers.pc++;
  return cycles{4};
};
} // namespace greenboy::instructions
