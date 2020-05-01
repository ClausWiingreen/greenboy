#pragma once
#include <memory>

namespace greenboy {
class CPU;
class Video;

class Gameboy {
  const std::unique_ptr<CPU> m_cpu;
  const std::unique_ptr<Video> m_video;

public:
  Gameboy(std::unique_ptr<CPU> cpu, std::unique_ptr<Video> video) noexcept;

  void step();
};
} // namespace greenboy
