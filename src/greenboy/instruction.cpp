#include "greenboy/instruction.hpp"

namespace greenboy {
byte Add(byte a, byte b, CPU::Flags &f) {

  auto a_val = a.value();
  auto b_val = b.value();
  auto result = static_cast<unsigned>(a_val + b_val);
  auto carry = result ^ a_val ^ b_val;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = (carry & 1u << 4u) != 0;
  f.carry = (carry & 1u << 8u) != 0;

  return byte{static_cast<uint8_t>(result)};
}

byte AddWithCarry(byte a, byte b, CPU::Flags &f) {
  return Add(a, byte{static_cast<uint8_t>(b.value() + (f.carry ? 1 : 0))}, f);
}
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
  m_destination->write(registers, memory, m_source->read(registers, memory));
  return m_destination->access_time() + m_source->access_time();
}
WordLoad::WordLoad(std::shared_ptr<data_access::WordAccess> dest,
                   std::shared_ptr<const data_access::WordAccess> src)
    : m_destination(std::move(dest)), m_source(std::move(src)) {
  if (m_source == nullptr) {
    throw std::invalid_argument("source may not be null");
  }

  if (m_destination == nullptr) {
    throw std::invalid_argument("destination may not be null");
  }
}
cycles WordLoad::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  m_destination->write(registers, memory, m_source->read(registers, memory));
  return m_destination->access_time() + m_source->access_time();
}

} // namespace instructions

namespace data_access {
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
std::shared_ptr<ByteRegister> ByteRegister::b() {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::B);
  return instance;
}
std::shared_ptr<ByteRegister> ByteRegister::c() {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::C);
  return instance;
}
std::shared_ptr<ByteRegister> ByteRegister::d() {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::D);
  return instance;
}
std::shared_ptr<ByteRegister> ByteRegister::h() {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::H);
  return instance;
}
std::shared_ptr<ByteRegister> ByteRegister::a() {
  static auto instance = std::make_shared<ByteRegister>(CPU::R8::A);
  return instance;
}
byte ImmediateByte::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  return memory.read(registers.pc++);
}
void ImmediateByte::write(CPU::RegisterSet & /* registers */,
                          MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to immediate byte");
}
std::shared_ptr<ImmediateByte> ImmediateByte::instance() {
  static auto instance = std::make_shared<ImmediateByte>();
  return instance;
}
byte IndirectByte::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  return memory.read(m_pointer->read(registers, memory));
}
void IndirectByte::write(CPU::RegisterSet &registers, MemoryBus &memory,
                         byte value) {
  memory.write(m_pointer->read(registers, memory), value);
}
std::shared_ptr<IndirectByte>
IndirectByte::from(std::shared_ptr<WordAccess> pointer) {
  return std::make_shared<IndirectByte>(std::move(pointer));
}
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
    return word{registers.a, registers.f.operator greenboy::byte()};
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
byte ConstantByte::read(CPU::RegisterSet & /* registers */,
                        MemoryBus & /* memory */) const {
  return m_value;
}
void ConstantByte::write(CPU::RegisterSet & /* registers */,
                         MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to a constant 8 bit value");
}
std::shared_ptr<ConstantByte> ConstantByte::from(byte value) {
  return std::make_shared<ConstantByte>(value);
}

word DoubleByteWord::read(CPU::RegisterSet &registers,
                          MemoryBus &memory) const {
  // Order is important if the source are both immediate values
  auto low = m_low->read(registers, memory);
  auto high = m_high->read(registers, memory);
  return word(high, low);
}
void DoubleByteWord::write(CPU::RegisterSet &registers, MemoryBus &memory,
                           word value) {
  // Order is important for push instruction
  m_high->write(registers, memory, value.high());
  m_low->write(registers, memory, value.low());
}
std::shared_ptr<DoubleByteWord>
DoubleByteWord::from(std::shared_ptr<ByteAccess> high,
                     std::shared_ptr<ByteAccess> low) {
  return std::make_shared<DoubleByteWord>(std::move(high), std::move(low));
}
word IncrementingWord::read(CPU::RegisterSet &registers,
                            MemoryBus &memory) const {
  auto value = m_inner->read(registers, memory);
  auto copy = value;
  m_inner->write(registers, memory, ++value);
  return copy;
}
void DecrementingWord::write(CPU::RegisterSet & /* registers */,
                             MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to an decrementing word");
}
word DecrementingWord::read(CPU::RegisterSet &registers,
                            MemoryBus &memory) const {
  auto value = m_inner->read(registers, memory);
  auto copy = value;
  m_inner->write(registers, memory, --value);
  return copy;
}
void IncrementingWord::write(CPU::RegisterSet & /* registers */,
                             MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to an incrementing word");
}

std::shared_ptr<IncrementingWord>
IncrementingWord::from(std::shared_ptr<WordAccess> inner) {
  return std::make_shared<IncrementingWord>(std::move(inner));
}

std::shared_ptr<DecrementingWord>
DecrementingWord::from(std::shared_ptr<WordAccess> inner) {
  return std::make_shared<DecrementingWord>(std::move(inner));
}
word PreDecrementingWord::read(CPU::RegisterSet &registers,
                               MemoryBus &memory) const {
  auto value = m_inner->read(registers, memory);
  --value;
  m_inner->write(registers, memory, value);
  return value;
}
void PreDecrementingWord::write(CPU::RegisterSet & /* registers */,
                                MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to an predecrementing word");
}
std::shared_ptr<PreDecrementingWord>
PreDecrementingWord::from(std::shared_ptr<WordAccess> inner) {
  return std::make_shared<PreDecrementingWord>(std::move(inner));
}
word OffsatWord::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  auto value = m_access->read(registers, memory);
  auto offset_low = m_offset->read(registers, memory);
  auto offset_high = byte{static_cast<uint8_t>(
      (offset_low.value() & (1u << 7u)) != 0 ? 0xff : 0x00)};

  auto low = Add(value.low(), offset_low, registers.f);
  auto high = AddWithCarry(value.high(), offset_high, registers.f);

  return word{high, low};
}
void OffsatWord::write(CPU::RegisterSet & /* registers */,
                       MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to an offsat word");
}
} // namespace data_access
} // namespace greenboy
