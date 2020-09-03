#include "greenboy/data_access/added_word.hpp"

#include <stdexcept>

namespace greenboy::data_access {
void AddedWord::write(CPU::RegisterSet & /* registers */,
                      MemoryBus & /* memory */, word /* value */) {
  throw std::runtime_error("Tried to write to an added word");
}
AddedWord::AddedWord(std::shared_ptr<WordAccess> lhs,
                     std::shared_ptr<WordAccess> rhs)
    : m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
word AddedWord::read(CPU::RegisterSet &registers, MemoryBus &memory) const {
  const auto a = m_lhs->read(registers, memory);
  const auto b = m_rhs->read(registers, memory);
  const int res = a + b;
  const auto carry = res ^ a ^ b;

  registers.f.negate = false;
  registers.f.half_carry = (carry & (1 << 12)) != 0;
  registers.f.carry = (carry & (1 << 16)) != 0;

  return static_cast<word>(res);
}
} // namespace greenboy::data_access
