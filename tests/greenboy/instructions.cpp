#include "greenboy/instruction.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using namespace greenboy;
using namespace instructions;
using ::testing::_;

class MockMemoryBus : public MemoryBus {
public:
  MOCK_METHOD(byte, read, (word), (override));
  MOCK_METHOD(void, write, (word, byte), (override));
};

TEST(NOP, Timing) {
  CPU::RegisterSet registers{};
  MockMemoryBus memory;
  EXPECT_CALL(memory, read(_)).Times(0);
  EXPECT_CALL(memory, write(_, _)).Times(0);

  auto time_passed = NOP{}.execute(registers, memory);

  EXPECT_EQ(time_passed, cycles{4});
  EXPECT_EQ(registers, CPU::RegisterSet{})
      << "Registers should not have changed";
}
} // namespace