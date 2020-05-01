#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
class WordRegister final : public WordAccess {
  CPU::R16 m_reg;

public:
  explicit WordRegister(CPU::R16 reg);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;

  static std::shared_ptr<WordRegister> bc();
  static std::shared_ptr<WordRegister> de();
  static std::shared_ptr<WordRegister> hl();
  static std::shared_ptr<WordRegister> sp();
};
} // namespace greenboy::data_access
