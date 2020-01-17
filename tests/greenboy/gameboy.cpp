#include "greenboy/gameboy.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using namespace greenboy;
using ::testing::Return;

class MockCPU : public CPU {
public:
  MOCK_METHOD(cycles, update, (), (override));
};

TEST(Step, CallsCPUUpdate) {
  auto cpu = std::make_unique<MockCPU>();
  EXPECT_CALL(*cpu, update()).WillOnce(Return(cycles{1}));
  Gameboy gameboy{std::move(cpu)};

  gameboy.step();
}
} // namespace
