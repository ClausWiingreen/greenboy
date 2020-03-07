#include "greenboy/fetch_execute_cpu.hpp"

#include <stdexcept>

namespace greenboy {
FetchExecuteCPU::FetchExecuteCPU(std::unique_ptr<MemoryBus> memory)
    : m_memory(std::move(memory)) {
  if (!m_memory) {
    throw std::invalid_argument("memory may not be null");
  }
}

cycles FetchExecuteCPU::update() {
  m_memory->read(word{});
  return cycles{};
}
} // namespace greenboy
