#pragma once
#include <memory>

#include "greenboy/instruction.hpp"

namespace greenboy {
namespace data_access {
class ByteAccess;
}

namespace instructions {
class ByteLoad final : public Instruction {
  std::shared_ptr<data_access::ByteAccess> m_destination;
  std::shared_ptr<const data_access::ByteAccess> m_source;

public:
  ByteLoad(std::shared_ptr<data_access::ByteAccess> dest,
           std::shared_ptr<const data_access::ByteAccess> src);

  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};
} // namespace instructions
} // namespace greenboy
