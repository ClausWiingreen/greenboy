#include "greenboy/instructions/byte_load.hpp"

#include <cassert>

#include "greenboy/data_access/byte_access.hpp"

namespace greenboy::instructions {

ByteLoad::ByteLoad(std::shared_ptr<data_access::ByteAccess> dest,
                   std::shared_ptr<const data_access::ByteAccess> src)
    : m_destination(std::move(dest)), m_source(std::move(src)) {
  assert(m_destination != nullptr);
  assert(m_source != nullptr);
}

cycles ByteLoad::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  m_destination->write(registers, memory, m_source->read(registers, memory));
  return m_destination->access_time() + m_source->access_time();
}
} // namespace greenboy::instructions