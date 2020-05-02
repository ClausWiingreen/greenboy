#include "greenboy/instructions/add.hpp"

#include "greenboy/data_access/byte_access.hpp"

static greenboy::byte Add(greenboy::byte a, greenboy::byte b,
                          greenboy::CPU::Flags &f) {

  auto a_val = std::to_integer<unsigned>(a);
  auto b_val = std::to_integer<unsigned>(b);
  auto result = a_val + b_val;
  auto carry = result ^ a_val ^ b_val;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = (carry & 1u << 4u) != 0;
  f.carry = (carry & 1u << 8u) != 0;

  return greenboy::byte{static_cast<uint8_t>(result)};
}

namespace greenboy::instructions {
Add::Add(std::shared_ptr<data_access::ByteAccess> lhs,
         std::shared_ptr<data_access::ByteAccess> rhs) noexcept
    : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

cycles Add::execute(CPU::RegisterSet &registers, MemoryBus &memory) const {
  auto result = ::Add(m_lhs->read(registers, memory),
                      m_rhs->read(registers, memory), registers.f);
  m_lhs->write(registers, memory, result);
  return m_lhs->access_time() + m_rhs->access_time();
}

} // namespace greenboy::instructions