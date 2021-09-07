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

      const T *readPtr() const noexcept { return &_data[_roff]; }
      size_t readAvail() const noexcept { return _woff - _roff; }

      T *writePtr() noexcept { return &_data[_woff]; }
      size_t writeAvail() const noexcept { return CAPACITY - _woff; }

      /* Consume nbytes from the buffer */
      void read(size_t nbytes) noexcept {
         _roff += nbytes;
         assert(checkInvariants());
      }

      /* Produce nbytes into the buffer */
      void wrote(size_t nbytes) noexcept {
         _woff += nbytes;
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
         if (_woff == 0)
            return;

         // this is inefficient but simple
         // TODO: use scatter/gather IO
         auto rptr = readPtr();
         auto avail = readAvail();
         std::copy(rptr, rptr + avail, &_data[0]);

         _woff -= _roff;
         _roff = 0;

         assert(checkInvariants());
      }

   private:
#ifndef NDEBUG
      bool checkInvariants() const noexcept {
         assert(_woff <= _data.size());
         assert(_roff <= _woff);
         return true;
      }
#endif

      std::array<T, CAPACITY> _data;
      size_t _roff = 0;
      size_t _woff = 0;
   };
} // namespace clap