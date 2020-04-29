#include "greenboy/instructions/word_load.hpp"

namespace greenboy::instructions {
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
} // namespace greenboy::instructions
