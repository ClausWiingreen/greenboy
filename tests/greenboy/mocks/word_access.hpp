#pragma once

#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"

class MockWordAccess : public greenboy::data_access::WordAccess {
public:
  MOCK_METHOD(greenboy::word, read,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &),
              (const, override));
  MOCK_METHOD(void, write,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &,
               greenboy::word),
              (override));
  MOCK_METHOD(greenboy::cycles, access_time, (), (const, override));
};
