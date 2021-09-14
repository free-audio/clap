#pragma once

#include <array>
#include <atomic>
#include <functional>

#include <clap/all.h>

template <typename T, size_t CAPACITY>
class ParamQueue {
public:
   using value_type = T;

   ParamQueue() = default;

   void push(const T &value) {
      int w;  // write element
      int wn; // next write element

      do {
         w = _writeOffset.load();
         wn = (w + 1) % CAPACITY;
      } while (wn == _readOffset); // wait until there is space

      _data[w] = value;
      _writeOffset = wn;
   }

   bool tryPop(T& value) {
      int r = _readOffset;

      if (r == _writeOffset)
         return false; // empty

      int rn = (r + 1) % CAPACITY;
      value = _data[r];
      _readOffset = rn;
      return true;
   }

   void reset()
   {
      _writeOffset = 0;
      _readOffset = 0;
   }

private:
   std::array<T, CAPACITY> _data;
   std::atomic<int> _writeOffset = {0};
   std::atomic<int> _readOffset = {0};
};