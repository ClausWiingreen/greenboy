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

class MockVideo : public Video {
public:
  MOCK_METHOD(void, advance, (cycles c), (override));
};

TEST(Step, CallsCPUUpdate) {
  auto cpu = std::make_unique<MockCPU>();
  auto video = std::make_unique<MockVideo>();
  
  EXPECT_CALL(*cpu, update()).WillOnce(Return(cycles{1}));
  
  Gameboy gameboy{std::move(cpu), std::move(video)};

  gameboy.step();
}

TEST(Step, CallsVideoAdvance) {
  auto cpu = std::make_unique<MockCPU>();
  auto video = std::make_unique<MockVideo>();

  EXPECT_CALL(*cpu, update()).WillOnce(Return(cycles{1})).WillOnce(Return(cycles{2}));
  EXPECT_CALL(*video, advance(cycles{1}));
  EXPECT_CALL(*video, advance(cycles{2}));
  
  Gameboy gameboy{std::move(cpu), std::move(video)};

  gameboy.step();
  gameboy.step();
}
} // namespace
