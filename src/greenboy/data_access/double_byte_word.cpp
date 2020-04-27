#include "greenboy/data_access/double_byte_word.hpp"

namespace greenboy::data_access {
DoubleByteWord::DoubleByteWord(std::shared_ptr<ByteAccess> high,
                               std::shared_ptr<ByteAccess> low)
    : m_high(std::move(high)), m_low(std::move(low)) {}

word DoubleByteWord::read(CPU::RegisterSet &registers,
                          MemoryBus &memory) const {
  // Order is important if the source are both immediate values
  auto low = m_low->read(registers, memory);
  auto high = m_high->read(registers, memory);
  return to_word(high, low);
}
void DoubleByteWord::write(CPU::RegisterSet &registers, MemoryBus &memory,
                           word value) {
  // Order is important for push instruction
  m_high->write(registers, memory, high_byte(value));
  m_low->write(registers, memory, low_byte(value));
}
cycles DoubleByteWord::access_time() const {
  return m_high->access_time() + m_low->access_time();
}
std::shared_ptr<DoubleByteWord>
DoubleByteWord::from(std::shared_ptr<ByteAccess> high,
                     std::shared_ptr<ByteAccess> low) {
  return std::make_shared<DoubleByteWord>(std::move(high), std::move(low));
}
} // namespace greenboy::data_access
