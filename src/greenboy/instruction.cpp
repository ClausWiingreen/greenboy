#include "greenboy/instruction.hpp"

namespace greenboy {
namespace instructions {
ByteLoad::ByteLoad(std::shared_ptr<data_access::ByteAccess> dest,
                   std::shared_ptr<const data_access::ByteAccess> src)
    : m_destination(std::move(dest)), m_source(std::move(src)) {
  if (m_source == nullptr) {
    throw std::invalid_argument("source may not be null");
  }

  if (m_destination == nullptr) {
    throw std::invalid_argument("destination may not be null");
  }
}

cycles ByteLoad::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  m_destination->write(registers, memory, m_source->read(registers, memory));
  return cycles(4);
}
} // namespace instructions

namespace data_access {
byte ByteRegisterAccess::read(CPU::RegisterSet &registers,
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
void ByteRegisterAccess::write(CPU::RegisterSet &registers,
                               MemoryBus & /* memory */, byte value) {
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
byte ImmediateByteAccess::read(CPU::RegisterSet &registers,
                               MemoryBus &memory) const {
  return memory.read(registers.pc++);
}
void ImmediateByteAccess::write(CPU::RegisterSet & /* registers */,
                                MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to immediate byte");
}
byte IndirectByteAccess::read(CPU::RegisterSet &registers,
                              MemoryBus &memory) const {
  return memory.read(m_pointer->read(registers, memory));
}
void IndirectByteAccess::write(CPU::RegisterSet &registers, MemoryBus &memory,
                               byte value) {
  memory.write(m_pointer->read(registers, memory), value);
}
word WordRegisterAccess::read(CPU::RegisterSet &registers,
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
    return word{registers.a, registers.f.operator greenboy::byte()};
  default:
    throw std::runtime_error("Tried to read from an unknown 16 bit register");
  }
}
void WordRegisterAccess::write(CPU::RegisterSet &registers,
                               MemoryBus & /* memory */, word value) {
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
byte ConstantByteAccess::read(CPU::RegisterSet & /* registers */,
                              MemoryBus & /* memory */) const {
  return m_value;
}
void ConstantByteAccess::write(CPU::RegisterSet & /* registers */,
                               MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to a constant 8 bit value");
}
word ImmediateWordAccess::read(CPU::RegisterSet &registers,
                               MemoryBus &memory) const {
  auto low = m_access.read(registers, memory);
  auto high = m_access.read(registers, memory);
  return word(high, low);
}
void ImmediateWordAccess::write(CPU::RegisterSet & /* registers */,
                                MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to immediate word");
}

word DoubleByteWordAccess::read(CPU::RegisterSet &registers,
                                MemoryBus &memory) const {
  return word(m_high->read(registers, memory), m_low->read(registers, memory));
}
void DoubleByteWordAccess::write(CPU::RegisterSet &registers, MemoryBus &memory,
                                 word value) {
  m_high->write(registers, memory, value.high());
  m_low->write(registers, memory, value.low());
}
byte IndirectAndIncrementByteAccess::read(CPU::RegisterSet &registers,
                                          MemoryBus &memory) const {
  auto result = m_inner.read(registers, memory);
  m_pointer->write(registers, memory, ++m_pointer->read(registers, memory));
  return result;
}
void IndirectAndIncrementByteAccess::write(CPU::RegisterSet &registers,
                                           MemoryBus &memory, byte value) {
  m_inner.write(registers, memory, value);
  m_pointer->write(registers, memory, ++m_pointer->read(registers, memory));
}
byte IndirectAndDecrementByteAccess::read(CPU::RegisterSet &registers,
                                          MemoryBus &memory) const {
  auto result = m_inner.read(registers, memory);
  m_pointer->write(registers, memory, --m_pointer->read(registers, memory));
  return result;
}
void IndirectAndDecrementByteAccess::write(CPU::RegisterSet &registers,
                                           MemoryBus &memory, byte value) {
  m_inner.write(registers, memory, value);
  m_pointer->write(registers, memory, --m_pointer->read(registers, memory));
}
} // namespace data_access
} // namespace greenboy
