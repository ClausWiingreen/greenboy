#include "greenboy/data_access/constant_byte.hpp"

namespace greenboy::data_access {
ConstantByte::ConstantByte(byte value) : m_value(value) {}
byte ConstantByte::read(CPU::RegisterSet & /* registers */,
                        MemoryBus & /* memory */) const {
  return m_value;
}
void ConstantByte::write(CPU::RegisterSet & /* registers */,
                         MemoryBus & /* memory */, byte /* value */) {
  throw std::runtime_error("Tried to write to a constant 8 bit value");
}
std::shared_ptr<ConstantByte> ConstantByte::from(byte value) {
  return std::make_shared<ConstantByte>(value);
}
} // namespace greenboy::data_access
