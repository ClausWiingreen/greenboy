#include "greenboy/data_access/pre_decrementing_word.hpp"

namespace greenboy::data_access {
PreDecrementingWord::PreDecrementingWord(std::shared_ptr<WordAccess> inner)
    : m_inner(std::move(inner)) {
  if (m_inner == nullptr) {
    throw std::runtime_error("Parameter inner may not be null");
  }
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
} // namespace greenboy::data_access