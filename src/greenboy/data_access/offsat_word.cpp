#include "greenboy/data_access/offsat_word.hpp"

greenboy::byte Add(greenboy::byte a, greenboy::byte b,
                   greenboy::CPU::Flags &f) {

  auto a_val = a.value();
  auto b_val = b.value();
  auto result = static_cast<unsigned>(a_val + b_val);
  auto carry = result ^ a_val ^ b_val;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = (carry & 1u << 4u) != 0;
  f.carry = (carry & 1u << 8u) != 0;

  return greenboy::byte{static_cast<uint8_t>(result)};
}

greenboy::byte AddWithCarry(greenboy::byte a, greenboy::byte b,
                            greenboy::CPU::Flags &f) {
  return Add(
      a, greenboy::byte{static_cast<uint8_t>(b.value() + (f.carry ? 1 : 0))},
      f);
}

namespace greenboy::data_access {
OffsatWord::OffsatWord(std::shared_ptr<WordAccess> access,
                       std::shared_ptr<ByteAccess> offset)
    : m_access(std::move(access)), m_offset(std::move(offset)) {
  if (m_offset == nullptr) {
    throw std::runtime_error(
        "Cannot create Offsat word with offset sat to null");
  }
  if (m_access == nullptr) {
    throw std::runtime_error(
        "Cannot create Offsat word with access sat to null");
  }
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
cycles OffsatWord::access_time() const {
  return m_offset->access_time() + m_access->access_time();
}
std::shared_ptr<OffsatWord>
OffsatWord::from(std::shared_ptr<WordAccess> access,
                 std::shared_ptr<ByteAccess> offset) {
  return std::make_shared<OffsatWord>(access, offset);
}
} // namespace greenboy::data_access
