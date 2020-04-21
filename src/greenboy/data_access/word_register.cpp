#include "greenboy/data_access/word_register.hpp"

namespace greenboy::data_access {
WordRegister::WordRegister(CPU::R16 reg) : m_reg(reg) {}
word WordRegister::read(CPU::RegisterSet &registers,
                        MemoryBus & /* memory */) const {
  switch (m_reg) {
  case CPU::R16::BC:
    return word{registers.b, registers.c};
  case CPU::R16::DE:
    return word{registers.d, registers.e};
  case CPU::R16::HL:
    return word{registers.h, registers.l};
  case CPU::R16::SP:
    return registers.sp;
  case CPU::R16::PC:
    return registers.pc;
  case CPU::R16::AF:
    return word{registers.a, static_cast<byte>(registers.f)};
  default:
    throw std::runtime_error("Tried to read from an unknown 16 bit register");
  }
}
void WordRegister::write(CPU::RegisterSet &registers, MemoryBus & /* memory */,
                         word value) {
  switch (m_reg) {
  case CPU::R16::BC:
    registers.b = value.high();
    registers.c = value.low();
    break;
  case CPU::R16::DE:
    registers.d = value.high();
    registers.e = value.low();
    break;
  case CPU::R16::HL:
    registers.h = value.high();
    registers.l = value.low();
    break;
  case CPU::R16::SP:
    registers.sp = value;
    break;
  case CPU::R16::PC:
    registers.pc = value;
    break;
  case CPU::R16::AF:
    registers.a = value.high();
    registers.f = CPU::Flags(value.low());
    break;
  default:
    throw std::runtime_error("Tried to write to an unknown 16 bit register");
  }
}
std::shared_ptr<WordRegister> WordRegister::bc() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::BC);
  return instance;
}
std::shared_ptr<WordRegister> WordRegister::de() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::DE);
  return instance;
}
std::shared_ptr<WordRegister> WordRegister::hl() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::HL);
  return instance;
}
std::shared_ptr<WordRegister> WordRegister::sp() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::SP);
  return instance;
}
} // namespace greenboy::data_access