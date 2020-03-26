#include "greenboy/instruction.hpp"

namespace greenboy::instructions {
cycles NOP::execute(CPU::RegisterSet &registers,
                    MemoryBus & /* memory */) const {
  ++registers.pc;
  return cycles{4};
}

cycles CALL::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
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
  ++registers.pc;
  memory.write(word(registers.l, registers.h), memory.read(registers.pc++));
  return cycles{12};
}

cycles RST::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;

  memory.write(registers.sp--, registers.pc.high());
  memory.write(registers.sp--, registers.pc.low());

  registers.pc = m_address;
  return cycles{16};
}

cycles LOAD_A_HLI::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  registers.a = memory.read(word{registers.l, registers.h});
  registers.l++;
  if (registers.l == byte{0}) {
    registers.h++;
  }
  return cycles{8};
}

cycles LOAD_A_HLD::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  registers.a = memory.read(word{registers.l, registers.h});
  registers.l--;
  if (registers.l == byte{0xff}) {
    registers.h--;
  }
  return cycles{8};
}
cycles LOAD_HLI_A::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  memory.write(word{registers.l, registers.h}, registers.a);
  registers.l++;
  if (registers.l == byte{0}) {
    registers.h++;
  }
  return cycles{8};
}

cycles LOAD_HLD_A::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  memory.write(word{registers.l, registers.h}, registers.a);
  registers.l--;
  if (registers.l == byte{0xff}) {
    registers.h--;
  }
  return cycles{8};
}

cycles LOAD_A_C::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  registers.a = memory.read(word{registers.c, byte{0xff}});
  return cycles{8};
}

cycles LOAD_C_A::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  memory.write(word{registers.c, byte{0xff}}, registers.a);
  return cycles{8};
}

cycles LOAD_A_n::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  registers.a = memory.read(word{memory.read(registers.pc++), byte{0xff}});
  return cycles{12};
}

cycles LOAD_n_A::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  memory.write(word{memory.read(registers.pc++), byte{0xff}}, registers.a);
  return cycles{12};
}

cycles LOAD_A_nn::execute(CPU::RegisterSet &registers,
                          MemoryBus &memory) const {
  ++registers.pc;
  auto low = memory.read(registers.pc++);
  auto high = memory.read(registers.pc++);
  registers.a = memory.read(word{low, high});
  return cycles{16};
}

cycles LOAD_nn_A::execute(CPU::RegisterSet &registers,
                          MemoryBus &memory) const {
  ++registers.pc;
  auto low = memory.read(registers.pc++);
  auto high = memory.read(registers.pc++);
  memory.write(word{low, high}, registers.a);
  return cycles{16};
}

cycles LOAD_R8_R8::execute(CPU::RegisterSet &registers,
                           MemoryBus & /* memory */) const {
  ++registers.pc;
  registers.reference(m_to) = registers.reference(m_from);
  return cycles{4};
}

cycles SET::execute(CPU::RegisterSet &registers,
                    MemoryBus & /* memory */) const {
  registers.pc++;
  registers.pc++;

  registers.reference(m_reg) |= byte{1u << m_bit};
  return cycles{8};
}
cycles RES::execute(CPU::RegisterSet &registers,
                    MemoryBus & /* memory */) const {
  registers.pc++;
  registers.pc++;

  registers.reference(m_reg) &= byte{static_cast<uint8_t>(~(1u << m_bit))};
  return cycles{8};
}

cycles LOAD_R8_n::execute(CPU::RegisterSet &registers,
                          MemoryBus &memory) const {
  ++registers.pc;
  registers.reference(m_to) = memory.read(registers.pc++);
  return cycles{8};
}
cycles LOAD_R8_HL::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  registers.reference(m_to) = memory.read(word(registers.l, registers.h));
  return cycles{8};
}
cycles LOAD_HL_R8::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  memory.write(word(registers.l, registers.h), registers.reference(m_from));
  return cycles{8};
}
cycles LOAD_A_R16::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  auto word_register = registers.reference(m_source);
  registers.a = memory.read(word_register.to_word());
  return cycles{8};
}
cycles LOAD_R16_A::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  auto word_register = registers.reference(m_destination);
  memory.write(word_register.to_word(), registers.a);
  return cycles{8};
}
cycles LOAD_R16_nn::execute(CPU::RegisterSet &registers,
                            MemoryBus &memory) const {
  ++registers.pc;
  auto word_register = registers.reference(m_destination);
  word_register.low(memory.read(registers.pc++));
  word_register.high(memory.read(registers.pc++));
  return cycles{12};
}
} // namespace greenboy::instructions
