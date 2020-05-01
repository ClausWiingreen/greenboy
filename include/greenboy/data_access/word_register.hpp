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

  [[nodiscard]] static std::shared_ptr<WordRegister> bc();
  [[nodiscard]] static std::shared_ptr<WordRegister> de();
  [[nodiscard]] static std::shared_ptr<WordRegister> hl();
  [[nodiscard]] static std::shared_ptr<WordRegister> sp();
};

inline std::shared_ptr<WordRegister> WordRegister::bc() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::BC);
  return instance;
}
inline std::shared_ptr<WordRegister> WordRegister::de() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::DE);
  return instance;
}
inline std::shared_ptr<WordRegister> WordRegister::hl() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::HL);
  return instance;
}
inline std::shared_ptr<WordRegister> WordRegister::sp() {
  static auto instance = std::make_shared<WordRegister>(CPU::R16::SP);
  return instance;
}
} // namespace greenboy::data_access
