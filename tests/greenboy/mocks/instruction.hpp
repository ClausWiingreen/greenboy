#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"

class MockInstruction : public greenboy::Instruction {
public:
  MOCK_METHOD(greenboy::cycles, execute,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &),
              (const, override));
};
