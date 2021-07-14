#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

namespace clap {
   template <typename T, size_t CAPACITY>
   class Buffer {
   public:
      Buffer() { assert(checkInvariants()); }

      Buffer(const Buffer<T, CAPACITY> &) = delete;
      Buffer(Buffer<T, CAPACITY> &&) = delete;
      Buffer<T, CAPACITY> &operator=(const Buffer<T, CAPACITY> &) = delete;
      Buffer<T, CAPACITY> &operator=(Buffer<T, CAPACITY> &&) = delete;

      const T *readData() const noexcept { return &data_[roff_]; }
      size_t readAvail() const noexcept { return woff_ - roff_; }
      void read(size_t bytes) noexcept {
         roff_ += bytes;
         assert(checkInvariants());
      }

      void write(const T *&data, size_t &size) {
         auto avail = std::min(size, writeAvail());
         auto end = data + avail;
         std::copy(data, data + avail, writeData());
         data = end;
         size -= avail;
      }
      T *writeData() noexcept { return &data_[woff_]; }
      size_t writeAvail() const noexcept { return CAPACITY - woff_; }
      void wrote(size_t bytes) noexcept {
         woff_ += bytes;
         assert(checkInvariants());
      }

      void rewind() noexcept {
         if (woff_ == 0)
            return;

         // this is inefficient but simple
         // TODO: use scatter/gather IO
         auto rptr = readData();
         auto avail = readAvail();
         std::copy(rptr, rptr + avail, &data_[0]);

         woff_ -= roff_;
         roff_ = 0;

         assert(checkInvariants());
      }

   private:
#ifndef NDEBUG
      bool checkInvariants() const noexcept {
         assert(woff_ <= data_.size());
         assert(roff_ <= woff_);
         return true;
      }
#endif

      std::array<T, CAPACITY> data_;
      size_t roff_ = 0;
      size_t woff_ = 0;
   };
} // namespace clap