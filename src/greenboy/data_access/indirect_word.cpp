#include "greenboy/data_access/indirect_word.hpp"
namespace greenboy::data_access {
IndirectWord::IndirectWord(std::shared_ptr<WordAccess> pointer)
    : m_pointer(std::move(pointer)) {}
word IndirectWord::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  auto pointer = m_pointer->read(registers, memory);
  auto low = memory.read(pointer);
  auto high = memory.read(++pointer);
  return to_word(high, low);
}
void IndirectWord::write(CPU::RegisterSet &registers, MemoryBus &memory,
                         word value) {
  auto pointer = m_pointer->read(registers, memory);
  memory.write(pointer, low_byte(value));
  memory.write(++pointer, high_byte(value));
}
cycles IndirectWord::access_time() const {
  return cycles{8} + m_pointer->access_time();
}
std::shared_ptr<IndirectWord>
IndirectWord::from(std::shared_ptr<WordAccess> pointer) {
  return std::make_shared<IndirectWord>(std::move(pointer));
}
} // namespace greenboy::data_access