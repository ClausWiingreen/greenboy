#pragma once

#include "timing.hpp"

namespace greenboy {
/** 
 * a well defined interface for Video emulation.
 */
class Video {
public:
  Video() = default;
  Video(const Video&) = delete;
  Video(Video&&) = delete;

  virtual ~Video() = default;
  
  Video& operator=(const Video&) = delete;
  Video& operator=(Video&&) = delete;

  /**
   * Advances the state of the emulation by the number of cycles.
   *
   * @param c the number of cycles to advance by 
   */
  virtual void advance(cycles c) = 0;
};
} // namespace greenboy
