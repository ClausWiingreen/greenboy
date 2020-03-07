#include "greenboy/fetch_execute_cpu.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace {
using namespace greenboy;
using ::testing::_;

class MockMemoryBus : public MemoryBus {
public:
  MOCK_METHOD(byte, read, (word), (override));
  MOCK_METHOD(void, write, (word, byte), (override));
};

TEST(FetchExecuteCPUCtor, RejectsNullMemoryBus) {
  ASSERT_THROW(FetchExecuteCPU{nullptr}, std::invalid_argument);
}

TEST(FetchExecuteCPUUpdate, FetchesFromMemory) {
  auto memory = std::make_unique<MockMemoryBus>();
  EXPECT_CALL(*memory, read(_));

  FetchExecuteCPU cpu{std::move(memory)};

  cpu.update();
}

TEST(FetchExecuteCPUUpdate, FirstFetchIsFromAddress0) {
  auto memory = std::make_unique<MockMemoryBus>();
  EXPECT_CALL(*memory, read(toWord(0x0000)));

  FetchExecuteCPU cpu{std::move(memory)};

  cpu.update();
}
} // namespace