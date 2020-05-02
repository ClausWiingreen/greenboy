#pragma once
#include "greenboy/instruction.hpp"

#include <memory>

namespace greenboy {
namespace data_access {
class ByteAccess;
}
namespace instructions {
class Add final : public Instruction {
  std::shared_ptr<data_access::ByteAccess> m_lhs;
  std::shared_ptr<data_access::ByteAccess> m_rhs;

public:
  Add(std::shared_ptr<data_access::ByteAccess> rhs,
      std::shared_ptr<data_access::ByteAccess> lhs) noexcept;
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};
} // namespace instructions
} // namespace greenboy
