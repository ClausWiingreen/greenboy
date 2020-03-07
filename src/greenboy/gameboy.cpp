#include "greenboy/gameboy.hpp"

namespace greenboy {
Gameboy::Gameboy(std::unique_ptr<CPU> cpu, std::unique_ptr<Video> video)
    : m_cpu{std::move(cpu)}, m_video(std::move(video)) {
  if (!m_cpu) {
    throw std::invalid_argument("cpu cannot be null");
  }
  if (!m_video) {
    throw std::invalid_argument("video cannot be null");
  }
}
void Gameboy::step() {
  auto time_passed = m_cpu->update();
  m_video->advance(time_passed);
}
} // namespace greenboy
