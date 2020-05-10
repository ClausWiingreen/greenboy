#pragma once
#include <functional>
#include <memory>

#include "greenboy/instruction.hpp"

namespace greenboy {
namespace data_access {
class ByteAccess;
}

namespace instructions {
class ByteArithmeticOperation final : public Instruction {
  std::shared_ptr<data_access::ByteAccess> m_lhs;
  std::shared_ptr<const data_access::ByteAccess> m_rhs;
  std::function<byte(const byte, const byte, CPU::Flags &)> m_operation;

public:
  ByteArithmeticOperation(
      std::shared_ptr<data_access::ByteAccess> lhs,
      std::shared_ptr<const data_access::ByteAccess> rhs,
      std::function<byte(const byte, const byte, CPU::Flags &)>
          operation) noexcept;

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};

namespace operations {
byte add(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte add_with_carry(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte subtract(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte subtract_with_carry(const byte lhs, const byte rhs,
                         CPU::Flags &f) noexcept;
byte bitwise_and(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte bitwise_or(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte bitwise_xor(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte compare(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte increment(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
byte decrement(const byte lhs, const byte rhs, CPU::Flags &f) noexcept;
} // namespace operations
} // namespace instructions
} // namespace greenboy
