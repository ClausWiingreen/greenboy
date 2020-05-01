#pragma once

#include "word_access.hpp"

#include <memory>

namespace greenboy::data_access {
class ByteAccess;

class DoubleByteWord final : public WordAccess {
  std::shared_ptr<ByteAccess> m_high;
  std::shared_ptr<ByteAccess> m_low;

public:
  DoubleByteWord(std::shared_ptr<ByteAccess> high,
                 std::shared_ptr<ByteAccess> low);

  word read(CPU::RegisterSet &registers, MemoryBus &memory) const override;
  void write(CPU::RegisterSet &registers, MemoryBus &memory,
             word value) override;
  cycles access_time() const override;

  static std::shared_ptr<DoubleByteWord> from(std::shared_ptr<ByteAccess> high,
                                              std::shared_ptr<ByteAccess> low);
};
} // namespace greenboy::data_access
