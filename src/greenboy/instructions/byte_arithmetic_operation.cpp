#include "greenboy/instructions/byte_arithmetic_operation.hpp"

#include "greenboy/data_access/byte_access.hpp"

namespace greenboy::instructions {
ByteArithmeticOperation::ByteArithmeticOperation(
    std::shared_ptr<data_access::ByteAccess> lhs,
    std::shared_ptr<const data_access::ByteAccess> rhs,
    std::function<byte(const byte, const byte, CPU::Flags &)>
        operation) noexcept
    : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)),
      m_operation(std::move(operation)) {}

cycles ByteArithmeticOperation::execute(CPU::RegisterSet &registers,
                                        MemoryBus &memory) const {
  auto result = m_operation(m_lhs->read(registers, memory),
                            m_rhs->read(registers, memory), registers.f);
  m_lhs->write(registers, memory, result);
  return m_lhs->access_time() + m_rhs->access_time();
}

namespace operations {
byte add(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {
  const auto a = std::to_integer<unsigned>(lhs);
  const auto b = std::to_integer<unsigned>(rhs);
  auto result = a + b;
  const auto carry = result ^ a ^ b;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = (carry & (1u << 4u)) != 0;
  f.carry = (carry & (1u << 8u)) != 0;

  return byte{static_cast<uint8_t>(result)};
}
byte add_with_carry(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {
  const auto a = std::to_integer<unsigned>(lhs);
  const auto b = std::to_integer<unsigned>(rhs);
  auto result = a + b + (f.carry ? 1 : 0);
  const auto carry = result ^ a ^ b;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = (carry & (1u << 4u)) != 0;
  f.carry = (carry & (1u << 8u)) != 0;

  return byte{static_cast<uint8_t>(result)};
}
byte subtract(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {
  const auto a = std::to_integer<unsigned>(lhs);
  const auto b = std::to_integer<unsigned>(rhs);
  auto result = a - b;
  const auto borrow = result ^ a ^ b;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = true;
  f.half_carry = (borrow & (1u << 4u)) != 0;
  f.carry = (borrow & (1u << 8u)) != 0;

  return byte{static_cast<uint8_t>(result)};
}
byte subtract_with_carry(const byte lhs, const byte rhs,
                         CPU::Flags &f) noexcept {
  const auto a = std::to_integer<unsigned>(lhs);
  const auto b = std::to_integer<unsigned>(rhs);
  auto result = a - b - (f.carry ? 1 : 0);
  const auto borrow = result ^ a ^ b;
  result &= 0xff;

  f.zero = result == 0;
  f.negate = true;
  f.half_carry = (borrow & (1u << 4u)) != 0;
  f.carry = (borrow & (1u << 8u)) != 0;

  return byte{static_cast<uint8_t>(result)};
}
byte bitwise_and(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {

  auto a = std::to_integer<unsigned>(lhs);
  auto b = std::to_integer<unsigned>(rhs);
  auto result = a & b;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = true;
  f.carry = false;

  return byte{static_cast<uint8_t>(result)};
}
byte bitwise_or(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {

  auto a = std::to_integer<unsigned>(lhs);
  auto b = std::to_integer<unsigned>(rhs);
  auto result = a | b;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = false;
  f.carry = false;

  return byte{static_cast<uint8_t>(result)};
}
byte bitwise_xor(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {

  auto a = std::to_integer<unsigned>(lhs);
  auto b = std::to_integer<unsigned>(rhs);
  auto result = a ^ b;

  f.zero = result == 0;
  f.negate = false;
  f.half_carry = false;
  f.carry = false;

  return byte{static_cast<uint8_t>(result)};
}
byte compare(const byte lhs, const byte rhs, CPU::Flags &f) noexcept {
  subtract(lhs, rhs, f);
  return lhs;
}
} // namespace operations
} // namespace greenboy::instructions
