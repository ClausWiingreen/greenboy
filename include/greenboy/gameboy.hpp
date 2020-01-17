#pragma once
#include <memory>

#include "cpu.hpp"

namespace greenboy {
class Gameboy {
  std::unique_ptr<CPU> m_cpu;

public:
  explicit Gameboy(std::unique_ptr<CPU> cpu);
  void step();
};
} // namespace greenboy
