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

cycles LOAD_R16_nn::execute(CPU::RegisterSet &registers,
                            MemoryBus &memory) const {
  ++registers.pc;
  auto word_register = registers.reference(m_destination);
  word_register.low(memory.read(registers.pc++));
  word_register.high(memory.read(registers.pc++));
  return cycles{12};
}

cycles PUSH_R16::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  auto reg = registers.reference(m_register);
  memory.write(registers.sp--, reg.high());
  memory.write(registers.sp--, reg.low());
  return cycles{16};
}

cycles POP_R16::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  ++registers.pc;
  auto reg = registers.reference(m_register);
  reg.from_word(word{memory.read(++registers.sp), memory.read(++registers.sp)});
  return cycles{12};
}

constexpr bool is_bit_set(unsigned value, unsigned index) {
  return (value & (1u << index)) == (1u << index);
}

cycles LOAD_HL_SP_e::execute(CPU::RegisterSet &registers,
                             MemoryBus &memory) const {
  ++registers.pc;

  auto offset_value = memory.read(registers.pc++).value();
  auto offset = static_cast<unsigned>(
      is_bit_set(offset_value, 7) ? offset_value - 256 : offset_value);
  auto sp = registers.sp.value();
  auto result = sp + offset;
  auto carry = result ^ sp ^ offset;

  registers.f.zero = false;
  registers.f.negate = false;
  registers.f.half_carry = is_bit_set(carry, 4u);
  registers.f.carry = is_bit_set(carry, 8u);

  registers.reference(CPU::R16::HL)
      .from_word(word{static_cast<uint16_t>(result)});

  return cycles{12};
}

cycles LOAD_nn_SP::execute(CPU::RegisterSet &registers,
                           MemoryBus &memory) const {
  ++registers.pc;
  auto low = memory.read(registers.pc++);
  auto high = memory.read(registers.pc++);
  word address{low, high};
  memory.write(address++, registers.sp.low());
  memory.write(address, registers.sp.high());
  return cycles{20};
}

ByteLoad::ByteLoad(std::shared_ptr<ByteAccess> dest,
                   std::shared_ptr<const ByteAccess> src)
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
byte ByteRegisterAccess::read(CPU::RegisterSet &registers,
                              const MemoryBus & /* memory */) const {
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
  default:
    throw std::runtime_error("Tried to write to an unknown 8 bit register");
  }
}
byte ImmediateByteAccess::read(CPU::RegisterSet &registers,
                               const MemoryBus &memory) const {
  return memory.read(registers.pc++);
}
void ImmediateByteAccess::write(CPU::RegisterSet & /* registers */,
                                MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to immediate byte");
}
byte IndirectByteAccess::read(CPU::RegisterSet &registers,
                              const MemoryBus &memory) const {
  return memory.read(m_pointer->read(registers, memory));
}
void IndirectByteAccess::write(CPU::RegisterSet &registers, MemoryBus &memory,
                               byte value) {
  memory.write(m_pointer->read(registers, memory), value);
}
word WordRegisterAccess::read(CPU::RegisterSet &registers,
                              const MemoryBus & /* memory */) const {
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
byte ByteConstantAccess::read(CPU::RegisterSet & /* registers */,
                              const MemoryBus & /* memory */) const {
  return m_value;
}
void ByteConstantAccess::write(CPU::RegisterSet & /* registers */,
                               MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to a constant 8 bit value");
}
word ImmediateWordAccess::read(CPU::RegisterSet &registers,
                               const MemoryBus &memory) const {
  auto low = m_access.read(registers, memory);
  auto high = m_access.read(registers, memory);
  return word(high, low);
}
void ImmediateWordAccess::write(CPU::RegisterSet & /* registers */,
                                MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to immediate word");
}
cycles WordLoad::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  m_destination->write(registers, memory, m_source->read(registers, memory));
  return cycles();
}
} // namespace greenboy::instructions
