#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"

class MockWordAccess : public greenboy::instructions::WordAccess {
public:
  MOCK_METHOD(greenboy::word, read,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &),
              (const, override));
  MOCK_METHOD(void, write,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &,
               greenboy::word),
              (override));
};
