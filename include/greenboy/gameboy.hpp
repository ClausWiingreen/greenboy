#pragma once
#include <memory>

#include "cpu.hpp"
#include "video.hpp"

namespace greenboy {
/**
 * the interface between user and core emulator.
 */
class Gameboy {
  std::unique_ptr<CPU> m_cpu;
  std::unique_ptr<Video> m_video;

public:
  /**
   * The main constructor for the Gameboy.
   * It provides opertunities for dependency injection.
   * 
   * \param cpu A pointer to an instance of the CPU interface.
   * \param video A pointer to an instance of the Video interface.
   */
  Gameboy(std::unique_ptr<CPU> cpu, std::unique_ptr<Video> video);

  /**
   * Advance the emulation by the smallest step possible.
   * 
   * \callgraph
   */
  void step();
};
} // namespace greenboy
