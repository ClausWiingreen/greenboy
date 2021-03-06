#include "greenboy/data_access/word_register.hpp"

#include <stdexcept>

namespace greenboy::data_access {
WordRegister::WordRegister(CPU::R16 reg) : m_reg(reg) {}
word WordRegister::read(CPU::RegisterSet &registers,
                        MemoryBus & /* memory */) const {
  switch (m_reg) {
  case CPU::R16::BC:
    return to_word(registers.b, registers.c);
  case CPU::R16::DE:
    return to_word(registers.d, registers.e);
  case CPU::R16::HL:
    return to_word(registers.h, registers.l);
  case CPU::R16::SP:
    return registers.sp;
  case CPU::R16::PC:
    return registers.pc;
  case CPU::R16::AF:
    return to_word(registers.a, static_cast<byte>(registers.f));
  default:
    throw std::runtime_error("Tried to read from an unknown 16 bit register");
  }
}
void WordRegister::write(CPU::RegisterSet &registers, MemoryBus & /* memory */,
                         word value) {
  switch (m_reg) {
  case CPU::R16::BC:
    registers.b = high_byte(value);
    registers.c = low_byte(value);
    break;
  case CPU::R16::DE:
    registers.d = high_byte(value);
    registers.e = low_byte(value);
    break;
  case CPU::R16::HL:
    registers.h = high_byte(value);
    registers.l = low_byte(value);
    break;
  case CPU::R16::SP:
    registers.sp = value;
    break;
  case CPU::R16::PC:
    registers.pc = value;
    break;
  case CPU::R16::AF:
    registers.a = high_byte(value);
    registers.f = CPU::Flags(low_byte(value));
    break;
  default:
    throw std::runtime_error("Tried to write to an unknown 16 bit register");
  }
}
} // namespace greenboy::data_access