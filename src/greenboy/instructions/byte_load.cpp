#include "greenboy/instructions/byte_load.hpp"

namespace greenboy::instructions {

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
} // namespace greenboy::instructions