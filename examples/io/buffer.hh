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

      const T *readPtr() const noexcept { return &data_[roff_]; }
      size_t readAvail() const noexcept { return woff_ - roff_; }

      T *writePtr() noexcept { return &data_[woff_]; }
      size_t writeAvail() const noexcept { return CAPACITY - woff_; }

      /* Consume nbytes from the buffer */
      void read(size_t nbytes) noexcept {
         roff_ += nbytes;
         assert(checkInvariants());
      }

      /* Produce nbytes into the buffer */
      void wrote(size_t nbytes) noexcept {
         woff_ += nbytes;
         assert(checkInvariants());
      }

      void write(const T *&data, size_t &size) {
         size_t avail = writeAvail();
         avail = std::min(size, avail);
         auto end = data + avail;
         std::copy(data, end, writePtr());
         wrote(size);
         data = end;
         size -= avail;
      }

      void rewind() noexcept {
         if (woff_ == 0)
            return;

         // this is inefficient but simple
         // TODO: use scatter/gather IO
         auto rptr = readPtr();
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