#include "greenboy/data_access/indirect_byte.hpp"

namespace greenboy::data_access {
IndirectByte::IndirectByte(std::shared_ptr<WordAccess> pointer)
    : m_pointer(std::move(pointer)) {}
byte IndirectByte::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  return memory.read(m_pointer->read(registers, memory));
}
void IndirectByte::write(CPU::RegisterSet &registers, MemoryBus &memory,
                         byte value) {
  memory.write(m_pointer->read(registers, memory), value);
}
cycles IndirectByte::access_time() const {
  return cycles{4} + m_pointer->access_time();
}
std::shared_ptr<IndirectByte>
IndirectByte::from(std::shared_ptr<WordAccess> pointer) {
  return std::make_shared<IndirectByte>(std::move(pointer));
}
} // namespace greenboy::data_access
