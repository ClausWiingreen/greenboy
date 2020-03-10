#pragma once
#include "instruction.hpp"

namespace greenboy {
/**
 * @brief translates opcode to instructions
 *
 */
class OpcodeTranslator {
public:
  //! @cond Doxygen_Suppress
  OpcodeTranslator() = default;
  OpcodeTranslator(const OpcodeTranslator &) = delete;
  OpcodeTranslator(OpcodeTranslator &&) = delete;

  virtual ~OpcodeTranslator() = default;

  OpcodeTranslator &operator=(const OpcodeTranslator &) = delete;
  OpcodeTranslator &operator=(OpcodeTranslator &&) = delete;
  //! @endcond

  /**
   * @brief Translates the opcode to an instruction object.
   * Return type is reference type to allow the translator to cache results.
   *
   * @param opcode the opcode to be translated
   * @return the translated instruction
   */
  virtual const Instruction &translate(byte opcode) = 0;
};
} // namespace greenboy
