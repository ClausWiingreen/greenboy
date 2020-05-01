#include "greenboy/data_access/immediate_byte.hpp"

#include <stdexcept>

#include "greenboy/memory_bus.hpp"

namespace greenboy::data_access {
byte ImmediateByte::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  return memory.read(registers.pc++);
}
void ImmediateByte::write(CPU::RegisterSet & /* registers */,
                          MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to immediate byte");
}
cycles ImmediateByte::access_time() const noexcept { return cycles{4}; }
std::shared_ptr<ImmediateByte> ImmediateByte::instance() {
  static auto instance = std::make_shared<ImmediateByte>();
  return instance;
}
} // namespace greenboy::data_access
