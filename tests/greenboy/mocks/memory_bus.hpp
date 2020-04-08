#pragma once

#include "greenboy/memory_bus.hpp"
#include "gmock/gmock.h"

class MockMemoryBus : public greenboy::MemoryBus {
public:
  MOCK_METHOD(greenboy::byte, read, (greenboy::word), (const, override));
  MOCK_METHOD(void, write, (greenboy::word, greenboy::byte), (override));
};
