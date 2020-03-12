#include "greenboy/instruction.hpp"

namespace greenboy::instructions {
cycles NOP::execute(CPU::RegisterSet &registers, MemoryBus & /*memory*/) const {
  registers.pc++;
  return cycles{4};
}

cycles CALL::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  registers.pc++;
  auto low = memory.read(registers.pc++);
  auto high = memory.read(registers.pc++);

  memory.write(registers.sp--, registers.pc.high());
  memory.write(registers.sp--, registers.pc.low());

  registers.pc = word(low, high);
  return cycles{24};
}

cycles RET::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  registers.pc = word(memory.read(++registers.sp), memory.read(++registers.sp));
  return cycles{16};
}

cycles LOAD_HL_n::execute(CPU::RegisterSet &registers,
                          MemoryBus &memory) const {
  registers.pc++;
  memory.write(word(registers.l, registers.h), memory.read(registers.pc++));
  return cycles{12};
}

} // namespace greenboy::instructions
