#include "greenboy/data_access/incrementing_word.hpp"

#include <stdexcept>

namespace greenboy::data_access {
IncrementingWord::IncrementingWord(std::shared_ptr<WordAccess> inner)
    : m_inner(std::move(inner)) {}
word IncrementingWord::read(CPU::RegisterSet &registers,
                            MemoryBus &memory) const {
  auto value = m_inner->read(registers, memory);
  auto copy = value;
  m_inner->write(registers, memory, ++value);
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
} // namespace greenboy::data_access