#include "greenboy/data_access/byte_register.hpp"

#include <stdexcept>

namespace greenboy::data_access {
ByteRegister::ByteRegister(CPU::R8 reg) noexcept : m_reg(reg) {}
byte ByteRegister::read(CPU::RegisterSet &registers,
                        MemoryBus & /* memory */) const {
  switch (m_reg) {
  case CPU::R8::B:
    return registers.b;
  case CPU::R8::C:
    return registers.c;
  case CPU::R8::D:
    return registers.d;
  case CPU::R8::E:
    return registers.e;
  case CPU::R8::H:
    return registers.h;
  case CPU::R8::L:
    return registers.l;
  case CPU::R8::A:
    return registers.a;
  default:
    throw std::runtime_error("Tried to read from an unknown 8 bit register");
  }
}
void ByteRegister::write(CPU::RegisterSet &registers, MemoryBus & /* memory */,
                         byte value) {
  switch (m_reg) {
  case CPU::R8::B:
    registers.b = value;
    break;
  case CPU::R8::C:
    registers.c = value;
    break;
  case CPU::R8::D:
    registers.d = value;
    break;
  case CPU::R8::E:
    registers.e = value;
    break;
  case CPU::R8::H:
    registers.h = value;
    break;
  case CPU::R8::L:
    registers.l = value;
    break;
  case CPU::R8::A:
    registers.a = value;
    break;
  default:
    throw std::runtime_error("Tried to write to an unknown 8 bit register");
  }
}
} // namespace greenboy::data_access