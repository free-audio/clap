#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace clap {
   template <size_t CAPACITY>
   class Buffer {

      const uint8_t *readData() const noexcept { return &data_[roff_]; }
      size_t readAvail() const noexcept { return woff_ - roff_; }
      void read(size_t bytes) noexcept { roff_ += bytes; }

      uint8_t *writeData() const noexcept { return &data_[woff_]; }
      size_t writeAvail() const noexcept { return CAPACITY - woff_; }
      void wrote(size_t bytes) noexcept { woff_ += bytes; }

      void rewind() {
         if (woff_ == 0)
            return;

         // this is inefficient but simple
         // TODO: use scatter/gather IO
         std::memmove(&data_[0], &data_[roff_], woff_ - roff_);
         woff_ -= roff_;
         roff_ = 0;
      }

      std::array<uint8_t, CAPACITY> data_;
      size_t roff_ = 0;
      size_t woff_ = 0;
   };
} // namespace clap