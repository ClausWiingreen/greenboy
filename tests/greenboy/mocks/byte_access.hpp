#pragma once

#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"

class MockByteAccess : public greenboy::data_access::ByteAccess {
public:
  MOCK_METHOD(greenboy::byte, read,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &),
              (const, override));
  MOCK_METHOD(void, write,
              (greenboy::CPU::RegisterSet &, greenboy::MemoryBus &,
               greenboy::byte),
              (override));
  MOCK_METHOD(cycles, access_time, (), (const, override));
};
