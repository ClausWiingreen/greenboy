#pragma once

#include "greenboy/cpu.hpp"
#include "gmock/gmock.h"

class MockCPU : public greenboy::CPU {
public:
  MOCK_METHOD(greenboy::cycles, update, (), (override));
};
