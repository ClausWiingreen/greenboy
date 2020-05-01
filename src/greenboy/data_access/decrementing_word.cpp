#include "greenboy/data_access/decrementing_word.hpp"

#include <stdexcept>

namespace greenboy::data_access {
void DecrementingWord::write(CPU::RegisterSet & /* registers */,
                             MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to an decrementing word");
}
DecrementingWord::DecrementingWord(std::shared_ptr<WordAccess> inner)
    : m_inner(std::move(inner)) {}
word DecrementingWord::read(CPU::RegisterSet &registers,
                            MemoryBus &memory) const {
  auto value = m_inner->read(registers, memory);
  auto copy = value;
  m_inner->write(registers, memory, --value);
  return copy;
}
std::shared_ptr<DecrementingWord>
DecrementingWord::from(std::shared_ptr<WordAccess> inner) {
  return std::make_shared<DecrementingWord>(std::move(inner));
}
} // namespace greenboy::data_access
