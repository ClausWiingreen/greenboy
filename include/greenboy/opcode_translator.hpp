#pragma once

#include "types.hpp"

namespace greenboy {
class Instruction;

class OpcodeTranslator {
public:
  OpcodeTranslator() = default;
  OpcodeTranslator(const OpcodeTranslator &) = delete;
  OpcodeTranslator(OpcodeTranslator &&) = delete;

  virtual ~OpcodeTranslator() = default;

  OpcodeTranslator &operator=(const OpcodeTranslator &) = delete;
  OpcodeTranslator &operator=(OpcodeTranslator &&) = delete;

  virtual const Instruction &translate(byte opcode) = 0;
};
} // namespace greenboy
