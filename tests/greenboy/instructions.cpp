#include "greenboy/instruction.hpp"
#include "mocks/byte_access.hpp"
#include "mocks/memory_bus.hpp"
#include "mocks/word_access.hpp"
#include "gtest/gtest.h"

#include "greenboy/instructions/byte_load.hpp"
#include "greenboy/instructions/word_load.hpp"

namespace {
using namespace greenboy;
using namespace instructions;
using namespace data_access;
using ::testing::_;
using ::testing::Return;

TEST(ByteLoad, TakesTheReadValueFromSourceAndWritesItToDestination) {
  auto source = std::make_shared<MockByteAccess>();
  auto destination = std::make_shared<MockByteAccess>();
  CPU::RegisterSet registers{};
  MockMemoryBus memory;

  EXPECT_CALL(*source, read(_, _)).WillOnce(Return(byte{0x24}));
  EXPECT_CALL(*destination, write(_, _, byte{0x24}));

  ByteLoad{destination, source}.execute(registers, memory);
}

TEST(WordLoad, TakesTheReadValueFromSourceAndWritesItToDestination) {
  auto source = std::make_shared<MockWordAccess>();
  auto destination = std::make_shared<MockWordAccess>();
  CPU::RegisterSet registers{};
  MockMemoryBus memory;

  EXPECT_CALL(*source, read(_, _)).WillOnce(Return(word{0x2480}));
  EXPECT_CALL(*destination, write(_, _, word{0x2480}));

  WordLoad{destination, source}.execute(registers, memory);
}
} // namespace