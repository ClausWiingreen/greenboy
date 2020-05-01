#pragma once
#include "greenboy/instruction.hpp"

#include <memory>

namespace greenboy {
namespace data_access {
class WordAccess;
} // namespace data_access

namespace instructions {
class WordLoad final : public Instruction {
  std::shared_ptr<data_access::WordAccess> m_destination;
  std::shared_ptr<const data_access::WordAccess> m_source;

public:
  WordLoad(std::shared_ptr<data_access::WordAccess> dest,
           std::shared_ptr<const data_access::WordAccess> src);
  cycles execute(CPU::RegisterSet &registers, MemoryBus &memory) const override;
};
} // namespace instructions
} // namespace greenboy