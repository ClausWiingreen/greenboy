#include "greenboy/opcode_translator.hpp"
#include "gmock/gmock.h"

class MockOpcodeTranslator : public greenboy::OpcodeTranslator {
public:
  MOCK_METHOD(greenboy::Instruction &, translate, (greenboy::byte), (override));
};
