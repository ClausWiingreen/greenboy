#include "greenboy/instruction.hpp"

#include <iomanip>
#include <iostream>

#include "greenboy/data_access/byte_access.hpp"
#include "greenboy/data_access/byte_register.hpp"
#include "greenboy/data_access/constant_byte.hpp"
#include "greenboy/data_access/decrementing_word.hpp"
#include "greenboy/data_access/double_byte_word.hpp"
#include "greenboy/data_access/delayed_word_access.hpp"
#include "greenboy/data_access/immediate_byte.hpp"
#include "greenboy/data_access/incrementing_word.hpp"
#include "greenboy/data_access/indirect_byte.hpp"
#include "greenboy/data_access/indirect_word.hpp"
#include "greenboy/data_access/offsat_word.hpp"
#include "greenboy/data_access/pre_decrementing_word.hpp"
#include "greenboy/data_access/word_access.hpp"
#include "greenboy/data_access/word_register.hpp"

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
PreDecrementingWord::PreDecrementingWord(std::shared_ptr<WordAccess> inner)
    : m_inner(std::move(inner)) {}
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
OffsatWord::OffsatWord(std::shared_ptr<WordAccess> access,
                       std::shared_ptr<ByteAccess> offset)
    : m_access(std::move(access)), m_offset(std::move(offset)) {}
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
cycles OffsatWord::access_time() const {
  return m_offset->access_time() + m_access->access_time();
}
std::shared_ptr<OffsatWord>
OffsatWord::from(std::shared_ptr<WordAccess> access,
                 std::shared_ptr<ByteAccess> offset) {
  return std::make_shared<OffsatWord>(access, offset);
}
IndirectWord::IndirectWord(std::shared_ptr<WordAccess> pointer)
    : m_pointer(std::move(pointer)) {}
word IndirectWord::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  auto pointer = m_pointer->read(registers, memory);
  auto low = memory.read(pointer);
  auto high = memory.read(++pointer);
  return word{high, low};
}
void IndirectWord::write(CPU::RegisterSet &registers, MemoryBus &memory,
                         word value) {
  auto pointer = m_pointer->read(registers, memory);
  memory.write(pointer, value.low());
  memory.write(++pointer, value.high());
}
cycles IndirectWord::access_time() const {
  return cycles{8} + m_pointer->access_time();
}
std::shared_ptr<IndirectWord>
IndirectWord::from(std::shared_ptr<WordAccess> pointer) {
  return std::make_shared<IndirectWord>(std::move(pointer));
}
cycles WordAccess::access_time() const { return cycles{}; }
DelayedWordAccess::DelayedWordAccess(std::shared_ptr<WordAccess> inner)
    : m_inner(std::move(inner)) {}
word DelayedWordAccess::read(CPU::RegisterSet &registers,
                             MemoryBus &memory) const {
  return m_inner->read(registers, memory);
}
void DelayedWordAccess::write(CPU::RegisterSet &registers, MemoryBus &memory,
                              word value) {
  m_inner->write(registers, memory, value);
}
cycles DelayedWordAccess::access_time() const {
  return cycles{4} + m_inner->access_time();
}
std::shared_ptr<DelayedWordAccess>
DelayedWordAccess::from(std::shared_ptr<WordAccess> inner) {
  return std::make_shared<DelayedWordAccess>(inner);
}
} // namespace data_access
} // namespace greenboy
