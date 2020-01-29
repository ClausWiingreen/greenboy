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

TEST(GameboyCtor, RejectsNullCPU) {
  auto cpu = std::make_unique<MockCPU>();
  EXPECT_THROW(Gameboy(std::move(cpu), nullptr), std::invalid_argument);
}

TEST(GameboyCtor, RejectsNullValues) {
  auto video = std::make_unique<MockVideo>();
  EXPECT_THROW(Gameboy(nullptr, std::move(video)), std::invalid_argument);
}

TEST(GameboyCtor, AcceptedConfiguration) {
  auto cpu = std::make_unique<MockCPU>();
  auto video = std::make_unique<MockVideo>();
  EXPECT_NO_THROW(Gameboy(std::move(cpu), std::move(video)));
}

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

  EXPECT_CALL(*cpu, update())
      .WillOnce(Return(cycles{1}))
      .WillOnce(Return(cycles{2}));
  EXPECT_CALL(*video, advance(cycles{1}));
  EXPECT_CALL(*video, advance(cycles{2}));

  Gameboy gameboy{std::move(cpu), std::move(video)};

  gameboy.step();
  gameboy.step();
}
} // namespace
