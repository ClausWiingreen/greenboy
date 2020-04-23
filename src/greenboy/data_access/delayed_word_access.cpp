#include "greenboy/data_access/delayed_word_access.hpp"

namespace greenboy::data_access {
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
} // namespace greenboy::data_access