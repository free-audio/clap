#pragma once

#include <clap/all.h>

namespace synaesthesis {
   class Processor {
   public:
      Processor(int sampleRate);

      virtual void process(uint32_t voiceIndex) = 0;

    protected:
      const int sampleRate_;
   };
} // namespace synaesthesis