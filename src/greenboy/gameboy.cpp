#include "greenboy/gameboy.hpp"

#include <cassert>

#include "greenboy/cpu.hpp"
#include "greenboy/video.hpp"

namespace greenboy {
Gameboy::Gameboy(std::unique_ptr<CPU> cpu,
                 std::unique_ptr<Video> video) noexcept
    : m_cpu{std::move(cpu)}, m_video(std::move(video)) {
  assert(m_cpu != nullptr);
  assert(m_video != nullptr);
}
void Gameboy::step() {
  const auto time_passed = m_cpu->update();
  m_video->advance(time_passed);
}
} // namespace greenboy
