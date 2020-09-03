#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
class AddedWord final : public WordAccess {
  std::shared_ptr<WordAccess> m_lhs;
  std::shared_ptr<WordAccess> m_rhs;

public:
  AddedWord(std::shared_ptr<WordAccess> lhs, std::shared_ptr<WordAccess> rhs);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override {
    return m_lhs->access_time() + m_rhs->access_time();
  }

  static std::shared_ptr<AddedWord> from(std::shared_ptr<WordAccess> lhs,
                                         std::shared_ptr<WordAccess> rhs) {
    return std::make_shared<AddedWord>(std::move(lhs), std::move(rhs));
  }
};
} // namespace greenboy::data_access
